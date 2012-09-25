avr-uart
========

An interrupt driven UART Library for 8-bit AVR microcontrollers

Maintained by Andy Gock

https://github.com/andygock/avr-uart

Derived from original library by Peter Fleury

Interrupt UART library using the built-in UART with transmit and receive circular buffers.

An interrupt is generated when the UART has finished transmitting or
receiving a byte. The interrupt handling routines use circular buffers
for buffering received and transmitted data.

The UART_RXn_BUFFER_SIZE and UART_TXn_BUFFER_SIZE constants define
the size of the circular buffers in bytes. Note that these constants must be a power of 2.

You need to define these buffer sizes in uart.h

To enable the individual UARTs, edit definitions of USARTn_ENABLED in uart.h
 
Supports AVR devices with up to 4 hardware UARTs with independant user defined buffer sizes.