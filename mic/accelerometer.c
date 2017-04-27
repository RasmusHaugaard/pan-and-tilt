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
    acc_x_data |= (ssi0_data<<8);
}
void update_acc_y0(INT8U ssi0_data)
{
    acc_y_data = ssi0_data;
}
void update_acc_y1(INT8U ssi0_data)
{
    acc_y_data |= (ssi0_data<<8);
}
void update_acc_z0(INT8U ssi0_data)
{
    acc_z_data = ssi0_data;
}
void update_acc_z1(INT8U ssi0_data)
{
    acc_z_data |= (ssi0_data<<8);
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

            if (acc_data_ready)
            {

                ssi0_write(DATAX0|READ_REG, NULL);
                ssi0_write(DUMMY, update_acc_x0);
                ssi0_write(DATAX1|READ_REG, NULL);
                ssi0_write(DUMMY, update_acc_x1);
                ssi0_write(DATAY0|READ_REG, NULL);
                ssi0_write(DUMMY, update_acc_y0);
                ssi0_write(DATAY1|READ_REG, NULL);
                ssi0_write(DUMMY, update_acc_y1);
                ssi0_write(DATAZ0|READ_REG, NULL);
                ssi0_write(DUMMY, update_acc_z0);
                ssi0_write(DATAZ1|READ_REG, NULL);
                ssi0_write(DUMMY, update_acc_z1);
                acc_data_ready = FALSE;
                GPIO_PORTA_IM_R|=(1<<6);  // activate PA6 interrupt
            }
            break;
    }

}

void acc_int_handler()
{
    GPIO_PORTA_IM_R&=~(1<<6);  // deactivate PA6 interrupt
    acc_data_ready = TRUE;
}
