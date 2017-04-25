/*****************************************************************************
* University of Southern Denmark
* Embedded Programming (EMP)
*
* MODULENAME.: gpio.c
*
* PROJECT....: EMP
*
* DESCRIPTION: See module specification file (.h-file).
*
* Change Log:
*****************************************************************************
* Date    Id    Change
* YYMMDD
* --------------------
* 170313  MBJ   Module created.
*
*****************************************************************************/

/***************************** Include files *******************************/
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "gpio.h"
/*****************************    Defines    *******************************/

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/

/*****************************   Functions   *******************************/

void init_gpio(void)
/*****************************************************************************
*   Input    :
*   Output   :
*   Function : The super loop.
******************************************************************************/
{
    SYSCTL_RCGC2_R = SYSCTL_RCGC2_GPIOA | SYSCTL_RCGC2_GPIOB | SYSCTL_RCGC2_GPIOC | SYSCTL_RCGC2_GPIOD | SYSCTL_RCGC2_GPIOF;
    SYSCTL_RCGC1_R |= 0x00000001;

    GPIO_PORTC_DIR_R = 0xF0;
    GPIO_PORTC_DEN_R = 0xF0;

    GPIO_PORTD_DIR_R = 0x4C;
    GPIO_PORTD_DEN_R = 0x4C;

    //GPIO_PORTF_DIR_R = 0x0E;
   // GPIO_PORTF_DEN_R = 0x1F;
}

/****************************** End Of Module *******************************/
