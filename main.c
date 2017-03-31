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
#include "tmodel.h"
#include "rtcs/rtcs.h"
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

  uart0_init( 9600, 8, 1, 'n' );

  // Enable the GPIO pin for the LED (PF3).  Set the direction as output, and
  // enable the GPIO pin for digital function.
  GPIO_PORTC_DIR_R = 0xF0;
  GPIO_PORTC_DEN_R = 0xF0;

  GPIO_PORTD_DIR_R = 0x4C;
  GPIO_PORTD_DEN_R = 0x4C;

  GPIO_PORTF_DIR_R = 0x0E;
  GPIO_PORTF_DEN_R = 0x1F;

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
  start_task( TASK_SPI, spi_task );
  start_task( TASK_ENCODER, encoder_task );

  schedule();

  return(0);
}

/****************************** End Of Module *******************************/
