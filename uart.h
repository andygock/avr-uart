#ifndef UART_H
#define UART_H


/************************************************************************
Title:    Interrupt UART library with receive/transmit circular buffers
Author:   Andy Gock
File:     $Id: uart.h,v 1.8.2.1 2007/07/01 11:14:38 peter Exp $
Software: AVR-GCC 4.1, AVR Libc 1.4
Hardware: any AVR with built-in UART, tested on AT90S8515 & ATmega8 at 4 Mhz
License:  GNU General Public License 
Usage:    see Doxygen manual

Based on original library by Peter Fluery, Tim Sharpe, Nicholas Zambetti.

https://github.com/andygock/avr-uart

LICENSE:
	Copyright (C) 2012 Andy Gock

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

LICENSE:
    Copyright (C) 2006 Peter Fleury

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
************************************************************************/

/************************************************************************
uart_available, uart_flush, uart1_available, and uart1_flush functions
were adapted from the Arduino HardwareSerial.h library by Tim Sharpe on 
11 Jan 2009.  The license info for HardwareSerial.h is as follows:

  HardwareSerial.h - Hardware serial library for Wiring
  Copyright (c) 2006 Nicholas Zambetti.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
************************************************************************/

/************************************************************************
Changelog for modifications made by Tim Sharpe, starting with the current
  library version on his Web site as of 05/01/2009. 

Date        Description
=========================================================================
05/12/2009  Added Arduino-style available() and flush() functions for both
			supported UARTs.  Really wanted to keep them out of the library, so
			that it would be as close as possible to Peter Fleury's original
			library, but has scoping issues accessing internal variables from
			another program.  Go C!

************************************************************************/

/** 
 *  @defgroup pfleury_uart UART Library
 *  @code #include <uart.h> @endcode
 * 
 *  @brief Interrupt UART library using the built-in UART with transmit and receive circular buffers. 
 *
 *  This library can be used to transmit and receive data through the built in UART. 
 *
 *  An interrupt is generated when the UART has finished transmitting or
 *  receiving a byte. The interrupt handling routines use circular buffers
 *  for buffering received and transmitted data.
 *
 *  The UART_RXn_BUFFER_SIZE and UART_TXn_BUFFER_SIZE constants define
 *  the size of the circular buffers in bytes. Note that these constants must be a power of 2.
 *  You may need to adapt this constants to your target and your application by adding 
 *  CDEFS += -DUART_RXn_BUFFER_SIZE=nn -DUART_TXn_BUFFER_SIZE=nn to your Makefile.
 *
 *  UART1 enalbed by default, to enable the other UART numbers, add the following (example):
 *  CDEFS += -DUSART1_ENABLED -DUSART2_ENABLED -DUSART3_ENABLED
 *
 *  @note Based on Atmel Application Note AVR306
 *  @author Andy Gock <andy@gock.net>
 *  @note Based on original library by Peter Fleury and Tim Sharpe.
 */
 
/**@{*/


#if (__GNUC__ * 100 + __GNUC_MINOR__) < 304
#error "This library requires AVR-GCC 3.4 or later, update to newer AVR-GCC compiler !"
#endif


/*
** constants and macros
*/

/* Enable USART 1, 2, 3 as required */
//#define USART1_ENABLED
//#define USART2_ENABLED 
//#define USART3_ENABLED

/** @brief  UART Baudrate Expression
 *  @param  xtalCpu  system clock in Mhz, e.g. 4000000L for 4Mhz          
 *  @param  baudRate baudrate in bps, e.g. 1200, 2400, 9600     
 */
#define UART_BAUD_SELECT(baudRate,xtalCpu) ((xtalCpu)/((baudRate)*16l)-1)

/** @brief  UART Baudrate Expression for ATmega double speed mode
 *  @param  xtalCpu  system clock in Mhz, e.g. 4000000L for 4Mhz           
 *  @param  baudRate baudrate in bps, e.g. 1200, 2400, 9600     
 */
#define UART_BAUD_SELECT_DOUBLE_SPEED(baudRate,xtalCpu) (((xtalCpu)/((baudRate)*8l)-1)|0x8000)

#ifndef UART_RX_BUFFER_SIZE
#define UART_RX_BUFFER_SIZE 128 /**< Size of the circular receive buffer, must be power of 2 */
#endif

#ifndef UART_TX_BUFFER_SIZE
#define UART_TX_BUFFER_SIZE 128 /**< Size of the circular transmit buffer, must be power of 2 */
#endif

#undef UART_RX_BUFFER_SIZE
#undef UART_TX_BUFFER_SIZE

#define UART_RX0_BUFFER_SIZE 128
#define UART_RX1_BUFFER_SIZE 1024
#define UART_RX2_BUFFER_SIZE 128
#define UART_RX3_BUFFER_SIZE 128

#define UART_TX0_BUFFER_SIZE 128
#define UART_TX1_BUFFER_SIZE 128
#define UART_TX2_BUFFER_SIZE 128
#define UART_TX3_BUFFER_SIZE 128

/* test if the size of the circular buffers fits into SRAM */
#if ( (UART_RX_BUFFER_SIZE+UART_TX_BUFFER_SIZE) >= (RAMEND-0x60 ) )
#error "size of UART_RX_BUFFER_SIZE + UART_TX_BUFFER_SIZE larger than size of SRAM"
#endif

/* 
** high byte error return code of uart_getc()
*/
#define UART_FRAME_ERROR      0x0800              /**< Framing Error by UART       */
#define UART_OVERRUN_ERROR    0x0400              /**< Overrun condition by UART   */
#define UART_BUFFER_OVERFLOW  0x0200              /**< receive ringbuffer overflow */
#define UART_NO_DATA          0x0100              /**< no receive data available   */


/*
** function prototypes
*/

/**
   @brief   Initialize UART and set baudrate 
   @param   baudrate Specify baudrate using macro UART_BAUD_SELECT()
   @return  none
*/
extern void uart_init(unsigned int baudrate);


/**
 *  @brief   Get received byte from ringbuffer
 *
 * Returns in the lower byte the received character and in the 
 * higher byte the last receive error.
 * UART_NO_DATA is returned when no data is available.
 *
 *  @return  lower byte:  received byte from ringbuffer
 *  @return  higher byte: last receive status
 *           - \b 0 successfully received data from UART
 *           - \b UART_NO_DATA           
 *             <br>no receive data available
 *           - \b UART_BUFFER_OVERFLOW   
 *             <br>Receive ringbuffer overflow.
 *             We are not reading the receive buffer fast enough, 
 *             one or more received character have been dropped 
 *           - \b UART_OVERRUN_ERROR     
 *             <br>Overrun condition by UART.
 *             A character already present in the UART UDR register was 
 *             not read by the interrupt handler before the next character arrived,
 *             one or more received characters have been dropped.
 *           - \b UART_FRAME_ERROR       
 *             <br>Framing Error by UART
 */
extern unsigned int uart_getc(void);


/**
 *  @brief   Put byte to ringbuffer for transmitting via UART
 *  @param   data byte to be transmitted
 *  @return  none
 */
extern void uart_putc(unsigned char data);


/**
 *  @brief   Put string to ringbuffer for transmitting via UART
 *
 *  The string is buffered by the uart library in a circular buffer
 *  and one character at a time is transmitted to the UART using interrupts.
 *  Blocks if it can not write the whole string into the circular buffer.
 * 
 *  @param   s string to be transmitted
 *  @return  none
 */
extern void uart_puts(const char *s );


/**
 * @brief    Put string from program memory to ringbuffer for transmitting via UART.
 *
 * The string is buffered by the uart library in a circular buffer
 * and one character at a time is transmitted to the UART using interrupts.
 * Blocks if it can not write the whole string into the circular buffer.
 *
 * @param    s program memory string to be transmitted
 * @return   none
 * @see      uart_puts_P
 */
extern void uart_puts_p(const char *s );

/**
 * @brief    Macro to automatically put a string constant into program memory
 * \param    __s string in program memory
 */
#define uart_puts_P(__s)       uart_puts_p(PSTR(__s))

/**
 *  @brief   Return number of bytes waiting in the receive buffer
 *  @return  bytes waiting in the receive buffer
 */
extern int uart_available(void);

/**
 *  @brief   Flush bytes waiting in receive buffer
 */
extern void uart_flush(void);


/** @brief  Initialize USART1 (only available on selected ATmegas) @see uart_init */
extern void uart1_init(unsigned int baudrate);
/** @brief  Get received byte of USART1 from ringbuffer. (only available on selected ATmega) @see uart_getc */
extern unsigned int uart1_getc(void);
/** @brief  Put byte to ringbuffer for transmitting via USART1 (only available on selected ATmega) @see uart_putc */
extern void uart1_putc(unsigned char data);
/** @brief  Put string to ringbuffer for transmitting via USART1 (only available on selected ATmega) @see uart_puts */
extern void uart1_puts(const char *s );
/** @brief  Put string from program memory to ringbuffer for transmitting via USART1 (only available on selected ATmega) @see uart_puts_p */
extern void uart1_puts_p(const char *s );
/** @brief  Macro to automatically put a string constant into program memory */
#define uart1_puts_P(__s)       uart1_puts_p(PSTR(__s))
/** @brief   Return number of bytes waiting in the receive buffer */
extern int uart1_available(void);
/** @brief   Flush bytes waiting in receive buffer */
extern void uart1_flush(void);


/** @brief  Initialize USART2 (only available on selected ATmegas) @see uart_init */
extern void uart2_init(unsigned int baudrate);
/** @brief  Get received byte of USART2 from ringbuffer. (only available on selected ATmega) @see uart_getc */
extern unsigned int uart2_getc(void);
/** @brief  Put byte to ringbuffer for transmitting via USART2 (only available on selected ATmega) @see uart_putc */
extern void uart2_putc(unsigned char data);
/** @brief  Put string to ringbuffer for transmitting via USART2 (only available on selected ATmega) @see uart_puts */
extern void uart2_puts(const char *s );
/** @brief  Put string from program memory to ringbuffer for transmitting via USART2 (only available on selected ATmega) @see uart_puts_p */
extern void uart2_puts_p(const char *s );
/** @brief  Macro to automatically put a string constant into program memory */
#define uart2_puts_P(__s)       uart2_puts_p(PSTR(__s))
/** @brief   Return number of bytes waiting in the receive buffer */
extern int uart2_available(void);
/** @brief   Flush bytes waiting in receive buffer */
extern void uart2_flush(void);


/** @brief  Initialize USART3 (only available on selected ATmegas) @see uart_init */
extern void uart3_init(unsigned int baudrate);
/** @brief  Get received byte of USART3 from ringbuffer. (only available on selected ATmega) @see uart_getc */
extern unsigned int uart3_getc(void);
/** @brief  Put byte to ringbuffer for transmitting via USART3 (only available on selected ATmega) @see uart_putc */
extern void uart3_putc(unsigned char data);
/** @brief  Put string to ringbuffer for transmitting via USART3 (only available on selected ATmega) @see uart_puts */
extern void uart3_puts(const char *s );
/** @brief  Put string from program memory to ringbuffer for transmitting via USART3 (only available on selected ATmega) @see uart_puts_p */
extern void uart3_puts_p(const char *s );
/** @brief  Macro to automatically put a string constant into program memory */
#define uart3_puts_P(__s)       uart3_puts_p(PSTR(__s))
/** @brief   Return number of bytes waiting in the receive buffer */
extern int uart3_available(void);
/** @brief   Flush bytes waiting in receive buffer */
extern void uart3_flush(void);

/**@}*/


#endif // UART_H 

