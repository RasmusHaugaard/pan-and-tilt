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
#include "spi.h"
#include "tmodel.h"
/*****************************    Defines    *******************************/
/*****************************   Constants   *******************************/
/*****************************   Variables   *******************************/
INT8U spi_data;
/*****************************   Functions   *******************************/

//************************************************************************
// Configure the SPI2 to run @ 0.25Mhz, 8 bit data, spi mode 0
//************************************************************************

void init_spi(void){
    SYSCTL_RCGCSSI_R|=(1<<2); //selecting SSI2 module
    SYSCTL_RCGC2_R|=(1<<1);   //providing clock to PORTB
    GPIO_PORTB_AFSEL_R|=(1<<4)|(1<<5)|(1<<6)|(1<<7);//selecting alternative fuctions
    GPIO_PORTB_PCTL_R=0x22220000;//selecting SSI as alternative function
    GPIO_PORTB_DEN_R|=(1<<4)|(1<<5)|(1<<6)|(1<<7);//enabling digital mode for PORTB 4,5,6,7
    GPIO_PORTB_PUR_R|=(1<<4)|(1<<5)|(1<<6)|(1<<7);//selecting pull ups for 4,5,6,7
    SSI2_CR1_R=0;          //disabling SSI module for settings
    SSI2_CC_R=0;           //using main system clock
    SSI2_CPSR_R=64;        //selecting divisor 64 for SSI clock
    SSI2_CR0_R=0x7;        //freescale mode, 8 bit data, steady clock low
    SSI2_CR1_R|=(1<<1);    //enabling SSI
}

void spi_tx_task(INT8U my_id, INT8U my_state, INT8U event, INT8U data)
{
    while( SSI2_SR_R&(SSI_SR_TNF) && get_queue( Q_SPI_TX, &spi_data, WAIT_FOREVER ))
    {
        SSI2_DR_R = spi_data;
    }
}
void spi_rx_task(INT8U my_id, INT8U my_state, INT8U event, INT8U data)
{
    while( SSI2_SR_R&(SSI_SR_RNE) )
    {
        spi_data = SSI2_DR_R;
        put_queue( Q_SPI_RX, spi_data, WAIT_FOREVER );
    }
}
