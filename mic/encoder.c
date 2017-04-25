/***************************** Include files *******************************/
#include <stdint.h>
#include <stdlib.h>
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "glob_def.h"
#include "encoder.h"
#include "spi.h"
#include "rtcs.h"
#include "interval.h"
#include "pan_tilt_config.h"

/*****************************    Defines    *******************************/

/*****************************   Variables   *******************************/
INT16S pan_process_variable = 0;
INT16S tilt_process_variable = 0;

/*****************************   Functions   *******************************/
INT16S get_pan_process_variable()
{
    return pan_process_variable;
}

INT16S get_tilt_process_variable()
{
    return tilt_process_variable;
}

void reset_pan_process_variable()
{
    pan_process_variable = 0;
}

void reset_tilt_process_variable()
{
    tilt_process_variable = 0;
}

void update_pan_encoder(INT8U spi_data){
    static INT8S last_data;
    static BOOLEAN initialized = FALSE;
    INT8S new_data = (INT8S)spi_data;
    if (!initialized){
        last_data = new_data;
        initialized = TRUE;
    }
    INT8S delta = new_data - last_data;
    pan_process_variable += delta;
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
    tilt_process_variable += delta;
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
                spi_write(FPGA_encoder_pan_reg, NULL);
                spi_write(FPGA_encoder_tilt_reg, update_pan_encoder);
                spi_write(DUMMY, update_tilt_encoder);
            }
            break;
    }
}
