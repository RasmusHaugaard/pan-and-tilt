#include "emp_type.h"
#include "glob_def.h"
#include "homing.h"
#include "controller.h"
#include "encoder.h"
#include "interval.h"
#include "pan_tilt_config.h"
#include "spi.h"
#include "rtcs.h"

#define HOMING_PAN_PWM 80
#define HOMING_TILT_PWM 80

#define PAN_MIN_SPEED 10

BOOLEAN homing_enable = FALSE;

void home()
{
    homing_enable = TRUE;
}

enum states{
    INIT,
    OFF,
    HOMING,
};

BOOLEAN pan_homed;
BOOLEAN tilt_homed;

INT16S last_pan_process_value;

INT8U i = 0;

void handle_pan_index(INT8U hall_data)
{
    if (pan_homed)
        return;

    if (hall_data)
    {
        reset_pan_process_variable();
        set_pan_control_variable(0);
        pan_homed = TRUE;
    }
    else
    {
        i++;
        i %= 10;
        if (i == 9)
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

void handle_tilt_index(INT8U data)
{
    if (data && !tilt_homed)
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
            tilt_homed = FALSE;
            set_pan_control_variable(HOMING_PAN_PWM);
            set_tilt_control_variable(HOMING_TILT_PWM);
            set_state(HOMING);
        }
        break;
    case HOMING:
        if (check_interval(interval))
        {
            if (!pan_homed)
            {
                spi_write(FPGA_REG_HOMING_PAN, NULL);
                spi_write(DUMMY, handle_pan_index);
            }
            if (!tilt_homed)
            {
                spi_write(FPGA_REG_HOMING_TILT, NULL);
                spi_write(DUMMY, handle_tilt_index);
            }
            if (tilt_homed && pan_homed)
            {
                set_state(OFF);
                set_pan_setpoint(0);
                set_tilt_setpoint(0);
                enable_controller();
            }
        }
        break;
    }
}
