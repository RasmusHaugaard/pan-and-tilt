#include "emp_type.h"
#include "controller.h"
#include "rtcs.h"
#include "interval.h"
#include "ssi2.h"
#include "pid.h"
#include "pan_tilt_config.h"
#include "encoder.h"
#include "accelerometer.h"
#include "ss.h"
#include "ss_conf.h"

#define FPGA_PWM_pan_reg        0x01
#define FPGA_PWM_tilt_reg       0x02

BOOLEAN accelerometer_enable = FALSE;
BOOLEAN controller_enable = FALSE;
INT16S pan_setpoint = 0;
INT16S tilt_setpoint = 0;
INT8S pan_control_variable = 0;
INT8S tilt_control_variable = 0;

enum Controller_type{
    PID,
    SS,
};
enum Controller_type controller_type = PID;

void enable_controller()
{
    controller_enable = TRUE;
}

void disable_controller()
{
    if (controller_enable){
        controller_enable = FALSE;
        set_pan_control_variable(0);
        set_tilt_control_variable(0);
    }
}

void enable_accelerometer()
{
    accelerometer_enable = TRUE;
}

void disable_accelerometer()
{
    if (accelerometer_enable){
        accelerometer_enable = FALSE;
        set_pan_control_variable(0);
        set_tilt_control_variable(0);
    }
}
void set_pan_setpoint(INT16S setpoint)
{
    pan_setpoint = setpoint;
}

void set_tilt_setpoint(INT16S setpoint)
{
    tilt_setpoint = setpoint;
}

INT16S get_pan_setpoint()
{
    return pan_setpoint;
}

INT16S get_tilt_setpoint()
{
    return tilt_setpoint;
}

void set_pan_control_variable(INT8S val)
{
    pan_control_variable = val;
    ssi2_write(FPGA_PWM_pan_reg, NULL);
    ssi2_write(val, NULL);
}

void set_tilt_control_variable(INT8S val)
{
    tilt_control_variable = val;
    ssi2_write(FPGA_PWM_tilt_reg, NULL);
    ssi2_write(val, NULL);
}

INT8S get_pan_control_variable()
{
    return pan_control_variable;
}

INT8S get_tilt_control_variable()
{
    return tilt_control_variable;
}

void controller_task(INT8U id, INT8U state, INT8U event, INT8U data)
{
    static INT8U interval;
    static INT8U pid_tilt;
    static INT8U pid_pan;
    static ss_ptr ss_tilt;

    switch (state)
    {
    case 0:
        interval = create_interval(millis(10));
        set_pan_control_variable(0);
        set_tilt_control_variable(0);

        pid_tilt = create_pid(40, 0, 0, 0.01);
        pid_pan = create_pid(40, 0, 0, 0.01);

        ss_tilt = ss_tilt_setup();

        set_state(1);
        break;
    case 1:
        if (accelerometer_enable)
        {
            set_pan_setpoint(rad_to_ticks(get_acc_roll()));
            set_tilt_setpoint(rad_to_ticks(get_acc_pitch()));
        }
        if (check_interval(interval) && controller_enable)
        {
            switch  (controller_type){
                case PID:
                    set_pan_control_variable(
                        voltage_to_dutycycle(
                            pid_next(
                                pid_pan,
                                ticks_to_rad(get_pan_process_variable()),
                                ticks_to_rad(pan_setpoint)
                            )
                        )
                    );

                    set_tilt_control_variable(
                        voltage_to_dutycycle(
                            pid_next(
                                pid_tilt,
                                ticks_to_rad(get_tilt_process_variable()),
                                ticks_to_rad(tilt_setpoint)
                            )
                        )
                    );
                    break;

                case SS:{
                    set_matrix(ss_tilt->y, 1, 0, (FP32) 3.1416 / 360 * get_tilt_process_variable());
                    set_matrix(ss_tilt->r, 1, 0, (FP32) 3.1416 / 360 * tilt_setpoint);
                    ss_next(ss_tilt);
                    set_tilt_control_variable(
                            voltage_to_dutycycle(
                                    get_matrix(ss_tilt->u, 0, 0)
                            )
                    );
                    break;
                }
            }
        }
        break;
    }
}
