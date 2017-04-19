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

#define TICKS_PR_REV 360
#define PI           3.14159
#define RAD_PR_TICK  2 * PI / TICKS_PR_REV
#define MAX_VOLTAGE  12
#define delta_t      0.001

FP32 Kp = 14.05;
FP32 Ki = 16.86;
FP32 Kd = 4.777;
/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/

FP32 prev_error = 0;
FP32 sum_error = 0;

FP32 pwm;     //-1 to 1

/***************************** Start of Module ******************************/
INT8S pid_next_pwm(INT16S curr_position, INT16S target)
{
    FP32 si_position = curr_position * RAD_PR_TICK;
    FP32 si_target = target * RAD_PR_TICK;

    FP32 error = si_target - si_position;
    sum_error += error * delta_t;

    pwm = Kp * error;
    pwm += Ki * sum_error;
    pwm += Kd * (error - prev_error) / delta_t;

    prev_error = error;

    //TODO: Er det for at få det i volt??
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
