#include "avr_usart.h"
#include <avr/interrupt.h>

static volatile struct
{
	uint8_t data[USART_BUFFER_SIZE];
	uint8_t size;
	uint8_t received;
	void (*callback)(uint8_t *data, uint8_t received);
} usart_rx_info;

static uint8_t usart_rx_size, usart_rx_received = 0, usart_rx_buffer[USART_BUFFER_SIZE];
static void (*usart_rx_onReceived)(uint8_t *data, uint8_t size);

void usart_rx_init(void)
{
	USART_0->UCSR_B.BITS.RXEN = 1; // RX enable
	usart_rx_info.received = 0;
}

uint8_t usart_receive(uint8_t size, void (*onReceived)(uint8_t *data, uint8_t received))
{
	if (USART_0->UCSR_B.BITS.RXCIE)
	{
		return USART_BUSY;
	}
	if (size > USART_BUFFER_SIZE)
	{
		return USART_BUFFER_OVF;
	}
	if (size == 0 && onReceived != NULL)
	{
		onReceived(NULL, 0);
	}
	else if (size != 0)
	{
		usart_rx_info.size = size;
		usart_rx_info.callback = onReceived;
		usart_rx_info.received = 0;
		USART_0->UCSR_B.BITS.RXCIE = 1;
	}
	return 0;
}

void usart_rx_cancell()
{
	USART_0->UCSR_B.BITS.RXCIE = 0;
	usart_rx_info.received = 0;
}

ISR(USART_RX_vect)
{
	usart_rx_info.data[usart_rx_info.received] = USART_0->UDR_;
	if (++usart_rx_info.received == usart_rx_info.size)
	{
		USART_0->UCSR_B.BITS.RXCIE = 0;
		if (usart_rx_info.callback != NULL)
			usart_rx_info.callback(usart_rx_info.data, usart_rx_info.received);
		usart_rx_info.received = 0;
	}
}
