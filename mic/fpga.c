/*****************************************************************************
* University of Southern Denmark
* Embedded Programming (EMP)
*
* MODULENAME.: fpga.h
*
* PROJECT....: Pan-and-tilt
*
* DESCRIPTION: FPGA interface.
*
* Change Log:
******************************************************************************
* Date    Id    Change
* YYMMDD
* --------------------
* 170331  MBJ    Module created.
*
*****************************************************************************/

/***************************** Include files *******************************/
#include <stdint.h>
#include <stdlib.h>
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "tmodel.h"
#include "fpga.h"
#include "swtimers.h"
#include "rtcs.h"
/*****************************    Defines    *******************************/

enum encoder_states
{
  ENC_IDLE,
  ENC_DUMMY,
  ENC_PAN,
  ENC_TILT,
};

#define FPGA_encoder_pan_reg    0x03
#define FPGA_encoder_tilt_reg   0x04
#define FPGA_homing_pan_reg     0x05
#define FPGA_homing_tilt_reg    0x06

/*****************************   Constants   *******************************/
/*****************************   Variables   *******************************/
volatile INT16S encoder_pan_data = 0;
volatile INT16S encoder_tilt_data = 0;
INT8U uart_data;
/*****************************   Functions   *******************************/
void encoder_task(INT8U my_id, INT8U my_state, INT8U event, INT8U data)
{
    switch (my_state)
    {
    case ENC_IDLE:
        if(event == EVENT_SIGNAL || wait_sem(SEM_SPI_AVAILABLE, WAIT_FOREVER))
        {
            put_queue( Q_SPI_TX, (1<<7)|FPGA_encoder_pan_reg, WAIT_FOREVER );
            put_queue( Q_SPI_TX, (1<<7)|FPGA_encoder_tilt_reg, WAIT_FOREVER );
            put_queue( Q_SPI_TX, 0, WAIT_FOREVER );
            set_state( ENC_DUMMY );
        }
        break;
    case ENC_DUMMY:
        if( get_queue( Q_SPI_RX, &uart_data, WAIT_FOREVER ))
        {
            set_state( ENC_PAN );
        }
        break;
    case ENC_PAN:
        if( get_queue( Q_SPI_RX, &uart_data, WAIT_FOREVER ))
        {
            encoder_pan_data = (INT8S)uart_data;
            set_state( ENC_TILT );
        }
        break;
    case ENC_TILT:
        if( get_queue( Q_SPI_RX, &uart_data, WAIT_FOREVER ))
        {
            encoder_tilt_data = (INT8S)uart_data;
            signal ( SEM_SPI_AVAILABLE );
            set_state( ENC_IDLE );
        }
        break;
    }
}
