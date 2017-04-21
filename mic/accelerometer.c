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
#include "spi.h"
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

/*****************************   Constants   *******************************/
/*****************************   Variables   *******************************/
INT16S acc_x_data = 0;
INT16S acc_y_data = 0;
INT16S acc_z_data = 0;
volatile BOOLEAN acc_data_ready;
/*****************************   Functions   *******************************/
void init_accelerometer()
{
    // Default output data rate: 100 Hz
    ssi3_write(((DATA_FORMAT)<<8)|0x08, NULL);  // 2g, 13-bit mode
    ssi3_write(((POWER_CTL)<<8)|0x08, NULL);    // Start measurement
    ssi3_write(((INT_ENABLE)<<8)|0x80, NULL);   // Enable DATA_READY interrupt
}
void update_acc_x0(INT16U ssi3_data)
{
    acc_x_data = (ssi3_data<<8);
}
void update_acc_x1(INT16U ssi3_data)
{
    acc_x_data |= ssi3_data;
}
void update_acc_y0(INT16U ssi3_data)
{
    acc_y_data = (ssi3_data<<8);
}
void update_acc_y1(INT16U ssi3_data)
{
    acc_y_data |= ssi3_data;
}
void update_acc_z0(INT16U ssi3_data)
{
    acc_z_data = (ssi3_data<<8);
}
void update_acc_z1(INT16U ssi3_data)
{
    acc_z_data |= ssi3_data;
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
            if (acc_data_ready)
            {
                ssi3_write((1<<15)|((DATAX0)<<8), update_acc_x0);
                ssi3_write((1<<15)|((DATAX1)<<8), update_acc_x1);
                ssi3_write((1<<15)|((DATAY0)<<8), update_acc_y0);
                ssi3_write((1<<15)|((DATAY1)<<8), update_acc_y1);
                ssi3_write((1<<15)|((DATAZ0)<<8), update_acc_z0);
                ssi3_write((1<<15)|((DATAZ1)<<8), update_acc_z1);
                acc_data_ready = FALSE;
                GPIO_PORTD_IM_R|=(1<<7);  // activate PD7 interrupt
            }
            break;
    }
}

void acc_int_handler()
{
    GPIO_PORTD_IM_R&=~(1<<7);  // deactivate PD7 interrupt
    acc_data_ready = TRUE;
}
