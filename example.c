/*************************************************************************

Example program, based on Peter Fluery's example.

*************************************************************************/

/*************************************************************************

Title:    Example program for the Interrupt controlled UART library
Author:   Peter Fleury <pfleury@gmx.ch>   http://tinyurl.com/peterfleury
File:     $Id: test_uart.c,v 1.7 2015/01/31 17:46:31 peter Exp $
Software: AVR-GCC 4.x
Hardware: AVR with built-in UART/USART

DESCRIPTION:
          This example shows how to use the UART library uart.c

*************************************************************************/
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "uart.h"

/* Define CPU frequency in Hz in Makefile or toolchain compiler configuration */
#ifndef F_CPU
#error "F_CPU undefined, please define CPU frequency in Hz in Makefile or compiler configuration"
#endif

/* Define UART baud rate here */
#define UART_BAUD_RATE 9600

int main(void)
{
    uint16_t c;
    char buffer[7];
    int8_t num = 134;

    /*
     *  Initialize UART library, pass baudrate and AVR cpu clock
     *  with the macro
     *  UART_BAUD_SELECT() (normal speed mode)
     *  or
     *  UART_BAUD_SELECT_DOUBLE_SPEED() (double speed mode)
     */
    uart_init(UART_BAUD_SELECT(UART_BAUD_RATE, F_CPU));

    /*
     * Now enable interrupt, since UART library is interrupt controlled
     */
    sei();

    /*
     *  Transmit string to UART
     *  The string is buffered by the uart library in a circular buffer
     *  and one character at a time is transmitted to the UART using interrupts.
     *  uart_puts() blocks if it can not write the whole string to the circular
     *  buffer
     */
    uart_puts("String stored in SRAM\n");

    /*
     * Transmit string from program memory to UART
     */
    uart_puts_P("String stored in FLASH\n");

    /*
     * Use standard avr-libc functions to convert numbers into string
     * before transmitting via UART
     */
    itoa(num, buffer, 10); // convert integer into string (decimal format)
    uart_puts(buffer);     // and transmit string to UART

    /*
     * Transmit single character to UART
     */
    uart_putc('\r');

    for (;;)
    {
        /*
         * Get received character from ringbuffer
         * uart_getc() returns in the lower byte the received character and
         * in the higher byte (bitmask) the last receive error
         * UART_NO_DATA is returned when no data is available.
         *
         */
        c = uart_getc();
        if (c & UART_NO_DATA)
        {
            /*
             * No data available from UART
             */
        }
        else
        {
            /*
             * New data available from UART
             * Check for Frame or Overrun error
             */
            if (c & UART_FRAME_ERROR)
            {
                /* Framing Error detected, i.e no stop bit detected */
                uart_puts_P("UART Frame Error: ");
            }
            if (c & UART_OVERRUN_ERROR)
            {
                /*
                 * Overrun, a character already present in the UART UDR register was
                 * not read by the interrupt handler before the next character arrived,
                 * one or more received characters have been dropped
                 */
                uart_puts_P("UART Overrun Error: ");
            }
            if (c & UART_BUFFER_OVERFLOW)
            {
                /*
                 * We are not reading the receive buffer fast enough,
                 * one or more received character have been dropped
                 */
                uart_puts_P("Buffer overflow error: ");
            }
            /*
             * Send received character back
             */
            uart_putc((unsigned char)c);
        }
    }
}
