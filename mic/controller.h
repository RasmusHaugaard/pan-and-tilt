#ifndef CONTROLLER_H_
#define CONTROLLER_H_

void controller_task(INT8U, INT8U, INT8U, INT8U);

void set_pan_setpoint(INT16S setpoint);
void set_tilt_setpoint(INT16S setpoint);

INT16S get_pan_setpoint();
INT16S get_tilt_setpoint();

INT8S get_pan_control_variable();
INT8S get_tilt_control_variable();

#endif
