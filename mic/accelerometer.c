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
/*****************************    Defines    *******************************/
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
/*****************************   Constants   *******************************/
FP32 alpha = 0.1;
/*****************************   Variables   *******************************/
INT16S acc_x_data = 0;
INT16S acc_y_data = 0;
INT16S acc_z_data = 0;
INT8U acc_x_temp = 0;
INT8U acc_y_temp = 0;
INT8U acc_z_temp = 0;
FP32 acc_x_filtered = 0;
FP32 acc_y_filtered = 0;
FP32 acc_z_filtered = 0;
FP32 acc_pitch = 0;
FP32 acc_roll = 0;
FP32 acc_x_filtered_prev = 0;
FP32 acc_y_filtered_prev = 0;
FP32 acc_z_filtered_prev = 0;

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
}
void update_acc_y0(INT8U ssi0_data)
{
    acc_y_temp = ssi0_data;
}
void update_acc_y1(INT8U ssi0_data)
{
    acc_y_data = (ssi0_data<<8)|acc_y_temp;
    acc_y_filtered = filter(acc_y_data, &acc_y_filtered_prev);
}
void update_acc_z0(INT8U ssi0_data)
{
    acc_z_temp = ssi0_data;
}
void update_acc_z1(INT8U ssi0_data)
{
    acc_z_data = (ssi0_data<<8)|acc_z_temp;
    acc_z_filtered = filter(acc_z_data, &acc_z_filtered_prev);
    calc_pitch();
    calc_roll();
    acc_data_req = TRUE;
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

void calc_pitch()
{
    acc_pitch = atan2(acc_x_filtered*0.00390625,sqrt(acc_y_filtered*0.00390625*acc_y_filtered*0.00390625+acc_z_filtered*0.00390625*acc_z_filtered*0.00390625));
}

void calc_roll()
{
    acc_roll = atan2(acc_y_filtered*0.00390625,sqrt(acc_x_filtered*0.00390625*acc_x_filtered*0.00390625+acc_z_filtered*0.00390625*acc_z_filtered*0.00390625));
}

FP32 get_pitch()
{
    return acc_pitch;
}
FP32 get_roll()
{
    return acc_roll;
}
