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

void send_byte(INT8U data){
        SSI2_DR_R=data; //putting the byte to send from SSI
    while((SSI2_SR_R&(1<<0))==0); //waiting for transmission to be done
}

INT8U recieve_byte()
{
    while((SSI2_SR_R&(1<<2))==0); //waiting for transmission to be done
    return (SSI2_DR_R);
}

INT8U spi_read(INT8U addr)
{
    INT8U dummy = 0;
    send_byte(addr|(1<<7));
    send_byte(dummy);
    dummy = recieve_byte();
    return (recieve_byte());
}

void spi_write(INT8U addr, INT8U data)
{
    INT8U dummy;
    send_byte(addr & ~(1<<7));
    send_byte(data);
    dummy = recieve_byte();
    dummy = recieve_byte();
}

void spi_task(INT8U my_id, INT8U my_state, INT8U event, INT8U data)
{

    if( SSI2_SR_R&(SSI_SR_RNE) )
    {
      put_queue( Q_SPI_RX, SSI2_DR_R, WAIT_FOREVER );
    }

    if( SSI2_SR_R&(SSI_SR_TNF) )
    {
      if( get_queue( Q_SPI_TX, &spi_data, WAIT_FOREVER ))
      {
          SSI2_SR_R = spi_data;
      }
    }


}
