/*****************************************************************************
* University of Southern Denmark
* Embedded C Programming (ECP)
*
* MODULENAME.: tmodel.h
*
* PROJECT....: ECP
*
* DESCRIPTION: Defines the elemtn of the task model..
*
* Change Log:
******************************************************************************
* Date    Id    Change
* YYMMDD
* --------------------
* 101004  MoH   Module created.
*
*****************************************************************************/

#ifndef _TMODEL_H_
#define _TMODEL_H_

#include "rtcs.h"

// Tasks.
// ------
#define TASK_UART_RX        USER_TASK
#define TASK_UART_TX        USER_TASK+1
#define TASK_SPI_TX         USER_TASK+2
#define TASK_SPI_RX         USER_TASK+3
#define TASK_MATLAB         USER_TASK+4
#define TASK_ENCODER        USER_TASK+5
#define TASK_MATLAB_ENCODER USER_TASK+6
#define TASK_UI             USER_TASK+7
#define TASK_PID            USER_TASK+8


// Interrupt Service Routines.
// ---------------------------
#define ISR_TIMER 21

// Semaphores.
// -----------
#define SEM_SPI_AVAILABLE   USER_SEM

// Shared State Memory.
// --------------------
#define SSM_PWM_LOW            31
#define SSM_PWM_HIGH           32
#define SSM_ENC_LOW            33
#define SSM_ENC_HIGH           33

//Shared Event Buffer.
//--------------------
#define SEB_UI_EVENTS          10
#define SEB_PWM_EVENTS         11

// QUEUEs.
// -------
#define Q_UART_TX 	USER_QUEUE
#define Q_UART_RX   USER_QUEUE+1
#define Q_SPI_TX    USER_QUEUE+2
#define Q_SPI_RX    USER_QUEUE+3

#endif /* _TMODEL_H_ */
