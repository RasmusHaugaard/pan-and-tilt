#ifndef PAN_TILT_CONFIG_H_
#define PAN_TILT_CONFIG_H_
#include "emp_type.h"

#define DUMMY 0

#define SET_PWM_PAN     0x04
#define SET_PWM_TILT    0x05
#define SET_TARGET_PAN  0x06
#define SET_TARGET_TILT 0x07

#define DATA_LOG_ON     0x01
#define DATA_LOG_OFF    0x02
#define ACCELEROMETER_ON 0x08
#define START_HOMING    0x03

#define ENC_PAN_RESP    0x10
#define ENC_TILT_RESP   0x11
#define PWM_PAN_RESP    0x12
#define PWM_TILT_RESP   0x13
#define PAN_SETPOINT_RESP 0x14
#define TILT_SETPOINT_RESP 0x15
#define HOMING_COMPLETE_RESP 0x16

#define ACC_X_RESP      0x17
#define ACC_Y_RESP      0x18
#define ACC_Z_RESP      0x19
#define ACC_PITCH_RESP  0x1A
#define ACC_ROLL_RESP   0x1B

#define GET_ENCODER     0x40

#define PING_REQ        0xF0
#define PING_RESP       0xF1

#define FPGA_PWM_pan_reg 0x01
#define FPGA_PWM_tilt_reg 0x02

#define READ_REG                (1<<7)
#define FPGA_encoder_pan_reg    (0x03 | READ_REG)
#define FPGA_encoder_tilt_reg   (0x04 | READ_REG)
#define FPGA_REG_HOMING_PAN    (0x05 | READ_REG)
#define FPGA_REG_HOMING_TILT    (0x06 | READ_REG)

#define DUMMY 0

FP32 ticks_to_rad(INT16S);
INT16S rad_to_ticks(FP32 rad);
INT8S voltage_to_dutycycle(FP32);

#endif
