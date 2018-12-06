#include <stdint.h>
#include <avr/interrupt.h>
#include "avr_usart.h"


static volatile struct
{
	volatile uint8_t data[USART_FIFO_SIZE];
	volatile uint8_t head;
	volatile uint8_t tail;
} USART_fifo;

static uint8_t USART_fifo_size()
{
	return USART_fifo.tail - USART_fifo.head;
}

static uint8_t USART_fifo_full()
{
	return USART_fifo_size() == USART_FIFO_SIZE;
}

static uint8_t USART_fifo_empty()
{
	return USART_fifo.head == USART_fifo.tail;
}

static uint8_t USART_fifo_wrap_around(uint8_t index)
{
	return (USART_FIFO_SIZE - 1) & index;
}

static uint8_t USART_fifo_enqueue(uint8_t data)
{
	if (USART_fifo_full())
		return 0;
	USART_fifo.data[USART_fifo_wrap_around(USART_fifo.tail++)] = data;
	return 1;
}

static uint8_t USART_fifo_dequeue(uint8_t *data)
{
	if (USART_fifo_empty())
		return 0;
	*data = USART_fifo.data[USART_fifo_wrap_around(USART_fifo.head++)];
	return 1;
}

uint8_t USART_read(uint8_t *data, uint8_t size)
{
	uint8_t i;
	for (i = 0; i < size && USART_fifo_dequeue(&data[i]); i++)
		;
	return i;
}

uint8_t USART_data_available()
{
	return USART_fifo_size();
}

ISR(USART_RX_vect)
{
	USART_fifo_enqueue(USART_0->UDR_);
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