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
#include "glob_def.h"
#include "rtcs.h"
#include "spi.h"
#include "file.h"
#include "interval.h"
#include "ui.h"

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
  PWM_PAN,
  PWM_TILT,
};
/*****************************   Constants   *******************************/
extern FILE F_UART;
/*****************************   Variables   *******************************/
INT8U ch;
BOOLEAN encoder_on = 0;
enum matlab_states state = FIRST_BYTE;
extern volatile INT16S encoder_pan_data;
extern volatile INT16S encoder_tilt_data;
/*****************************   Functions   *******************************/
void handle_byte(INT8U ch)
{
    switch (state)
    {
    case FIRST_BYTE:
        switch (ch)
        {
        case SET_PWM_PAN:
            state = PWM_PAN;
            break;
        case SET_PWM_TILT:
            state = PWM_TILT;
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
            file_write(F_UART, PING_RESP);
            break;
        }
        break;
    case PWM_PAN:
        spi_write(FPGA_PWM_pan_reg, NULL);
        spi_write(ch, NULL);
        state = FIRST_BYTE;
        break;
    case PWM_TILT:
        spi_write(FPGA_PWM_tilt_reg, NULL);
        spi_write(ch, NULL);
        state = FIRST_BYTE;
        break;
    }
}

void ui_input_task(INT8U my_id, INT8U my_state, INT8U event, INT8U data)
{
    while(file_read(F_UART,&ch))
    {
        handle_byte(ch);
    }
}

void ui_output_task(INT8U my_id, INT8U my_state, INT8U event, INT8U data)
{
    static INT8U interval;

    switch(my_state)
    {
         case 0:
             interval = create_interval(millis(100));
             set_state(1);
             break;
         case 1:
             if (check_interval(interval)&&encoder_on)
             {
                 file_write(F_UART, ENC_PAN_RESP);
                 file_write(F_UART, LOW(encoder_pan_data));
                 file_write(F_UART, HIGH(encoder_pan_data));
                 file_write(F_UART, ENC_TILT_RESP);
                 file_write(F_UART, LOW(encoder_tilt_data));
                 file_write(F_UART, HIGH(encoder_tilt_data));
             }
             break;
    }
}
