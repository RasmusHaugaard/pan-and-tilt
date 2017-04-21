/***************************************************************************
* University of Southern Denmark
* Embedded Programming (EMP)
*
* MODULENAME.: PID.c
*
* PROJECT....:
*
* DESCRIPTION:
*
* Change Log:
*****************************************************************************
* Date    Id    Change
* YYMMDD
* --------------------
* YYMMDD  RDK    Module created.
*
****************************************************************************/
/**************************** Include Libraries ****************************/
#include <stdint.h>
/***************************** Include files *******************************/
#include "emp_type.h"
#include "pid.h"
/*****************************    Defines    *******************************/

#define TICKS_PR_REV (360 * 3)
#define PI           3.14159
#define RAD_PR_TICK  2 * PI / TICKS_PR_REV
#define MAX_VOLTAGE  12
#define delta_t      0.01

FP32 Kp = 40.0;
FP32 Ki = 16;
FP32 Kd = 2;
/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/

FP32 prev_error = 0;
FP32 sum_error = 0;


#define AVG_FILTER_SIZE 10
FP32 delta_error_values[AVG_FILTER_SIZE];

/***************************** Start of Module ******************************/
INT8S pid_next_pwm(INT16S curr_position, INT16S target)
{
    FP32 si_position = curr_position * (FP32)RAD_PR_TICK;
    FP32 si_target = target * (FP32)RAD_PR_TICK;

    FP32 error = si_target - si_position;
    sum_error += error * delta_t;

    static INT8U avg_filter_i = 0;
    delta_error_values[avg_filter_i++] = error - prev_error;
    if (avg_filter_i == AVG_FILTER_SIZE)
        avg_filter_i = 0;
    static FP32 filtered_delta_error = 0;
    for (INT8U i = 0; i < AVG_FILTER_SIZE; i++)
        filtered_delta_error += delta_error_values[i];
    filtered_delta_error /= AVG_FILTER_SIZE;

    FP32 pwm = Kp * error;
    pwm += Ki * sum_error;
    pwm += Kd * filtered_delta_error / delta_t;

    prev_error = error;

    // Volt -> PWM
    pwm /= MAX_VOLTAGE;
    pwm *= 127;

    if(pwm > 0)
        pwm += 0.5;
    else if(pwm < 0)
        pwm -= 0.5;

    if(pwm < -128)
        pwm = -128;
    else if(pwm > 127)
        pwm = 127;

    return (INT8S) pwm;
}

/****************************** End Of Module *******************************/
