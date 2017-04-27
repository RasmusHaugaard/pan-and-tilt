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
#include "ssi1.h"
#include "glob_def.h"
#include "file.h"
/*****************************    Defines    *******************************/
#define HIGH(x)  ((x) >> 8)
#define LOW(x)  ((x) & 0xFF)
/*****************************   Constants   *******************************/
extern FILE F_UART;
/*****************************   Variables   *******************************/
extern QUEUE Q_SSI1_TX, Q_SSI1_RX;
/*****************************   Functions   *******************************/



//************************************************************************
// Configure the SSI3 to run @ 1Mhz, 16 bit data, CPOL=1, CPHA=1
//************************************************************************
void init_ssi1(void){

/*    SYSCTL_RCGCSSI_R|=(1<<2); //selecting SSI2 module
    SYSCTL_RCGC2_R|=(1<<1);   //providing clock to PORTB
    GPIO_PORTB_AFSEL_R|=(1<<4)|(1<<5)|(1<<6)|(1<<7);//selecting alternative fuctions
    GPIO_PORTB_PCTL_R=0x22220000;//selecting SSI2 as alternative function
    GPIO_PORTB_DEN_R|=(1<<4)|(1<<5)|(1<<6)|(1<<7);//enabling digital mode for PORTB 4,5,6,7
    GPIO_PORTB_PUR_R|=(1<<4)|(1<<5)|(1<<6)|(1<<7);//selecting pull ups for 4,5,6,7
    SSI2_CR1_R=0;          //disabling SSI2 module for settings
    SSI2_CC_R=SSI_CC_CS_PIOSC; //using PIOSC clock (16 MHz)
    SSI2_CPSR_R=16;        //selecting divisor 64 for SSI clock
    SSI2_CR0_R=0xCF;        //freescale mode, 8 bit data, CPOL=0, CPHA=0
    SSI2_CR1_R|=(1<<1);    //enabling SSI2*/

    SYSCTL_RCGC2_R|=(1<<0);   //providing clock to PORTA
    SYSCTL_RCGC1_R |= SYSCTL_RCGC1_SSI0; // providing clock to SSI1
    SYSCTL_RCGCSSI_R|=(1<<0); //selecting SSI0 module
    GPIO_PORTB_AFSEL_R|=(1<<2)|(1<<3)|(1<<4)|(1<<5);//selecting alternative fuctions
    GPIO_PORTB_PCTL_R|=0x00222200;//selecting SSI2 as alternative function
    GPIO_PORTB_DEN_R|=(1<<2)|(1<<3)|(1<<4)|(1<<5);//enabling digital mode for PORTA 2,3,4,5
    GPIO_PORTB_PUR_R|=(1<<2)|(1<<3)|(1<<4)|(1<<5);//selecting pull ups for 2,3,4,5
    SSI0_CR1_R=0;          //disabling SSI1 module for settings
    SSI0_CC_R=SSI_CC_CS_PIOSC; //using PIOSC clock (16 MHz)
    SSI0_CPSR_R=16;        //selecting divisor 16 for SSI clock
    SSI0_CR0_R=0xCF;        //freescale mode, 16 bit data, CPOL=1, CPHA=1
    SSI0_CR1_R|=(1<<1);    //enabling SSI0

    /*
    SYSCTL_RCGCGPIO_R|=(1<<5);
    SYSCTL_RCGC2_R|=(1<<5);   //providing clock to PORTF
    SYSCTL_RCGC1_R |= SYSCTL_RCGC1_SSI1; // providing clock to SSI1
    GPIO_PORTF_DEN_R|=(1<<0)|(1<<1)|(1<<2)|(1<<3);//enabling digital mode for PORTF 0,1,2,3
    GPIO_PORTF_AFSEL_R|=(1<<0)|(1<<1)|(1<<2)|(1<<3);//selecting alternative fuctions
    GPIO_PORTF_PCTL_R=0x00002222;//selecting SSI1 as alternative function
    GPIO_PORTF_PUR_R|=(1<<0)|(1<<1)|(1<<2)|(1<<3);//selecting pull ups for 0,1,2,3
    SYSCTL_RCGCSSI_R|=(1<<1); //selecting SSI1 module

    SSI1_CR1_R=0;          //disabling SSI1 module for settings
    SSI1_CPSR_R=16;        //selecting divisor 16 for SSI clock
    SSI1_CC_R=SSI_CC_CS_PIOSC; //using PIOSC clock (16 MHz)
    SSI1_CR0_R=0xCF;        //freescale mode, 16 bit data, CPOL=1, CPHA=1
    SSI1_CR1_R|=(1<<1);    //enabling SSI1
*/
 /*   GPIO_PORTD_IS_R|=(1<<7);  // selecting level interrupt for PD7
    GPIO_PORTD_IEV_R|=(1<<7); // high level interrupt
    GPIO_PORTD_IM_R|=(1<<7);  // activate PD7 interrupt*/
}

fptr buf_ssi1[2 * (QUEUE_SIZE + 8)];
INT8U buf_ssi1_head = 0;
INT8U buf_ssi1_tail = 0;

void put_ptr_queue2(fptr cb)
{
    buf_ssi1[buf_ssi1_head++] = cb;
    if (buf_ssi1_head == QUEUE_SIZE)
        buf_ssi1_head = 0;
}

fptr get_ptr_queue2()
{
    fptr cb = buf_ssi1[buf_ssi1_tail++];
    if (buf_ssi1_tail == QUEUE_SIZE)
        buf_ssi1_tail = 0;
    return cb;
}

void ssi1_write(INT8U data, fptr cb){
    put_queue( Q_SSI1_TX, data, WAIT_FOREVER );
    put_ptr_queue2(cb);
}

void ssi1_tx_task(INT8U my_id, INT8U my_state, INT8U event, INT8U data)
{
    static INT8U ssi1_data_low;
    static INT8U ssi1_data_high;
    static BOOLEAN ssi1_data_low_ready = FALSE;
    static BOOLEAN ssi1_data_high_ready = FALSE;

    while(SSI0_SR_R & SSI_SR_TNF)
    {
        if(!ssi1_data_high_ready)
            ssi1_data_high_ready = get_queue(Q_SSI1_TX, &ssi1_data_high, WAIT_FOREVER);
        if(!ssi1_data_low_ready)
            ssi1_data_low_ready = get_queue(Q_SSI1_TX, &ssi1_data_low, WAIT_FOREVER);
        if(ssi1_data_high_ready && ssi1_data_low_ready)
        {
            SSI0_DR_R = ((ssi1_data_high<<8)|ssi1_data_low);

            ssi1_data_low_ready = FALSE;
            ssi1_data_high_ready = FALSE;
        }
        else
            break;
    }
}

void ssi1_rx_task(INT8U my_id, INT8U my_state, INT8U event, INT8U data)
{
    while( SSI0_SR_R & SSI_SR_RNE )
    {
        INT16U ssi1_data = SSI0_DR_R;

        fptr cb1 = get_ptr_queue2();
        if (cb1)
            cb1((INT8U)(ssi1_data>>8));
        fptr cb2 = get_ptr_queue2();
        if (cb2)
            cb2((INT8U)(ssi1_data));
    }
}
