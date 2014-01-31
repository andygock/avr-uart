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

The `UART_RXn_BUFFER_SIZE` and `UART_TXn_BUFFER_SIZE` constants define
the size of the circular buffers in bytes. Note that these constants must be a power of 2.
You may need to adapt this constants to your target and your application by adding to your
compiler options:

	-DUART_RXn_BUFFER_SIZE=nn -DUART_TXn_BUFFER_SIZE=nn
 
`RXn` and `TXn` refer to UART number, for UART3 with 128 byte buffers, add:

	-DUART_RX3_BUFFER_SIZE=128 -DUART_TX3_BUFFER_SIZE=128

UART0 is always enabled by default, to enable the other available UARTs, add the following
to your compiler options (or symbol options), for the relevant USART number:

	-DUSART1_ENABLED -DUSART2_ENABLED -DUSART3_ENABLED
 
To enable large buffer support (over 256 bytes, up to 2^16 bytes) use:

	-DUSARTn_LARGE_BUFFER
	
Where n = USART number.

Supports AVR devices with up to 4 hardware USARTs.
