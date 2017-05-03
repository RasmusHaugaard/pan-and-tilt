/*****************************************************************************
* University of Southern Denmark
* Embedded Programming (EMP)
*
* MODULENAME.: accelerometer.h
*
* PROJECT....: Pan-and-tilt
*
* DESCRIPTION: Accelerometer interface.
*
*
* Change Log:
******************************************************************************
* Date    Id    Change
* YYMMDD
* --------------------
* 170407  MBJ    Module created.
*
*****************************************************************************/

/***************************** Include files *******************************/
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "glob_def.h"
#include "accelerometer.h"
#include "rtcs.h"
#include "ssi0.h"
#include "math.h"
#include "file.h"
/*****************************    Defines    *******************************/
#define PI 3.1415926535

#define ACC_X_RESP      0x30
#define ACC_Y_RESP      0x31
#define ACC_Z_RESP      0x32
#define ACC_PITCH_RESP  0x33
#define ACC_ROLL_RESP   0x34
#define ACC_PITCH_RESP_NF   0x35
#define ACC_ROLL_RESP_NF    0x36
#define ACC_PITCH_RESP_BW   0x37
#define ACC_ROLL_RESP_BW    0x38

#define HIGH(x)  ((x) >> 8)
#define LOW(x)  ((x) & 0xFF)

#define BW_RATE     0x2C
#define POWER_CTL   0x2D
#define INT_ENABLE  0x2E
#define INT_MAP     0x2F
#define DATA_FORMAT 0x31
#define DATAX0      0x32
#define DATAX1      0x33
#define DATAY0      0x34
#define DATAY1      0x35
#define DATAZ0      0x36
#define DATAZ1      0x37
#define READ_REG    (1<<7)
#define DUMMY       0

#define NZEROS 8
#define NPOLES 8
#define GAIN   8.881761746e+07
/*****************************   Constants   *******************************/
extern FILE F_UART;
FP32 alpha = 0.1;
/*****************************   Variables   *******************************/
FP32 acc_x_xv[NZEROS+1], acc_x_yv[NPOLES+1];
FP32 acc_y_xv[NZEROS+1], acc_y_yv[NPOLES+1];
FP32 acc_z_xv[NZEROS+1], acc_z_yv[NPOLES+1];

INT16S acc_x_data = 0;
INT16S acc_y_data = 0;
INT16S acc_z_data = 0;
INT8U acc_x_temp = 0;
INT8U acc_y_temp = 0;
INT8U acc_z_temp = 0;
FP32 acc_x_filtered = 0;
FP32 acc_y_filtered = 0;
FP32 acc_z_filtered = 0;
FP32 acc_x_bw_filtered = 0;
FP32 acc_y_bw_filtered = 0;
FP32 acc_z_bw_filtered = 0;
FP32 acc_pitch = 0;
FP32 acc_roll = 0;
FP32 acc_pitch_no_filter = 0;
FP32 acc_roll_no_filter = 0;
FP32 acc_roll_bw_filter = 0;
FP32 acc_pitch_bw_filter = 0;
FP32 acc_x_filtered_prev = 0;
FP32 acc_y_filtered_prev = 0;
FP32 acc_z_filtered_prev = 0;

extern BOOLEAN acc_test_on;
BOOLEAN acc_data_req = TRUE;
/*****************************   Functions   *******************************/
void init_accelerometer()
{
    // Default output data rate: 100 Hz
    ssi0_write(POWER_CTL, NULL);
    ssi0_write(0, NULL);        // Wakeup
    ssi0_write(DATA_FORMAT, NULL);
    ssi0_write(0x0B, NULL);     // ±16g, FULL_RES mode, interrupt active high
    ssi0_write(INT_ENABLE, NULL);
    ssi0_write(0, NULL);     // Disable interrupts
    ssi0_write(INT_MAP, NULL);
    ssi0_write(0x80, NULL);     // Route DATA_READY interrupt to INT2
    ssi0_write(INT_ENABLE, NULL);
    ssi0_write(0x80, NULL);     // Enable DATA_READY interrupt
    ssi0_write(POWER_CTL, NULL);
    ssi0_write(0x08, NULL);     // Start measurement
}
void update_acc_x0(INT8U ssi0_data)
{
    acc_x_temp = ssi0_data;
}
void update_acc_x1(INT8U ssi0_data)
{
    acc_x_data = (ssi0_data<<8)|acc_x_temp;
    acc_x_filtered = filter(acc_x_data, &acc_x_filtered_prev);
    acc_x_bw_filtered = filter_bw(acc_x_data, acc_x_xv, acc_x_yv);
}
void update_acc_y0(INT8U ssi0_data)
{
    acc_y_temp = ssi0_data;
}
void update_acc_y1(INT8U ssi0_data)
{
    acc_y_data = (ssi0_data<<8)|acc_y_temp;
    acc_y_filtered = filter(acc_y_data, &acc_y_filtered_prev);
    acc_y_bw_filtered = filter_bw(acc_y_data, acc_y_xv, acc_y_yv);
}
void update_acc_z0(INT8U ssi0_data)
{
    acc_z_temp = ssi0_data;
}
void update_acc_z1(INT8U ssi0_data)
{
    acc_z_data = (ssi0_data<<8)|acc_z_temp;
    acc_z_filtered = filter(acc_z_data, &acc_z_filtered_prev);
    acc_z_bw_filtered = filter_bw(acc_z_data, acc_z_xv, acc_z_yv);
    calc_pitch();
    calc_roll();

    acc_pitch_no_filter = get_pitch(acc_x_data,acc_y_data,acc_z_data);
    acc_roll_no_filter = get_roll(acc_x_data,acc_y_data,acc_z_data);
    acc_pitch_bw_filter = get_pitch(acc_x_bw_filtered,acc_y_bw_filtered,acc_z_bw_filtered);
    acc_roll_bw_filter = get_roll(acc_x_bw_filtered,acc_y_bw_filtered,acc_z_bw_filtered);

    acc_data_req = TRUE;

    if(acc_test_on){
    file_write(F_UART, ACC_X_RESP);
    file_write(F_UART, LOW(acc_x_data));
    file_write(F_UART, HIGH(acc_x_data));
    file_write(F_UART, ACC_Y_RESP);
    file_write(F_UART, LOW(acc_y_data));
    file_write(F_UART, HIGH(acc_y_data));
    file_write(F_UART, ACC_Z_RESP);
    file_write(F_UART, LOW(acc_z_data));
    file_write(F_UART, HIGH(acc_z_data));
    file_write(F_UART, ACC_PITCH_RESP);
    file_write(F_UART, LOW((INT16S)(acc_pitch*180/PI)));
    file_write(F_UART, HIGH((INT16S)(acc_pitch*180/PI)));
    file_write(F_UART, ACC_ROLL_RESP);
    file_write(F_UART, LOW((INT16S)(acc_roll*180/PI)));
    file_write(F_UART, HIGH((INT16S)(acc_roll*180/PI)));
    file_write(F_UART, ACC_PITCH_RESP_NF);
    file_write(F_UART, LOW((INT16S)(acc_pitch_no_filter*180/PI)));
    file_write(F_UART, HIGH((INT16S)(acc_pitch_no_filter*180/PI)));
    file_write(F_UART, ACC_ROLL_RESP_NF);
    file_write(F_UART, LOW((INT16S)(acc_roll_no_filter*180/PI)));
    file_write(F_UART, HIGH((INT16S)(acc_roll_no_filter*180/PI)));
    file_write(F_UART, ACC_PITCH_RESP_BW);
    file_write(F_UART, LOW((INT16S)(acc_pitch_bw_filter*180/PI)));
    file_write(F_UART, HIGH((INT16S)(acc_pitch_bw_filter*180/PI)));
    file_write(F_UART, ACC_ROLL_RESP_BW);
    file_write(F_UART, LOW((INT16S)(acc_roll_bw_filter*180/PI)));
    file_write(F_UART, HIGH((INT16S)(acc_roll_bw_filter*180/PI)));
    }
}

void accelerometer_task(INT8U my_id, INT8U my_state, INT8U event, INT8U data)
{
    switch(my_state)
    {
        case 0:
            wait(millis(2));  // Wait for accelerometer power on
            set_state(1);
            break;
        case 1:
            init_accelerometer();
            set_state(2);
            break;
        case 2:
            if (acc_data_req && (GPIO_PORTA_DATA_R & (1<<6)))
            {
                ssi0_write(DATAY0|READ_REG, NULL);
                ssi0_write(DUMMY, update_acc_y0);
                ssi0_write(DATAY1|READ_REG, NULL);
                ssi0_write(DUMMY, update_acc_y1);
                ssi0_write(DATAX0|READ_REG, NULL);
                ssi0_write(DUMMY, update_acc_x0);
                ssi0_write(DATAX1|READ_REG, NULL);
                ssi0_write(DUMMY, update_acc_x1);
                ssi0_write(DATAZ0|READ_REG, NULL);
                ssi0_write(DUMMY, update_acc_z0);
                ssi0_write(DATAZ1|READ_REG, NULL);
                ssi0_write(DUMMY, update_acc_z1);
                acc_data_req = FALSE;
            }
            break;
    }

}

FP32 filter(FP32 acc_data, FP32 *acc_data_prev)
{
    *acc_data_prev = acc_data * alpha + (*acc_data_prev * (1.0-alpha));
    return *acc_data_prev;
}

FP32 filter_bw(FP32 next_input_value, FP32 *xv, FP32 *yv)
{
     xv[0] = xv[1]; xv[1] = xv[2]; xv[2] = xv[3]; xv[3] = xv[4]; xv[4] = xv[5]; xv[5] = xv[6]; xv[6] = xv[7]; xv[7] = xv[8];
      xv[8] = next_input_value / GAIN;
      yv[0] = yv[1]; yv[1] = yv[2]; yv[2] = yv[3]; yv[3] = yv[4]; yv[4] = yv[5]; yv[5] = yv[6]; yv[6] = yv[7]; yv[7] = yv[8];
      yv[8] =   (xv[0] + xv[8]) + 8 * (xv[1] + xv[7]) + 28 * (xv[2] + xv[6])
                   + 56 * (xv[3] + xv[5]) + 70 * xv[4]
                   + ( -0.3282569437 * yv[0]) + (  2.9907148016 * yv[1])
                   + (-11.9493557270 * yv[2]) + ( 27.3499099140 * yv[3])
                   + (-39.2266969180 * yv[4]) + ( 36.1062210080 * yv[5])
                   + (-20.8316432430 * yv[6]) + (  6.8891042262 * yv[7]);
      return yv[8];

}

void calc_pitch()
{
    acc_pitch = atan2(acc_x_filtered*0.00390625,sqrt(acc_y_filtered*0.00390625*acc_y_filtered*0.00390625+acc_z_filtered*0.00390625*acc_z_filtered*0.00390625));
}

void calc_roll()
{
    acc_roll = atan2(acc_y_filtered*0.00390625,sqrt(acc_x_filtered*0.00390625*acc_x_filtered*0.00390625+acc_z_filtered*0.00390625*acc_z_filtered*0.00390625));
}

FP32 get_pitch(INT16S vector_x, INT16S vector_y, INT16S vector_z)
{
    return atan2(vector_x*0.00390625,sqrt(vector_y*0.00390625*vector_y*0.00390625+vector_z*0.00390625*vector_z*0.00390625));
}

FP32 get_roll(INT16S vector_x, INT16S vector_y, INT16S vector_z)
{
    return atan2(vector_y*0.00390625,sqrt(vector_x*0.00390625*vector_x*0.00390625+vector_z*0.00390625*vector_z*0.00390625));
}

FP32 get_acc_pitch()
{
    return acc_pitch;
}
FP32 get_acc_roll()
{
    return acc_roll;
}
