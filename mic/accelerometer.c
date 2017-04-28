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
#include "file.h"
#include "interval.h"
#include "math.h"
/*****************************    Defines    *******************************/
#define PI 3.1415926535

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

#define ACC_X_RESP      0x30
#define ACC_Y_RESP      0x31
#define ACC_Z_RESP      0x32
#define ACC_PITCH_RESP  0x33
#define ACC_ROLL_RESP   0x34
#define HIGH(x)  ((x) >> 8)
#define LOW(x)  ((x) & 0xFF)
/*****************************   Constants   *******************************/

extern FILE F_UART;
/*****************************   Variables   *******************************/
INT16S acc_x_data = 0;
INT16S acc_y_data = 0;
INT16S acc_z_data = 0;
INT16S acc_pitch;
INT16S acc_roll;
volatile BOOLEAN acc_data_req = TRUE;
extern BOOLEAN acc_test_on;


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
    acc_x_data = ssi0_data;
}
void update_acc_x1(INT8U ssi0_data)
{
    filter_x((ssi0_data<<8)|acc_x_data);
}
void update_acc_y0(INT8U ssi0_data)
{
    acc_y_data = ssi0_data;
}
void update_acc_y1(INT8U ssi0_data)
{
    filter_y((ssi0_data<<8)|acc_y_data);
}
void update_acc_z0(INT8U ssi0_data)
{
    acc_z_data = ssi0_data;
}
void update_acc_z1(INT8U ssi0_data)
{
    filter_z((ssi0_data<<8)|acc_z_data);
    acc_data_req = TRUE;
    acc_pitch = (atan2(acc_x_data*0.00390625,sqrt(acc_y_data*0.00390625*acc_y_data*0.00390625+acc_z_data*0.00390625*acc_z_data*0.00390625)) * 180.0) / PI;
    acc_roll = (atan2(acc_y_data*0.00390625,(sqrt(acc_x_data*0.00390625*acc_x_data*0.00390625+acc_z_data*0.00390625*acc_z_data*0.00390625))) * 180.0) / PI;

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
    file_write(F_UART, LOW(acc_pitch));
    file_write(F_UART, HIGH(acc_pitch));
    file_write(F_UART, ACC_ROLL_RESP);
    file_write(F_UART, LOW(acc_roll));
    file_write(F_UART, HIGH(acc_roll));
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

       //     static INT8U interval;
       //     interval = create_interval(millis(2000));
            break;
        case 2:
       //     if (check_interval(interval))

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
             //   wait(millis(5));
             //   GPIO_PORTA_IM_R|=(1<<6);  // activate PA6 interrupt
            }
            break;
    }

}

void acc_int_handler()
{
//    GPIO_PORTA_IM_R&=~(1<<6);  // deactivate PA6 interrupt
//    acc_data_ready = TRUE;
}


#define NZEROS 8
#define NPOLES 8
#define GAIN   8.881761746e+07

static FP64 xv[NZEROS+1], yv[NPOLES+1];
static FP64 xv2[NZEROS+1], yv2[NPOLES+1];
static FP64 xv3[NZEROS+1], yv3[NPOLES+1];

void filter_x(INT16S next_input_value)
  {
       xv[0] = xv[1]; xv[1] = xv[2]; xv[2] = xv[3]; xv[3] = xv[4]; xv[4] = xv[5]; xv[5] = xv[6]; xv[6] = xv[7]; xv[7] = xv[8];
        xv[8] = (FP64)next_input_value / GAIN;
        yv[0] = yv[1]; yv[1] = yv[2]; yv[2] = yv[3]; yv[3] = yv[4]; yv[4] = yv[5]; yv[5] = yv[6]; yv[6] = yv[7]; yv[7] = yv[8];
        yv[8] =   (xv[0] + xv[8]) + 8 * (xv[1] + xv[7]) + 28 * (xv[2] + xv[6])
                     + 56 * (xv[3] + xv[5]) + 70 * xv[4]
                     + ( -0.3282569437 * yv[0]) + (  2.9907148016 * yv[1])
                     + (-11.9493557270 * yv[2]) + ( 27.3499099140 * yv[3])
                     + (-39.2266969180 * yv[4]) + ( 36.1062210080 * yv[5])
                     + (-20.8316432430 * yv[6]) + (  6.8891042262 * yv[7]);
        acc_x_data = (INT16S)yv[8];

  }

void filter_y(INT16S next_input_value)
  {
       xv2[0] = xv2[1]; xv2[1] = xv2[2]; xv2[2] = xv2[3]; xv2[3] = xv2[4]; xv2[4] = xv2[5]; xv2[5] = xv2[6]; xv2[6] = xv2[7]; xv2[7] = xv2[8];
        xv2[8] = (FP64)next_input_value / GAIN;
        yv2[0] = yv2[1]; yv2[1] = yv2[2]; yv2[2] = yv2[3]; yv2[3] = yv2[4]; yv2[4] = yv2[5]; yv2[5] = yv2[6]; yv2[6] = yv2[7]; yv2[7] = yv2[8];
        yv2[8] =   (xv2[0] + xv2[8]) + 8 * (xv2[1] + xv2[7]) + 28 * (xv2[2] + xv2[6])
                     + 56 * (xv2[3] + xv2[5]) + 70 * xv2[4]
                     + ( -0.3282569437 * yv2[0]) + (  2.9907148016 * yv2[1])
                     + (-11.9493557270 * yv2[2]) + ( 27.3499099140 * yv2[3])
                     + (-39.2266969180 * yv2[4]) + ( 36.1062210080 * yv2[5])
                     + (-20.8316432430 * yv2[6]) + (  6.8891042262 * yv2[7]);
        acc_y_data = (INT16S)yv2[8];

  }

void filter_z(INT16S next_input_value)
  {
       xv3[0] = xv3[1]; xv3[1] = xv3[2]; xv3[2] = xv3[3]; xv3[3] = xv3[4]; xv3[4] = xv3[5]; xv3[5] = xv3[6]; xv3[6] = xv3[7]; xv3[7] = xv3[8];
        xv3[8] = (FP64)next_input_value / GAIN;
        yv3[0] = yv3[1]; yv3[1] = yv3[2]; yv3[2] = yv3[3]; yv3[3] = yv3[4]; yv3[4] = yv3[5]; yv3[5] = yv3[6]; yv3[6] = yv3[7]; yv3[7] = yv3[8];
        yv3[8] =   (xv3[0] + xv3[8]) + 8 * (xv3[1] + xv3[7]) + 28 * (xv3[2] + xv3[6])
                     + 56 * (xv3[3] + xv3[5]) + 70 * xv3[4]
                     + ( -0.3282569437 * yv3[0]) + (  2.9907148016 * yv3[1])
                     + (-11.9493557270 * yv3[2]) + ( 27.3499099140 * yv3[3])
                     + (-39.2266969180 * yv3[4]) + ( 36.1062210080 * yv3[5])
                     + (-20.8316432430 * yv3[6]) + (  6.8891042262 * yv3[7]);
        acc_z_data = (INT16S)yv3[8];

  }
