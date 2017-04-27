/*****************************************************************************
* University of Southern Denmark
* Embedded Programming (EMP)
*
* MODULENAME.: encoder.h
*
* PROJECT....: Pan-and-tilt
*
* DESCRIPTION: FPGA interface.
*
* Change Log:
******************************************************************************
* Date    Id    Change
* YYMMDD
* --------------------
* 170331  MBJ    Module created.
*
*****************************************************************************/

/***************************** Include files *******************************/
#include <stdint.h>
#include <stdlib.h>
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "glob_def.h"
#include "encoder.h"
#include "ssi2.h"
#include "rtcs.h"
#include "interval.h"

/*****************************    Defines    *******************************/
#define READ_REG                (1<<7)
#define FPGA_encoder_pan_reg    (0x03 | READ_REG)
#define FPGA_encoder_tilt_reg   (0x04 | READ_REG)
#define DUMMY 0
/*****************************   Constants   *******************************/
/*****************************   Variables   *******************************/
INT16S encoder_pan_data = 0;
INT16S encoder_tilt_data = 0;

/*****************************   Functions   *******************************/

void update_pan_encoder(INT8U spi_data){
    static INT8S last_data;
    static BOOLEAN initialized = FALSE;
    INT8S new_data = (INT8S)spi_data;
    if (!initialized){
        last_data = new_data;
        initialized = TRUE;
    }
    INT8S delta = new_data - last_data;
    encoder_pan_data += delta;
    last_data = new_data;
}

void update_tilt_encoder(INT8U spi_data){
    static INT8S last_data;
    static BOOLEAN initialized = FALSE;
    INT8S new_data = (INT8S)spi_data;
    if (!initialized){
        last_data = new_data;
        initialized = TRUE;
    }
    INT8S delta = new_data - last_data;
    encoder_tilt_data += delta;
    last_data = new_data;
}

void encoder_task(INT8U my_id, INT8U my_state, INT8U event, INT8U data)
{
    static INT8U interval;
    switch(my_state)
    {
        case 0:
            interval = create_interval(millis(1));
            set_state(1);
            break;
        case 1:
            if (check_interval(interval))
            {
                ssi2_write(FPGA_encoder_pan_reg, NULL);
                ssi2_write(FPGA_encoder_tilt_reg, update_pan_encoder);
                ssi2_write(DUMMY, update_tilt_encoder);
            }
            break;
    }
}
