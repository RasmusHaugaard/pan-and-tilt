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
#include "rtcs.h"
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
/*
#define SET_PWM_PAN     'A'
#define SET_PWM_TILT    'B'
#define ENC_ON          'C'
#define ENC_OFF         'D'
#define ENC_PAN_RESP    'E'
#define ENC_TILT_RESP   'F'
#define PING_REQ        'G'
#define PING_RESP       'H'*/

#define FPGA_PWM_pan_reg        0x01
#define FPGA_PWM_tilt_reg       0x02

enum matlab_states
{
  FIRST_BYTE,
  SET_PWM,
  WAIT_FOR_PWM,
  WAIT_FOR_DUMMY,
};

/*****************************   Constants   *******************************/
/*****************************   Variables   *******************************/
INT8U uart_cmd;
INT8U uart_data;
INT8U dummy_cnt = 0;
BOOLEAN encoder_on = 0;
extern volatile INT16S encoder_pan_data;
extern volatile INT16S encoder_tilt_data;
/*****************************   Functions   *******************************/
void matlab_task(INT8U my_id, INT8U my_state, INT8U event, INT8U data)
{

    switch (my_state)
    {
    case FIRST_BYTE:
        if( get_queue( Q_UART_RX, &uart_cmd, WAIT_FOREVER ))
        {
            switch (uart_cmd)
            {
            case SET_PWM_PAN:
                set_state( SET_PWM );
                break;
            case SET_PWM_TILT:
                set_state( SET_PWM  );
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
    case SET_PWM:
        if(event == EVENT_SIGNAL || wait_sem(SEM_SPI_AVAILABLE, WAIT_FOREVER))
        {
            set_state( WAIT_FOR_PWM );
        }
        break;
    case WAIT_FOR_PWM:
        if( get_queue( Q_UART_RX, &uart_data, WAIT_FOREVER ))
        {
            if (uart_cmd == SET_PWM_PAN)
                put_queue( Q_SPI_TX, FPGA_PWM_pan_reg, WAIT_FOREVER );
            else
                put_queue( Q_SPI_TX, FPGA_PWM_tilt_reg, WAIT_FOREVER );
            put_queue( Q_SPI_TX, uart_data, WAIT_FOREVER );
            set_state( WAIT_FOR_DUMMY );
        }
        break;
    case WAIT_FOR_DUMMY:
        while( get_queue( Q_SPI_RX, &uart_data, WAIT_FOREVER ))
        {
            dummy_cnt++;
        }
        if (dummy_cnt >= 2)
        {
            dummy_cnt = 0;
            signal (SEM_SPI_AVAILABLE);
            set_state( FIRST_BYTE );
        }
        break;

    default:
        set_state( FIRST_BYTE );
        break;
    }
}
void matlab_encoder_task(INT8U my_id, INT8U my_state, INT8U event, INT8U data)
{
    if(encoder_on)
    {
        put_queue( Q_UART_TX, ENC_PAN_RESP, WAIT_FOREVER );
        put_queue( Q_UART_TX, LOW(encoder_pan_data), WAIT_FOREVER );
        put_queue( Q_UART_TX, HIGH(encoder_pan_data), WAIT_FOREVER );
        put_queue( Q_UART_TX, ENC_TILT_RESP, WAIT_FOREVER );
        put_queue( Q_UART_TX, LOW(encoder_tilt_data), WAIT_FOREVER );
        put_queue( Q_UART_TX, HIGH(encoder_tilt_data), WAIT_FOREVER );
        wait(TIM_100_MSEC);
    }
}
