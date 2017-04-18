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

#ifndef SPI_H_
#define SPI_H_

/***************************** Include files *******************************/

/*****************************    Defines    *******************************/
typedef void (*fptr)(INT8U);

/*****************************   Constants   *******************************/

/*****************************   Functions   *******************************/
void init_spi();
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : Initialize SPI on port B.
******************************************************************************/

void spi_write(INT8U data, fptr cb);
/*****************************************************************************
*   Input    : INT8U data, fptr cb
*   Output   : -
*   Function : Add data to tx queue and function pointer (callback) to ptr queue.
*              The callback function will be called with the response data
******************************************************************************/
void spi_tx_task(INT8U my_id, INT8U my_state, INT8U event, INT8U data);
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : Send data to FPGA.
******************************************************************************/
void spi_rx_task(INT8U my_id, INT8U my_state, INT8U event, INT8U data);
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : Receive data from FPGA.
******************************************************************************/
/****************************** End Of Module *******************************/


#endif /* SPI_H_ */
