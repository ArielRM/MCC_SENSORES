#include <stdint.h>
#include <avr/interrupt.h>
#include "avr_usart.h"

static volatile struct
{
	volatile uint8_t *data;
	volatile uint8_t size_max;
	volatile uint8_t size;
} USART_buffer;

void USART_read(volatile uint8_t *data, uint8_t size)
{
	USART_buffer.data = data;
	USART_buffer.size_max = size;
	USART_buffer.size = 0;
}

uint8_t USART_data_size()
{
	return USART_buffer.size;
}

ISR(USART_RX_vect)
{
	if (USART_buffer.size < USART_buffer.size_max)
		USART_buffer.data[USART_buffer.size++] = USART_0->UDR_;
}

/* Receive one byte: busy waiting */
uint8_t USART_rx()
{

	/* Wait until something arrive  */
	while (!(USART_0->UCSR_A.BITS.RXC))
		;

	return USART_0->UDR_;
}

void USART_RX_init()
{
	USART_0->UCSR_B.BITS.RXEN = 1;
	USART_0->UCSR_B.BITS.RXCIE = 1;
}