/*****************************************************************************
* University of Southern Denmark
* Embedded Programming (EMP)
*
* MODULENAME.: matlab.h
*
* PROJECT....: Pan-and-tilt
*
* DESCRIPTION: MatLab interface.
*
* Change Log:
******************************************************************************
* Date    Id    Change
* YYMMDD
* --------------------
* 170331  MBJ    Module created.
*
*****************************************************************************/

#ifndef MATLAB_H_
#define MATLAB_H_

/***************************** Include files *******************************/

/*****************************    Defines    *******************************/

/*****************************   Constants   *******************************/

/*****************************   Functions   *******************************/
void matlab_task(INT8U my_id, INT8U my_state, INT8U event, INT8U data);
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : Send and receive data from MatLab according to the protocol.
******************************************************************************/

/****************************** End Of Module *******************************/


#endif /* MATLAB_H_ */
