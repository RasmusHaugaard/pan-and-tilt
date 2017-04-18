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

#endif /* _TMODEL_H_ */
