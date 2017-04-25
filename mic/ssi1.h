/*****************************************************************************
* University of Southern Denmark
* Embedded Programming (EMP)
*
* MODULENAME.: spi.h
*
* PROJECT....: Pan-and-tilt
*
* DESCRIPTION: SPI communication.
*
* Change Log:
******************************************************************************
* Date    Id    Change
* YYMMDD
* --------------------
* 170309  MBJ    Module created.
*
*****************************************************************************/

#ifndef SSI1_H_
#define SSI1_H_

/***************************** Include files *******************************/

/*****************************    Defines    *******************************/
typedef void (*fptr)(INT8U);

/*****************************   Constants   *******************************/

/*****************************   Functions   *******************************/
void init_ssi1();
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : Initialize SPI on port F.
******************************************************************************/


void ssi1_write(INT8U data, fptr cb);
/*****************************************************************************
*   Input    : INT8U data, fptr cb
*   Output   : -
*   Function : Add data to tx queue and function pointer (callback) to ptr queue.
*              The callback function will be called with the response data
******************************************************************************/
void ssi1_tx_task(INT8U my_id, INT8U my_state, INT8U event, INT8U data);
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : Send data to accelerometer.
******************************************************************************/
void ssi1_rx_task(INT8U my_id, INT8U my_state, INT8U event, INT8U data);
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : Receive data from accelerometer.
******************************************************************************/
/****************************** End Of Module *******************************/


#endif /* SSI1_H_ */
