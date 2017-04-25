#ifndef PAN_TILT_CONFIG_H_
#define PAN_TILT_CONFIG_H_
#include "emp_type.h"

#define FPGA_REG_HOMING_PAN 5
#define FPGA_REG_HOMING_TILT 6
#define DUMMY 0

FP32 ticks_to_rad(INT16S);
INT8S voltage_to_dutycycle(FP32);

#endif
