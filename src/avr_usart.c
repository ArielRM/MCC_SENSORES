#define AVR_USART_SRC

#include <stdio.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include "avr_usart.h"
#include "bits.h"

static int usart_putchar(char c, FILE *fp)
{
	USART_tx(c);

	return 0;
}
/* Stream init for printf  */
static FILE usart_str = FDEV_SETUP_STREAM(usart_putchar, NULL, _FDEV_SETUP_WRITE);

FILE *USART_init(uint16_t bauds)
{

	USART_0->UBRR_H = (uint8_t)(bauds >> 8);
	USART_0->UBRR_L = bauds;

	/* Disable double speed  */
	USART_0->UCSR_A.MASK = 0;
	
	USART_RX_init();
	USART_tx_init();

	/* Asynchronous mode:
	 * - 8 data bits
	 * - 1 stop bit
	 * - no parity 	 */
	USART_0->UCSR_C.MASK = SET(UCSZ01) | SET(UCSZ00);

	return &usart_str;
}
