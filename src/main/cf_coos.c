#include <stdint.h>

#include <coos.h>

#include "build_config.h"
#include "drivers/system.h"
#include "main.h"

#define CONTROL_TASK_PRIO	0
#define CONTROL_TASK_STACK_SIZE	0x100

#define MAIN_TASK_PRIO	1
#define MAIN_TASK_STACK_SIZE	0x100

extern int16_t debug[4];

static OS_TID main_task_id;
static OS_TID control_task_id;
OS_FlagID controlLoopFlagID;

static void mainTask( void *pv )
{
	UNUSED(pv);

	/* run the regular mainLoop */
	mainLoop();
}

static void controlTask( void *pv )
{
	UNUSED(pv);

	/* run the controlLoop */
	while( 1 )
	{
		if ( CoWaitForSingleFlag( controlLoopFlagID, 0 ) == E_OK )
		{
			executeControlLoopTasks();
		}
	}
}

static void createCoosTasks( void )
{
	static OS_STK control_task_stack[CONTROL_TASK_STACK_SIZE] = {0};
	static OS_STK main_task_stack[MAIN_TASK_STACK_SIZE] = {0};

	control_task_id = CoCreateTask( controlTask, Co_NULL, CONTROL_TASK_PRIO, &control_task_stack[CONTROL_TASK_STACK_SIZE-1], CONTROL_TASK_STACK_SIZE );
	main_task_id = CoCreateTask( mainTask, Co_NULL, MAIN_TASK_PRIO, &main_task_stack[MAIN_TASK_STACK_SIZE-1], MAIN_TASK_STACK_SIZE );
}

static void initCoosResources( void )
{
	/* create the control loop flag */
	controlLoopFlagID = CoCreateFlag( 1, 0 );
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
		mainLoopTicker = 0;
		isr_SetFlag( controlLoopFlagID );
	}

	CoExitISR();
}

