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
#include "events.h"
#include "tmodel.h"
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
    if(get_msg_event(SEB_UI_EVENTS)==UIE_NEW_TARGET)
    {
        target = get_msg_data(SEB_UI_EVENTS);
        sum_error = 0;
        prev_error = 0;
    }


    FP32 si_position = curr_position * RAD_PR_TICK;
    FP32 si_target = target * RAD_PR_TICK;

    FP32 error = si_target - si_position;

    pwm_procent = Kp * error;

    sum_error += error*delta_t;
    pwm_procent += Ki * sum_error;

    pwm_procent += Kd * (error - prev_error)/delta_t;

    pwm_procent /= MAX_VOLTAGE;
    pwm_procent *= 127;

    if(pwm_procent > 0)
        pwm_procent += 0.5;
    else if(pwm_procent < 0)
        pwm_procent -= 0.5;

    INT16S result=(INT16S)pwm_procent;

    if(result < -128)
        result = -128;
    else if(result > 127)
        result = 127;

    prev_error = error;
    put_msg_event(SEB_PWM_EVENTS, PIDE_NEW_PWM);
    put_msg_data(SEB_PWM_EVENTS, result);
}

/****************************** End Of Module *******************************/




