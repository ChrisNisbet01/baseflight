#include <stdint.h>

#include <coos.h>

#include "build_config.h"
#include "drivers/system.h"
#include "main.h"

#define MAIN_TASK_PRIO	0
#define MAIN_TASK_SIZE	0x100

extern int16_t debug[4];

static OS_TID main_task_id;
OS_FlagID mainLoopFlagID;

static void mainTask( void *pv )
{
	UNUSED(pv);

	/* run the regular mainLoop */
	mainLoop();
}

static void createCoosTasks( void )
{
	static OS_STK main_task_stack[MAIN_TASK_SIZE] = {0};

	main_task_id = CoCreateTask( mainTask, Co_NULL, MAIN_TASK_PRIO, &main_task_stack[MAIN_TASK_SIZE-1], MAIN_TASK_SIZE );
	// TODO: Low priority task
}

static void initCoosResources( void )
{
	/* create the main loop flag */
	mainLoopFlagID = CoCreateFlag( 1, 0 );
}

void initCoos( void )
{
	CoInitOS();
	initCoosResources();

}

void startCoos( void )
{
	createCoosTasks();

	CoStartOS();
}

void CoosSysTickHandler()
{
	static unsigned int mainLoopTicker;
	static unsigned int CoOSTicker;

	CoEnterISR();

	CoOSTicker++;
	if ( CoOSTicker >= SYSTICK_TICKS_PER_MILLISEC )	/* once per millisecond */
	{
		CoOSTicker = 0;
		CoOS_SysTick_Handler();
	}

	/* update the main loop counter */
	mainLoopTicker += SYSTICK_PERIOD_IN_USECS;
	if ( mainLoopTicker >= getMainLoopTimeCfg() )	/* once per configured loop time */
	{
		extern OS_FlagID mainLoopFlagID;

		static unsigned int last_micros;
		unsigned int now = micros();
		mainLoopTicker = 0;
		debug[1] = now - last_micros;
		last_micros = now;
		isr_SetFlag( mainLoopFlagID );
	}

	CoExitISR();
}

