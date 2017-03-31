/*
 * main.c
 */

#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "uart.h"
#include "spi.h"
#include "systick.h"

#define HIGH(x)  ((x) >> 8)
#define LOW(x)  ((x) & 0xFF)

#define TIM_100_MSEC   200

#define FIRST_BYTE 0
#define SECOND_BYTE 1

#define SET_PWM_PAN     0x10
#define SET_PWM_TILT    0x11
#define ENC_ON          0x01
#define ENC_OFF         0x02
#define ENC_PAN_RESP    0x20
#define ENC_TILT_RESP   0x21
#define PING_REQ        0xF0
#define PING_RESP       0xF1

#define FPGA_PWM_pan_reg        0x01
#define FPGA_PWM_tilt_reg       0x02
#define FPGA_encoder_pan_reg    0x03
#define FPGA_encoder_tilt_reg   0x04
#define FPGA_homing_pan_reg     0x05
#define FPGA_homing_tilt_reg    0x06

volatile INT16S ticks = 0;
INT8U state = FIRST_BYTE;
INT8U uart_cmd;
INT8U uart0_data;
INT8U timer = TIM_100_MSEC;
BOOLEAN encoder_state = 0;
INT16S encoder_pan_data = 0;
INT16S encoder_tilt_data = 0;


int main(void) {

    uart0_init( 9600, 8, 1, 'n' );
    init_spi();
    init_systick();
	
    while(1)
    {
        if(uart0_rx_rdy())
        {
            uart0_data = uart0_getc();

            if(state == FIRST_BYTE) //Command-byte
            {
                switch (uart0_data)
                {
                case SET_PWM_PAN:
                    uart_cmd = SET_PWM_PAN;
                    state = SECOND_BYTE;
                    break;
                case SET_PWM_TILT:
                    uart_cmd = SET_PWM_TILT;
                    state = SECOND_BYTE;
                    break;
                case ENC_ON:
                    encoder_state = 1;
                    encoder_pan_data = 0;
                    encoder_tilt_data = 0;
                    break;
                case ENC_OFF:
                    encoder_state = 0;
                    break;
                case PING_REQ:
                    while(!uart0_tx_rdy());
                    uart0_putc(PING_RESP);
                    break;
                case PING_RESP:
                    uart_cmd = PING_RESP;
                    break;
                default:
                    break;
                }
            }
            else if(state == SECOND_BYTE)
            {
                switch (uart_cmd)
                {
                case SET_PWM_PAN:
                    spi_write(FPGA_PWM_pan_reg, uart0_data);
                    state = FIRST_BYTE;
                    break;
                case SET_PWM_TILT:
                    spi_write(FPGA_PWM_tilt_reg, uart0_data);
                    state = FIRST_BYTE;
                    break;
                default:
                    state = FIRST_BYTE;
                    break;
                }
            }
        }
        if(ticks)
        {

            ticks--;
            encoder_pan_data += (INT8S)(spi_read(FPGA_encoder_pan_reg));
            encoder_tilt_data += (INT8S)(spi_read(FPGA_encoder_tilt_reg));

            if(encoder_state && !--timer)
            {
                timer = TIM_100_MSEC;

                while(!uart0_tx_rdy());
                uart0_putc(ENC_PAN_RESP);
                while(!uart0_tx_rdy());
                uart0_putc((INT8U)(LOW(encoder_pan_data)));
                while(!uart0_tx_rdy());
                uart0_putc((INT8U)(HIGH(encoder_pan_data)));

                while(!uart0_tx_rdy());
                uart0_putc(ENC_TILT_RESP);
                while(!uart0_tx_rdy());
                uart0_putc((INT8U)(LOW(encoder_tilt_data)));
                while(!uart0_tx_rdy());
                uart0_putc((INT8U)(HIGH(encoder_tilt_data)));
            }

        }
    }

	return 0;
}
