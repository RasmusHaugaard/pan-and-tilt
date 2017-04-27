#include "emp_type.h"
#include "controller.h"
#include "rtcs.h"
#include "interval.h"
#include "ssi2.h"
#include "pid.h"

extern INT16S encoder_pan_data;
extern INT16S encoder_tilt_data;

INT16S pan_target = 0;
INT16S tilt_target = 0;
INT8S pan_pwm = 0;
INT8S tilt_pwm = 0;

#define FPGA_PWM_pan_reg        0x01
#define FPGA_PWM_tilt_reg       0x02

void controller_task(INT8U id, INT8U state, INT8U event, INT8U data)
{
    static INT8U interval;
    switch (state)
    {
        case 0:
            interval = create_interval(millis(10));
            set_state(1);
            break;
        case 1:
        {
            if (check_interval(interval))
            {
                tilt_pwm = pid_next_pwm(encoder_tilt_data, tilt_target);
                ssi2_write(FPGA_PWM_tilt_reg, NULL);
                ssi2_write(tilt_pwm, NULL);
            }
            break;
        }
    }
}
