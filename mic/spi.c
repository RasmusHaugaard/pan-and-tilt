/*****************************************************************************
* University of Southern Denmark
* Embedded Programming (EMP)
*
* MODULENAME.: spi.c
*
* PROJECT....: Pan and tilt
*
* DESCRIPTION: SPI communication.
*
* Change Log:
*****************************************************************************
* Date    Id    Change
* YYMMDD
* --------------------
*170307  MBJ   Module created.
*
*****************************************************************************/

/***************************** Include files *******************************/
#include <stdint.h>
#include <stdlib.h>
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "rtcs.h"
#include "spi.h"
#include "glob_def.h"
/*****************************    Defines    *******************************/
/*****************************   Constants   *******************************/
/*****************************   Variables   *******************************/
extern QUEUE Q_SPI_TX, Q_SPI_RX;
/*****************************   Functions   *******************************/

//************************************************************************
// Configure the SSI2 to run @ 0.25Mhz, 8 bit data, CPOL=0, CPHA=0
//************************************************************************
void init_ssi2(void){
    SYSCTL_RCGCSSI_R|=(1<<2); //selecting SSI2 module
    SYSCTL_RCGC2_R|=(1<<1);   //providing clock to PORTB
    GPIO_PORTB_AFSEL_R|=(1<<4)|(1<<5)|(1<<6)|(1<<7);//selecting alternative fuctions
    GPIO_PORTB_PCTL_R=0x22220000;//selecting SSI2 as alternative function
    GPIO_PORTB_DEN_R|=(1<<4)|(1<<5)|(1<<6)|(1<<7);//enabling digital mode for PORTB 4,5,6,7
    GPIO_PORTB_PUR_R|=(1<<4)|(1<<5)|(1<<6)|(1<<7);//selecting pull ups for 4,5,6,7
    SSI2_CR1_R=0;          //disabling SSI2 module for settings
    SSI2_CC_R=SSI_CC_CS_PIOSC; //using PIOSC clock (16 MHz)
    SSI2_CPSR_R=64;        //selecting divisor 64 for SSI clock
    SSI2_CR0_R=0x7;        //freescale mode, 8 bit data, CPOL=0, CPHA=0
    SSI2_CR1_R|=(1<<1);    //enabling SSI2
}

fptr buf_spi[2 * (QUEUE_SIZE + 8)];
INT8U buf_spi_head = 0;
INT8U buf_spi_tail = 0;


void put_ptr_queue(fptr cb)
{
    buf_spi[buf_spi_head++] = cb;
    if (buf_spi_head == QUEUE_SIZE)
        buf_spi_head = 0;
}

fptr get_ptr_queue()
{
    fptr cb = buf_spi[buf_spi_tail++];
    if (buf_spi_tail == QUEUE_SIZE)
        buf_spi_tail = 0;
    return cb;
}

void spi_write(INT8U data, fptr cb){
    put_queue( Q_SPI_TX, data, WAIT_FOREVER );
    put_ptr_queue(cb);
}

void spi_tx_task(INT8U my_id, INT8U my_state, INT8U event, INT8U data)
{
    INT8U spi_data;
    while( (SSI2_SR_R & SSI_SR_TNF) && get_queue(Q_SPI_TX, &spi_data, WAIT_FOREVER))
        SSI2_DR_R = spi_data;
}

void spi_rx_task(INT8U my_id, INT8U my_state, INT8U event, INT8U data)
{
    while( SSI2_SR_R & SSI_SR_RNE ){
        fptr cb = get_ptr_queue();
        INT8U data = SSI2_DR_R;
        if (cb)
            cb(data);
    }
}
