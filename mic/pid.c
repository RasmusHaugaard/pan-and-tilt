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
#define MAX_PIDS     2
#define AVG_FILTER_SIZE 10
#define MAX_VOLTAGE 12

typedef struct{
    FP32 Kp;
    FP32 Ki;
    FP32 Kd;
    FP32 delta_t;
    FP32 prev_error;
    FP32 sum_error;
    FP32 delta_error_values[AVG_FILTER_SIZE];
    INT8U filter_index;
} Pid_controller;

Pid_controller pids[MAX_PIDS];

/***************************** Functions ******************************/
INT8U create_pid(FP32 Kp, FP32 Ki, FP32 Kd, FP32 delta_t)
{
    static INT8U next_id = 0;
    INT8U id = next_id++;
    if (id < MAX_PIDS){
        pids[id].Kp = Kp;
        pids[id].Ki = Ki;
        pids[id].Kd = Kd;
        pids[id].delta_t = delta_t;
        pids[id].prev_error = 0;
        pids[id].sum_error = 0;
        pids[id].filter_index = 0;
        for (INT8U i = 0; i < AVG_FILTER_SIZE; i++)
            pids[id].delta_error_values[i] = 0;
    }
    return id;
}


FP32 pid_next(INT8U id, FP32 process_variable, FP32 set_point)
{
    if (id >= MAX_PIDS)
        return 0;

    FP32 error = set_point - process_variable;
    if((pids[id].sum_error*pids[id].Ki < MAX_VOLTAGE && pids[id].sum_error*pids[id].Ki > -MAX_VOLTAGE) ||
            (pids[id].sum_error < 0 && error > 0) ||
            (pids[id].sum_error > 0 && error < 0))
        pids[id].sum_error += error * pids[id].delta_t;

    pids[id].delta_error_values[pids[id].filter_index] = error - pids[id].prev_error;
    pids[id].filter_index %= AVG_FILTER_SIZE;

    FP32 filtered_delta_error = 0;
    for (INT8U i = 0; i < AVG_FILTER_SIZE; i++)
        filtered_delta_error += pids[id].delta_error_values[i];
    filtered_delta_error /= AVG_FILTER_SIZE;

    FP32 output = pids[id].Kp * error;
    output += pids[id].Ki * pids[id].sum_error;
    output += pids[id].Kd * filtered_delta_error / pids[id].delta_t;

    pids[id].prev_error = error;
    return output;
}

/****************************** End Of Module *******************************/
