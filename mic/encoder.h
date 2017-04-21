#ifndef ENCODER_H_
#define ENCODER_H_

void encoder_task(INT8U my_id, INT8U my_state, INT8U event, INT8U data);

INT16S get_pan_process_variable();
INT16S get_tilt_process_variable();

#endif
