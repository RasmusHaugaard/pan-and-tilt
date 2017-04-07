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
#include "tm4c123gh6pm.h"
#include "emp_type.h"
/*****************************    Defines    *******************************/

#define TICKS_PR_REV 360
#define PI 3.14159
#define RAD_PR_TICK  2 * PI / TICKS_PR_REV
#define MAX_VOLTAGE 12

FP32 Kp = 100.5;
FP32 Ki = 14.2;
FP32 Kd = 3.7;
/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/

INT16S target = 0;        //In ticks
INT16S curr_position = 0; //In ticks

FP32 prev_error = 0;
FP32 sum_error = 0;

FP32 pwm_procent = 0;     //-1 to 1

/***************************** Start of Module ******************************/
void pid_task(INT8U my_id, INT8U my_state, INT8U event, INT8U data)
{
/****************************************************************************
*   Function : See .h file for information
*****************************************************************************/

    FP32 si_position = curr_position * RAD_PR_TICK;
    FP32 si_target = target * RAD_PR_TICK;

    FP32 error = si_target - si_position;

    pwm_procent = Kp * error;

    sum_error += error;
    pwm_procent += Ki * sum_error;

    pwm_procent += Kd * (error - prev_error);

    pwm_procent /= MAX_VOLTAGE;

    if(pwm_procent < -1)
        pwm_procent = -1;
    else if(pwm_procent > 1)
        pwm_procent = 1;

    prev_error = error;
}

/****************************** End Of Module *******************************/




