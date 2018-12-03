#include "avr_usart.h"

void usart_init(uint16_t baud)
{
	USART_0->UCSR_A.BITS.U2X = 0;  // Garante que o mode Double Speed está desativado
	USART_0->UCSR_C.BITS.UPM0 = 0; // Sem paridade
	USART_0->UCSR_C.BITS.UPM1 = 0; // Sem paridade
	USART_0->UCSR_C.BITS.USBS = 0; // 1 stop bit
	USART_0->UBRR_H = ((uint16_t)baud >> 8);
	USART_0->UBRR_L = baud;

	usart_rx_init();
	usart_tx_init();
}
