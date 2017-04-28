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
#include "ssi2.h"
#include "accelerometer.h"
#include "controller.h"
#include "ssi0.h"
#include "ui.h"

/*****************************    Defines    *******************************/

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/
FILE F_UART, F_SPI;
QUEUE Q_UART_TX, Q_UART_RX, Q_SSI2_TX, Q_SSI2_RX, Q_SSI0_TX, Q_SSI0_RX;
SEM SEM_STATE_UPDATED;
/*****************************   Functions   *******************************/

int main(void)
{
  set_80MHz();
  init_gpio();
  uart0_init( 115200, 8, 1, 'n' );
  init_ssi0();
  init_ssi2();
  init_rtcs();

  //Queue initialization
  Q_UART_TX = create_queue();
  Q_UART_RX = create_queue();
  Q_SSI2_TX = create_queue();
  Q_SSI2_RX = create_queue();
  Q_SSI0_TX = create_queue();
  Q_SSI0_RX = create_queue();

  //File initialization
  F_UART = create_file( uart_get_q, uart_put_q );

  //Semaphore initialization
  SEM_STATE_UPDATED = create_sem();

  //Task initialization
  create_task( uart_tx_task, "UART TX" );
  create_task( uart_rx_task, "UART RX" );
  create_task( ssi2_tx_task, "SSI2 TX" );
  create_task( ssi2_rx_task, "SSI2 RX" );
  create_task( ssi0_tx_task, "SSI0 TX" );
  create_task( ssi0_rx_task, "SSI0 RX" );
  create_task( encoder_task, "ENCODER" );
  create_task( ui_input_task, "UI INPUT" );
  create_task( ui_output_task, "UI OUTPUT" );
  create_task( accelerometer_task, "ACCELEROMETER" );
  create_task( controller_task, "CONTROLLER" );

  schedule();
}

/****************************** End Of Module *******************************/
