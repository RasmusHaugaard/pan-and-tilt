/*****************************************************************************
* University of Southern Denmark
* Embedded Programming (EMP)
*
* MODULENAME.: main.c
*
* PROJECT....: Pan and tilt
*
* DESCRIPTION: Pan and tilt, test.
*
* Change Log:
*****************************************************************************
* Date    Id    Change
* YYMMDD
* --------------------
* 170327  MBJ   Module created.
*
*****************************************************************************/

/***************************** Include files *******************************/
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "gpio.h"
#include "tmodel.h"
#include "rtcs.h"
#include "uart.h"
#include "spi.h"
#include "fpga.h"
#include "matlab.h"

/*****************************    Defines    *******************************/

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/

/*****************************   Functions   *******************************/

int main(void)
{
  init_gpio();
  init_uart0( 115200, 8, 1, 'n' );
  init_spi();
  init_rtcs();

  open_queue( Q_UART_TX );
  open_queue( Q_UART_RX );
  open_queue( Q_SPI_TX );
  open_queue( Q_SPI_RX );

  preset_sem( SEM_SPI_AVAILABLE, 1 );

  start_task( TASK_UART_TX, uart_tx_task );
  start_task( TASK_UART_RX, uart_rx_task );
//start_task( TASK_UI, ui_task );
  start_task( TASK_MATLAB, matlab_task);
  start_task( TASK_SPI_TX, spi_tx_task );
  start_task( TASK_SPI_RX, spi_rx_task );
  start_task( TASK_ENCODER, encoder_task );
  start_task( TASK_MATLAB_ENCODER, matlab_encoder_task );

  schedule();

  return(0);
}

/****************************** End Of Module *******************************/
