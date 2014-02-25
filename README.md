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

## Setting up

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

## Documentation

Doxygen based documentation will be coming soon.

## Notes

### Buffer overflow behaviour

When the RX circular buffer is full, and it receives further data from the UART, a buffer overflow condition occurs. Any new data is dropped. The RX buffer must be read before any more incoming data from the UART is placed into the RX buffer.

If the TX buffer is full, and new data is sent to it using one of the `uartN_put*()` functions, this function will loop and wait until the buffer is not full any more. It is important to make sure you have not disabled your UART transmit interrupts (`TXEN*`) elsewhere in your application (e.g with `cli()`) before calling the `uartN_put*()` functions, as the application will lock up. The UART interrupts are automatically enabled when you use the `uartN_init()` functions. This is probably not the idea behaviour, I'll probably fix this some time.

For now, make sure `TXEN*` interrupts are enabled when calling `uartN_put*()` functions. This should not be an issue unless you have code elsewhere purposely turning it off.
