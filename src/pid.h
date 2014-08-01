#ifndef __PID_CONTROLLER__
#define __PID_CONTROLLER__

typedef enum pid_differential_mode
{
    differential_mode_normal,   /* changes in setpoint will give a differential bump */
    differential_mode_bumpless  /* changes in setpoint will not give a differential bump */
} pid_differential_mode;

typedef struct pid_t{
    pid_differential_mode   differential_mode;
    float                   output_minimum;         /* indicates the maximum value +ve and -ve the output signal should have */     
    float                   output_maximum;         /* indicates the maximum value +ve and -ve the output signal should have */     
    float                   integral_limit;         /* maximum integral value. */
    float                   base_dt;                /* base interval beween calls to pid_update(). Used to normalise I and D term calculations */
    
    float                   proportional_gain;
    float                   integral_gain;
    float                   derivative_gain;

    int                     previous_error_valid;   /* used to determine if differential term can be calculated */
    float                   previous_error[2];      /* we save the two previous input values so that we can calculate the change in input */

    float                   integral_sum;         /* used to store the current integral sum */

    float                   output_signal;
} pid_t;


/* 
    pid_init()
    
    Initialise the pid structure 
    pid:        The PID state info structure to initialise
    differential_mode: The mode to use for calculating the differential term.
                in bumpless mode, changes in setpoint do not result in a 'bump' in the differential term.
    output_minimum: Used to restrict integral windup as well as limit the actual output value.
    output_maximum: Used to restrict integral windup as well as limit the actual output value.
    integral_limit: Maximum integral windup storage (before it is multiplied by the i_gain)
*/
void pid_init( pid_t *pid, pid_differential_mode differential_mode, float output_minimum, float output_maximum, float integral_limit );

/*
    pid_reset()

    Reset internal state. Used to reset integral and differential state only. Does not performa a full initialise.
*/
void pid_reset(pid_t * const pid);

/*
    pid_set_gains()

    Set the PID gain values.
*/
void pid_set_gains( pid_t *pid, float p_gain, float i_gain, float d_gain );


/*
    pid_update()

    Perform PID processing.
    Should be called at a rate nominally equal to base_dt.
    Performs the PID operation given the previous PID state, the current input value, and the
    current setpoint.
*/
void pid_update(pid_t * const pid, float input, float setpoint, float dt);

#endif
