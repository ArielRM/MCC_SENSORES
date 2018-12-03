#include "avr_usart.h"
#include <avr/interrupt.h>

static struct
{
	uint8_t data[USART_BUFFER_SIZE];
	uint8_t size;
	void (*callback)(void);
} usart_tx_info;

void usart_tx_init(void)
{
	USART_0->UCSR_B.BITS.UDRIE = 0;
	USART_0->UCSR_B.BITS.TXEN = 1; // TX enable
}

uint8_t usart_send(uint8_t *data, uint8_t size, void (*callback)(void))
{
	if (USART_0->UCSR_B.BITS.UDRIE)
	{
		return USART_BUSY;
	}
	if (size > USART_BUFFER_SIZE)
	{
		return USART_BUFFER_OVF;
	}
	for (uint8_t i = 0; i < size; i++)
		usart_tx_info.data[i] = data[i];

	usart_tx_info.size = size;
	usart_tx_info.callback = callback;
	USART_0->UCSR_B.BITS.UDRIE = 1;
	return 0;
}

ISR(USART_UDRE_vect)
{
	static uint8_t sent = 0;
	if (sent == usart_tx_info.size)
	{
		USART_0->UCSR_B.BITS.UDRIE = 0;
		sent = 0;
		if (usart_tx_info.callback != NULL)
			usart_tx_info.callback();
	}
	else
	{
		USART_0->UDR_ = ((uint8_t *)usart_tx_info.data)[sent++];
	}
}
