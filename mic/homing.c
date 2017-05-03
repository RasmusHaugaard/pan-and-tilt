#include "emp_type.h"
#include "glob_def.h"
#include "homing.h"
#include "controller.h"
#include "encoder.h"
#include "interval.h"
#include "pan_tilt_config.h"
#include "ssi2.h"
#include "rtcs.h"
#include "file.h"

#define HOMING_PAN_PWM 60
#define HOMING_TILT_PWM 60

#define PAN_MIN_SPEED 5

extern FILE F_UART;
BOOLEAN homing_enable = FALSE;

void home()
{
    homing_enable = TRUE;
}

enum states{
    INIT,
    OFF,
    HOMING,
    WAITING_FOR_CONTROLLER,
};

BOOLEAN pan_homed;
BOOLEAN tilt_homed;

INT16S last_pan_process_value;
INT8U pan_pv_update_index;

void handle_pan_index(INT8U hall_data)
{
    if (pan_homed)
        return;

    if (!hall_data)
    {
        reset_pan_process_variable();
        set_pan_control_variable(0);
        pan_homed = TRUE;
    }
    else
    {
        pan_pv_update_index++;
        pan_pv_update_index %= 20;
        if (pan_pv_update_index == 19)
        {
            INT16S pv = get_pan_process_variable();
            INT16S delta = pv - last_pan_process_value;
            last_pan_process_value = pv;

            if (delta < PAN_MIN_SPEED && delta > -PAN_MIN_SPEED)
            {
                set_pan_control_variable(-get_pan_control_variable());
            }
        }
    }
}

void handle_tilt_index(INT8U hall_data)
{
    if (!hall_data && !tilt_homed)
    {
        reset_tilt_process_variable();
        set_tilt_control_variable(0);
        tilt_homed = TRUE;
    }
}

void homing_task(INT8U my_id, INT8U state, INT8U event, INT8U data)
{
    static INT8U interval;
    switch(state)
    {
    case INIT:
        interval = create_interval(millis(10));
        set_state(OFF);
        break;
    case OFF:
        if (homing_enable){
            homing_enable = FALSE;
            disable_controller();
            pan_homed = FALSE;
            last_pan_process_value = get_pan_process_variable();
            pan_pv_update_index = 0;
            tilt_homed = FALSE;
            set_pan_control_variable(HOMING_PAN_PWM);
            set_tilt_control_variable(HOMING_TILT_PWM);
            set_state(HOMING);
            reset_interval(interval);
        }
        break;
    case HOMING:
        if (check_interval(interval))
        {
            if (!pan_homed)
            {
                ssi2_write(FPGA_REG_HOMING_PAN, NULL);
                ssi2_write(DUMMY, handle_pan_index);
            }
            if (!tilt_homed)
            {
                ssi2_write(FPGA_REG_HOMING_TILT, NULL);
                ssi2_write(DUMMY, handle_tilt_index);
            }
            if (tilt_homed && pan_homed)
            {
                set_state(WAITING_FOR_CONTROLLER);
                set_pan_setpoint(0);
                set_tilt_setpoint(0);
                enable_controller();
                wait(millis(1000));
            }
        }
        break;
    case WAITING_FOR_CONTROLLER:
        file_write(F_UART, HOMING_COMPLETE_RESP);
        disable_controller();
        set_state(OFF);
        break;
    }
}
