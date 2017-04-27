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
#include "ssi1.h"
#include "file.h"
#include "interval.h"
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

#define ACC_X_RESP      0x30
#define ACC_Y_RESP      0x31
#define ACC_Z_RESP      0x32
#define HIGH(x)  ((x) >> 8)
#define LOW(x)  ((x) & 0xFF)
/*****************************   Constants   *******************************/

extern FILE F_UART;
/*****************************   Variables   *******************************/
INT16S acc_x_data = 2;
INT16S acc_y_data = 2;
INT16S acc_z_data = 2;
volatile BOOLEAN acc_data_ready;


/*****************************   Functions   *******************************/
void init_accelerometer()
{
    // Default output data rate: 100 Hz
    ssi1_write(POWER_CTL, NULL);
    ssi1_write(0, NULL);        // Wakeup
    ssi1_write(POWER_CTL, NULL);
    ssi1_write(0x08, NULL);     // Start measurement
    ssi1_write(DATA_FORMAT, NULL);
    ssi1_write(0x0B, NULL);     // ±16g, FULL_RES mode

//    ssi1_write(INT_ENABLE, NULL);
//    ssi1_write(0x80, NULL);     // Enable DATA_READY interrupt
}
void update_acc_x0(INT8U ssi3_data)
{
    acc_x_data = ssi3_data;
}
void update_acc_x1(INT8U ssi3_data)
{
    acc_x_data |= (ssi3_data<<8);
}
void update_acc_y0(INT8U ssi3_data)
{
    acc_y_data = ssi3_data;
}
void update_acc_y1(INT8U ssi3_data)
{
    acc_y_data |= (ssi3_data<<8);
}
void update_acc_z0(INT8U ssi3_data)
{
    acc_z_data = ssi3_data;
}
void update_acc_z1(INT8U ssi3_data)
{
    acc_z_data |= (ssi3_data<<8);
    file_write(F_UART, ACC_X_RESP);
    file_write(F_UART, LOW(acc_x_data));
    file_write(F_UART, HIGH(acc_x_data));
    file_write(F_UART, ACC_Y_RESP);
    file_write(F_UART, LOW(acc_y_data));
    file_write(F_UART, HIGH(acc_y_data));
    file_write(F_UART, ACC_Z_RESP);
    file_write(F_UART, LOW(acc_z_data));
    file_write(F_UART, HIGH(acc_z_data));
}

void accelerometer_task(INT8U my_id, INT8U my_state, INT8U event, INT8U data)
{
    static INT8U interval;
    switch(my_state)
    {
        case 0:
            wait(millis(1000));  // Wait for accelerometer power on
            set_state(1);
            break;
        case 1:
            init_accelerometer();
            interval = create_interval(millis(10));
            set_state(2);
            break;
        case 2:
             if (check_interval(interval))
             {
                 ssi1_write(DATAX0|READ_REG, NULL);
                 ssi1_write(DUMMY, update_acc_x0);
                 ssi1_write(DATAX1|READ_REG, NULL);
                 ssi1_write(DUMMY, update_acc_x1);
                 ssi1_write(DATAY0|READ_REG, NULL);
                 ssi1_write(DUMMY, update_acc_y0);
                 ssi1_write(DATAY1|READ_REG, NULL);
                 ssi1_write(DUMMY, update_acc_y1);
                 ssi1_write(DATAZ0|READ_REG, NULL);
                 ssi1_write(DUMMY, update_acc_z0);
                 ssi1_write(DATAZ1|READ_REG, NULL);
                 ssi1_write(DUMMY, update_acc_z1);
             }
             break;
    }
/*            if (acc_data_ready)
            {

                acc_data_ready = FALSE;
                GPIO_PORTD_IM_R|=(1<<7);  // activate PD7 interrupt
            }
            break;*/
}

void acc_int_handler()
{
    GPIO_PORTD_IM_R&=~(1<<7);  // deactivate PD7 interrupt
    acc_data_ready = TRUE;
}
