#include <stdint.h>
#include <stdlib.h>
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "glob_def.h"
#include "rtcs.h"
#include "ssi2.h"
#include "file.h"
#include "interval.h"
#include "ui.h"
#include "controller.h"
#include "encoder.h"
#include "homing.h"
#include "pan_tilt_config.h"
#include "accelerometer.h"
/*****************************    Defines    *******************************/
#define HIGH(x)  ((x) >> 8)
#define LOW(x)  ((x) & 0xFF)

enum ui_states
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
BOOLEAN encoder_on = FALSE;
extern INT16S encoder_pan_data;
extern INT16S encoder_tilt_data;
extern INT16S acc_x_data;
extern INT16S acc_y_data;
extern INT16S acc_z_data;
extern INT16S pan_target;
extern INT16S tilt_target;
extern INT8S pan_pwm;
extern INT8S tilt_pwm;

BOOLEAN data_logging_on = 0;
enum ui_states state = FIRST_BYTE;
/*****************************   Functions   *******************************/
void handle_byte(INT8U ch)
{
    switch (state)
    {
        case FIRST_BYTE:
            switch (ch)
            {
                case START_HOMING:
                    home();
                    break;
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
                case ACCELEROMETER_ON:
                    enable_accelerometer();
                    enable_controller();
                    break;
                case DATA_LOG_ON:
                    data_logging_on = TRUE;
                    break;
                case DATA_LOG_OFF:
                    data_logging_on = FALSE;
                    break;
                case PING_REQ:
                    file_write(F_UART, PING_RESP);
                    break;
            }
            break;
        case PWM_PAN:
            disable_controller();
            set_pan_control_variable(ch);
            state = FIRST_BYTE;
            break;
        case PWM_TILT:
            disable_controller();
            set_tilt_control_variable(ch);
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
                disable_accelerometer();
                set_pan_setpoint((ch << 8) | low_byte);
                enable_controller();
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
                disable_accelerometer();
                set_tilt_setpoint((ch << 8) | low_byte);
                enable_controller();
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

void log_pan_setpoint()
{
    file_write(F_UART, PAN_SETPOINT_RESP);
    file_write(F_UART, LOW(get_pan_setpoint()));
    file_write(F_UART, HIGH(get_pan_setpoint()));
}

void log_tilt_setpoint()
{
    file_write(F_UART, TILT_SETPOINT_RESP);
    file_write(F_UART, LOW(get_tilt_setpoint()));
    file_write(F_UART, HIGH(get_tilt_setpoint()));
}

void log_pan_control_variable()
{
    file_write(F_UART, PWM_PAN_RESP);
    file_write(F_UART, get_pan_control_variable());
}

void log_tilt_control_variable()
{
    file_write(F_UART, PWM_TILT_RESP);
    file_write(F_UART, get_tilt_control_variable());
}

void log_pan_process_variable()
{
    file_write(F_UART, ENC_PAN_RESP);
    file_write(F_UART, LOW(get_pan_process_variable()));
    file_write(F_UART, HIGH(get_pan_process_variable()));
}

void log_tilt_process_variable()
{
    file_write(F_UART, ENC_TILT_RESP);
    file_write(F_UART, LOW(get_tilt_process_variable()));
    file_write(F_UART, HIGH(get_tilt_process_variable()));
}

void log_acc_data()
{
    file_write(F_UART, ACC_X_RESP);
    file_write(F_UART, LOW(acc_x_data));
    file_write(F_UART, HIGH(acc_x_data));
    file_write(F_UART, ACC_Y_RESP);
    file_write(F_UART, LOW(acc_y_data));
    file_write(F_UART, HIGH(acc_y_data));
    file_write(F_UART, ACC_Z_RESP);
    file_write(F_UART, LOW(acc_z_data));
    file_write(F_UART, HIGH(acc_z_data));
}

void log_acc_pitch()
{
    file_write(F_UART, ACC_PITCH_RESP);
    file_write(F_UART, LOW((INT16S)get_acc_pitch()));
    file_write(F_UART, HIGH((INT16S)get_acc_pitch()));
}

void log_acc_roll()
{
    file_write(F_UART, ACC_ROLL_RESP);
    file_write(F_UART, LOW((INT16S)get_acc_roll()));
    file_write(F_UART, HIGH((INT16S)get_acc_roll()));
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
             if (check_interval(interval) && data_logging_on)
             {
                 log_pan_setpoint();
                 log_tilt_setpoint();
                 log_pan_control_variable();
                 log_tilt_control_variable();
                 log_pan_process_variable();
                 log_tilt_process_variable();
                 log_acc_data();
                 log_acc_pitch();
                 log_acc_roll();
             }
             break;
    }
}
