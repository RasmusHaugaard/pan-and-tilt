/*****************************************************************************
* University of Southern Denmark
* Embedded Programming (EMP)
*
* MODULENAME.: accelerometer.h
*
* PROJECT....: Pan-and-tilt
*
* DESCRIPTION: Accelerometer interface.
*
* Change Log:
******************************************************************************
* Date    Id    Change
* YYMMDD
* --------------------
* 170407  MBJ    Module created.
*
*****************************************************************************/

#ifndef ACCELEROMETER_H_
#define ACCELEROMETER_H_

/***************************** Include files *******************************/

/*****************************    Defines    *******************************/

/*****************************   Constants   *******************************/

/*****************************   Functions   *******************************/
void init_accelerometer();
void update_acc_x0(INT8U ssi0_data);
void update_acc_x1(INT8U ssi0_data);
void update_acc_y0(INT8U ssi0_data);
void update_acc_y1(INT8U ssi0_data);
void update_acc_z0(INT8U ssi0_data);
void update_acc_z1(INT8U ssi0_data);
void accelerometer_task(INT8U my_id, INT8U my_state, INT8U event, INT8U data);
FP32 filter(FP32 acc_data, FP32 *acc_data_prev);
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : Collects accelerometer data from the ADXL345Z.
******************************************************************************/

/****************************** End Of Module *******************************/


#endif /* ACCELEROMETER_H_ */
