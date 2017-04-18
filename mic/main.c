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
#include "glob_def.h"

#include "clk.h"
#include "encoder.h"
#include "gpio.h"
#include "systick.h"
#include "rtcs.h"
#include "file.h"

#include "uart.h"
#include "spi.h"

#include "matlab.h"
#include "pid.h"

/*****************************    Defines    *******************************/

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/
FILE F_UART, F_SPI;
QUEUE Q_UART_TX, Q_UART_RX, Q_SPI_TX, Q_SPI_RX;
SEM SEM_SPI_AVAILABLE;
/*****************************   Functions   *******************************/

int main(void)
{
  set_80MHz();
  init_gpio();

  init_uart0( 115200, 8, 1, 'n' );
  init_spi();

  init_rtcs();

  //Queue initialization
  Q_UART_TX = create_queue();
  Q_UART_RX = create_queue();
  Q_SPI_TX = create_queue();
  Q_SPI_RX = create_queue();

  //File initialization
  F_UART = create_file( uart_get_q, uart_put_q );
  F_SPI = create_file( spi_get_q, spi_put_q );

  //Semaphore initialization
  SEM_SPI_AVAILABLE = create_sem();
  preset_sem( SEM_SPI_AVAILABLE, 1 );

  //Task initialization
  create_task( uart_tx_task, "UART TX" );
  create_task( uart_rx_task, "UART RX" );
  create_task( spi_tx_task, "SPI TX" );
  create_task( spi_rx_task, "SPI RX" );

  create_task( matlab_task, "MATLAB" );

  create_task( encoder_task, "ENCODER" );
  create_task( matlab_encoder_task, "ENC MATLAB" );
  create_task( pid_task, "PID" );

  schedule();
}

/****************************** End Of Module *******************************/
