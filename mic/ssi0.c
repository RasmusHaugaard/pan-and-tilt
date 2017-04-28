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
#include "ssi0.h"
#include "glob_def.h"
#include "file.h"
/*****************************    Defines    *******************************/
#define HIGH(x)  ((x) >> 8)
#define LOW(x)  ((x) & 0xFF)
/*****************************   Constants   *******************************/
extern FILE F_UART;
/*****************************   Variables   *******************************/
extern QUEUE Q_SSI0_TX, Q_SSI0_RX;
/*****************************   Functions   *******************************/



//************************************************************************
// Configure the SSI0 to run @ 1Mhz, 16 bit data, CPOL=1, CPHA=1
//************************************************************************
void init_ssi0(void){

    SYSCTL_RCGC2_R|=(1<<0);   //providing clock to PORTA
    SYSCTL_RCGC1_R |= SYSCTL_RCGC1_SSI0; // providing clock to SSI0
    SYSCTL_RCGCSSI_R|=(1<<0); //selecting SSI0 module
    GPIO_PORTA_AFSEL_R|=(1<<2)|(1<<3)|(1<<4)|(1<<5);//selecting alternative fuctions
    GPIO_PORTA_PCTL_R|=0x00222200;//selecting SSI2 as alternative function
    GPIO_PORTA_DIR_R&=~(1<<6);
    GPIO_PORTA_DEN_R|=(1<<2)|(1<<3)|(1<<4)|(1<<5)|(1<<6);//enabling digital mode for PORTA 2,3,4,5
    GPIO_PORTA_PUR_R|=(1<<2)|(1<<3)|(1<<4)|(1<<5)|(1<<6);//selecting pull ups for 2,3,4,5

 /*   GPIO_PORTA_IM_R&=~(1<<6);  // deactivate PA6 interrupt
    GPIO_PORTA_IS_R|=(1<<6);  // selecting level interrupt for PA6
    GPIO_PORTA_IEV_R|=(1<<6); // low level interrupt
    GPIO_PORTA_IBE_R &= ~(1<<6);
    GPIO_PORTA_ICR_R|=(1<<6);
    GPIO_PORTA_IM_R|=(1<<6);  // activate PA6 interrupt
    NVIC_EN0_R|=(1<<INT_GPIOA);    // activate GPIOA interrupt*/

    SSI0_CR1_R=0;          //disabling SSI1 module for settings
    SSI0_CC_R=SSI_CC_CS_PIOSC; //using PIOSC clock (16 MHz)
    SSI0_CPSR_R=16;        //selecting divisor 16 for SSI clock
    SSI0_CR0_R=0xCF;        //freescale mode, 16 bit data, CPOL=1, CPHA=1
    SSI0_CR1_R|=(1<<1);    //enabling SSI0



}

fptr buf_ssi0[2 * (QUEUE_SIZE + 8)];
INT8U buf_ssi0_head = 0;
INT8U buf_ssi0_tail = 0;

void put_ptr_queue2(fptr cb)
{
    buf_ssi0[buf_ssi0_head++] = cb;
    if (buf_ssi0_head == QUEUE_SIZE)
        buf_ssi0_head = 0;
}

fptr get_ptr_queue2()
{
    fptr cb = buf_ssi0[buf_ssi0_tail++];
    if (buf_ssi0_tail == QUEUE_SIZE)
        buf_ssi0_tail = 0;
    return cb;
}

void ssi0_write(INT8U data, fptr cb){
    put_queue( Q_SSI0_TX, data, WAIT_FOREVER );
    put_ptr_queue2(cb);
}

void ssi0_tx_task(INT8U my_id, INT8U my_state, INT8U event, INT8U data)
{
    static INT8U ssi0_data_low;
    static INT8U ssi0_data_high;
    static BOOLEAN ssi0_data_low_ready = FALSE;
    static BOOLEAN ssi0_data_high_ready = FALSE;

    while(SSI0_SR_R & SSI_SR_TNF)
    {
        if(!ssi0_data_high_ready)
            ssi0_data_high_ready = get_queue(Q_SSI0_TX, &ssi0_data_high, WAIT_FOREVER);
        if(!ssi0_data_low_ready)
            ssi0_data_low_ready = get_queue(Q_SSI0_TX, &ssi0_data_low, WAIT_FOREVER);
        if(ssi0_data_high_ready && ssi0_data_low_ready)
        {
            SSI0_DR_R = ((ssi0_data_high<<8)|ssi0_data_low);

            ssi0_data_low_ready = FALSE;
            ssi0_data_high_ready = FALSE;
        }
        else
            break;
    }
}

void ssi0_rx_task(INT8U my_id, INT8U my_state, INT8U event, INT8U data)
{
    while( SSI0_SR_R & SSI_SR_RNE )
    {
        INT16U ssi0_data = SSI0_DR_R;

        fptr cb1 = get_ptr_queue2();
        if (cb1)
            cb1((INT8S)(ssi0_data>>8));
        fptr cb2 = get_ptr_queue2();
        if (cb2)
            cb2((INT8U)(ssi0_data));
    }
}
