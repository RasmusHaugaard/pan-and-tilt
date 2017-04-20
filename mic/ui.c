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
#define SET_TARGET_PAN  0x12
#define SET_TARGET_TILT 0x13
#define ENC_ON          0x01
#define ENC_OFF         0x02
#define ENC_PAN_RESP    0x20
#define ENC_TILT_RESP   0x21
#define PWM_PAN_RESP    0x22
#define PWM_TILT_RESP   0x23
#define PING_REQ        0xF0
#define PING_RESP       0xF1

#define FPGA_PWM_pan_reg        0x01
#define FPGA_PWM_tilt_reg       0x02
enum matlab_states
{
  FIRST_BYTE,
  PWM_PAN,
  PWM_TILT,
  TARGET_PAN,
  TARGET_TILT,
};
/*****************************   Constants   *******************************/
extern FILE F_UART;
/*****************************   Variables   *******************************/
INT8U ch;
BOOLEAN encoder_on = 0;
enum matlab_states state = FIRST_BYTE;
extern INT16S encoder_pan_data;
extern INT16S encoder_tilt_data;
extern INT16S pan_target;
extern INT16S tilt_target;
extern INT8S pan_pwm;
extern INT8S tilt_pwm;
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
                case SET_TARGET_PAN:
                    state = TARGET_PAN;
                    break;
                case SET_TARGET_TILT:
                    state = TARGET_TILT;
                    break;
                case ENC_ON:
                    encoder_on = TRUE;
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
        case TARGET_PAN:
        {
            static INT8U i = 0;
            static INT8U low_byte;
            if (i == 0){
                low_byte = ch;
                i++;
            }else{
                i = 0;
                pan_target = (ch << 8) | low_byte;
                state = FIRST_BYTE;
            }
            break;
        }
        case TARGET_TILT:
        {
            static INT8U i = 0;
            static INT8U low_byte;
            if (i == 0){
                low_byte = ch;
                i++;
            }else{
                i = 0;
                tilt_target = (ch << 8) | low_byte;
                state = FIRST_BYTE;
            }
            break;
        }
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
             interval = create_interval(millis(10));
             set_state(1);
             break;
         case 1:
             if (check_interval(interval) && encoder_on)
             {
                 file_write(F_UART, ENC_PAN_RESP);
                 file_write(F_UART, LOW(encoder_pan_data));
                 file_write(F_UART, HIGH(encoder_pan_data));
                 file_write(F_UART, ENC_TILT_RESP);
                 file_write(F_UART, LOW(encoder_tilt_data));
                 file_write(F_UART, HIGH(encoder_tilt_data));
                 file_write(F_UART, PWM_PAN_RESP);
                 file_write(F_UART, pan_pwm);
                 file_write(F_UART, PWM_TILT_RESP);
                 file_write(F_UART, tilt_pwm);
             }
             break;
    }
}
