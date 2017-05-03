/***************************************************************************
* University of Southern Denmark
* Embedded Programming (EMP)
*
* MODULENAME.: PID.h
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
#ifndef PID_H_
#define PID_H_
/*****************************    Defines    *******************************/

/*****************************   Functions   *******************************/
INT8U create_pid(FP32 Kp, FP32 Ki, FP32 Kd, FP32 delta_t);
FP32 pid_next(INT8U id, FP32 si_position, FP32 si_target);

/****************************** End Of Module ******************************/
#endif
