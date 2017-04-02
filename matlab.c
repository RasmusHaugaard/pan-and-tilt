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

/***************************** Include files *******************************/
#include <stdint.h>
#include <stdlib.h>
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "matlab.h"
#include "glob_def.h"
#include "swtimers.h"
#include "tmodel.h"
/*****************************    Defines    *******************************/
#define HIGH(x)  ((x) >> 8)
#define LOW(x)  ((x) & 0xFF)

#define SET_PWM_PAN     0x10
#define SET_PWM_TILT    0x11
#define ENC_ON          0x01
#define ENC_OFF         0x02
#define ENC_PAN_RESP    0x20
#define ENC_TILT_RESP   0x21
#define PING_REQ        0xF0
#define PING_RESP       0xF1

#define FPGA_PWM_pan_reg        0x01
#define FPGA_PWM_tilt_reg       0x02

enum matlab_states
{
  FIRST_BYTE,
  SPI_SET_PWM_PAN,
  SPI_SET_PWM_TILT,
};

/*****************************   Constants   *******************************/
/*****************************   Variables   *******************************/
INT16U counter_100_msec = TIM_100_MSEC;
INT8U uart_cmd;
INT8U uart_data;
BOOLEAN encoder_on = 0;
volatile INT16S encoder_pan_data = 0;
volatile INT16S encoder_tilt_data = 0;
/*****************************   Functions   *******************************/
void matlab_task(INT8U my_id, INT8U my_state, INT8U event, INT8U data)
{
    switch (my_state)
    {
    case FIRST_BYTE:
        if( get_queue( Q_UART_RX, &uart_data, WAIT_FOREVER ))
        {
            switch (uart_data)
            {
            case SET_PWM_PAN:
                set_state( SPI_SET_PWM_PAN );
                break;
            case SET_PWM_TILT:
                set_state( SPI_SET_PWM_PAN  );
                break;
            case ENC_ON:
                encoder_on = TRUE;
                encoder_pan_data = 0;
                encoder_tilt_data = 0;
                break;
            case ENC_OFF:
                encoder_on = FALSE;
                break;
            case PING_REQ:
                put_queue( Q_UART_TX, PING_RESP, WAIT_FOREVER );
                break;
            default:
                break;
            }
        }
        break;
    case SPI_SET_PWM_PAN:
        if(wait_sem(SEM_SPI_AVAILABLE, WAIT_FOREVER))
        {
            put_queue( Q_SPI_TX, FPGA_PWM_pan_reg, WAIT_FOREVER );
            put_queue( Q_SPI_TX, uart_data, WAIT_FOREVER );
            signal (SEM_SPI_AVAILABLE);
            set_state( FIRST_BYTE );
        }
        break;
    case SPI_SET_PWM_TILT:
        if(wait_sem(SEM_SPI_AVAILABLE, WAIT_FOREVER))
        {
            put_queue( Q_SPI_TX, FPGA_PWM_tilt_reg, WAIT_FOREVER );
            put_queue( Q_SPI_TX, uart_data, WAIT_FOREVER );
            signal (SEM_SPI_AVAILABLE);
            set_state( FIRST_BYTE );
        }
        break;
    default:
        set_state( FIRST_BYTE );
        break;
    }

    if(encoder_on)
    {
        if(!--counter_100_msec)
        {
            counter_100_msec = TIM_100_MSEC;
            put_queue( Q_UART_TX, ENC_PAN_RESP, WAIT_FOREVER );
            put_queue( Q_UART_TX, LOW(encoder_pan_data), WAIT_FOREVER );
            put_queue( Q_UART_TX, HIGH(encoder_pan_data), WAIT_FOREVER );
            put_queue( Q_UART_TX, ENC_TILT_RESP, WAIT_FOREVER );
            put_queue( Q_UART_TX, LOW(encoder_tilt_data), WAIT_FOREVER );
            put_queue( Q_UART_TX, HIGH(encoder_tilt_data), WAIT_FOREVER );
        }
    }
}
