#include <stdint.h>
#include "avr_usart.h"

/* Send one byte: busy waiting */
void USART_tx(uint8_t data)
{

	/* Wait until hardware is ready */
	while (!(USART_0->UCSR_A.BITS.UDRE))
		;

	USART_0->UDR_ = data;
}

void USART_tx_init()
{
	USART_0->UCSR_B.BITS.TXEN = 1;
}
