#include "pid.h"
 
void pid_reset(pid_t * const pid)
{
    // set prev and integrated error to zero
    pid->integral_sum = 0.0f;
    pid->previous_error_valid = 0;
    pid->previous_error[0] = pid->previous_error[1] = 0.0f;
}

void pid_set_gains( pid_t * const pid, float p_gain, float i_gain, float d_gain )
{
    pid->proportional_gain = p_gain;
    pid->integral_gain = i_gain;
    pid->derivative_gain = d_gain;
}

void pid_init( pid_t * const pid, pid_differential_mode differential_mode, float output_minimum, float output_maximum, float integral_limit )
{
    pid->differential_mode = differential_mode;
    pid->output_minimum = output_minimum;
    pid->output_maximum = output_maximum;
    pid->integral_limit = integral_limit;
    
    pid_reset(pid);
}
 
void pid_update(pid_t * const pid, float input, float setpoint, float dt)
{
    float const error = setpoint - input;
    float diff;
    float p_term;
    float i_term;
    float d_term;
    float temp; 
    p_term = (pid->proportional_gain * error);

    // integration with windup limiting
    pid->integral_sum += (error * dt);

    if (pid->integral_limit != 0.0f)
    {
        if (pid->integral_sum < -pid->integral_limit)
            pid->integral_sum = -pid->integral_limit;
        else if (pid->integral_sum > pid->integral_limit)
            pid->integral_sum = pid->integral_limit;
    }
 
    // scaling
    i_term = pid->integral_gain * pid->integral_sum;

    /* 
        limit integral error value to difference between limits and proportional term.
        There is no point in letting integral wind up any more once the output has been saturated.
    */
    if ( p_term >= pid->output_maximum )
    {
        i_term = 0;
        pid->integral_sum = 0.0f;
    }
    else if ( p_term > 0.0f && i_term > 0.0f )
    {
        /* calculate maximum allowed i_term value */
        temp = pid->output_maximum - p_term;
        if ( i_term > temp )
        {
            i_term = temp;
            /* update the integral error storage with the limited value */
            pid->integral_sum = i_term / pid->integral_gain;  /* pid->integral_gain must be != 0 for i_term to be != 0 */
        }
    }
    if ( p_term <= pid->output_minimum )
    {
        i_term = 0;
        pid->integral_sum = 0.0f;
    }
    else if ( p_term < 0.0f && i_term < 0.0f )
    {
        /* calculate minimum allowed i_term value */
        temp = pid->output_minimum - p_term;
        if ( i_term < temp )
        {
            i_term = temp;
            /* update the integral error storage with the limited value */
            pid->integral_sum = i_term / pid->integral_gain;  /* pid->integral_gain must be != 0 for i_term to be != 0 */
        }
    }
    
    // differentiation
    diff = 0.0f;
    if ( pid->differential_mode == differential_mode_normal )
    {
        if (pid->previous_error_valid == 1)
            diff = (error - pid->previous_error[0]) / dt;

        /* save the error value */
        pid->previous_error[0] = error;
        if ( pid->previous_error_valid == 0 )
            pid->previous_error_valid = 1;
    }
    else if ( pid->differential_mode == differential_mode_bumpless )
    {
        /* bumpless mode only checks the change in input value, not the change in error */
        if (pid->previous_error_valid == 2)
            diff = ((input - pid->previous_error[0]) - (pid->previous_error[0]-pid->previous_error[1])) /dt;

        /* save the error values */
        pid->previous_error[1] = pid->previous_error[0];
        pid->previous_error[0] = input;
        if ( pid->previous_error_valid < 2 )
            pid->previous_error_valid++;
    }

    d_term = pid->derivative_gain * diff;

    /* Sum the terms */
    pid->output_signal = p_term + i_term + d_term;

    /* output signal limiting */
    if ( pid->output_signal > pid->output_maximum )
        pid->output_signal = pid->output_maximum;
    if ( pid->output_signal < pid->output_minimum )
        pid->output_signal = pid->output_minimum;

}

