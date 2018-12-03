/*
 * avr_usart.h
 *
 *  Created on: May 1, 2018
 *      Author: Renan Augusto Starke
 *      Instituto Federal de Santa Catarina
 */

#ifndef _USART_H
#define _USART_H

#include <stdint.h>
#include <avr/io.h>
#include <stdio.h>

#define USART_BUFFER_SIZE 8

#ifndef _IO
#define _IO volatile uint8_t
#endif

#ifndef _IO16
#define _IO16 volatile uint16_t
#endif

/* Baudrates */
#define B9600 (F_CPU / 16 / 9600 - 1)
#define B57600 (F_CPU / 16 / 57600 - 1)
#define B115200 (F_CPU / 16 / 115200 - 1)

/* Registers structure definition */
typedef struct
{
	union {
		_IO MASK;
		struct
		{
			_IO MPCM : 1; /*!< Multi-processor Communication Mode */
			_IO U2X : 1;  /*!< Double the USART Transmission Speed */
			_IO UPE : 1;  /*!< USART Parity Error */
			_IO DOR : 1;  /*!< Data OverRun */
			_IO FE : 1;   /*!< Frame Error */
			_IO UDRE : 1; /*< USART Data Register Empty */
			_IO TXC : 1;  /*< USART Transmit Complete */
			_IO RXC : 1;  /*< USART Receive Complete */
		} BITS;
	} UCSR_A; /*!< USART Control and Status Register A */

	union {
		_IO MASK;
		struct
		{
			_IO TXB8 : 1;  /*!< Transmit Data Bit 8 */
			_IO RXB8 : 1;  /*!< Receive Data Bit 8 */
			_IO UCSZ2 : 1; /*!< Character Size 2*/
			_IO TXEN : 1;  /*!< Transmitter Enable */
			_IO RXEN : 1;  /*!< Receiver Enable */
			_IO UDRIE : 1; /*!< USART Data Register Empty Interrupt Enable */
			_IO TXCIE : 1; /*!< TX Complete Interrupt Enable */
			_IO RXCIE : 1; /*!< RX Complete Interrupt Enable */
		} BITS;
	} UCSR_B; /*!< USART Control and Status Register B */

	union {
		_IO MASK;
		struct
		{
			_IO UCPOL : 1;  /*!< Clock Polarity */
			_IO UCSZ0 : 1;  /*!< Character Size 0 */
			_IO UCSZ1 : 1;  /*!< Character Size 1 */
			_IO USBS : 1;   /*!< Stop Bit Select */
			_IO UPM0 : 1;   /*!< Parity Mode 0 */
			_IO UPM1 : 1;   /*!< Parity Mode 1 */
			_IO UMSEL0 : 1; /*!< USART Mode Select 0 */
			_IO UMSEL1 : 1; /*!< USART Mode Select 1 */
		} BITS;
	} UCSR_C; /*!< USART Control and Status Register C */

	_IO unused;
	_IO UBRR_L; /*!< USART Baud Rate Low Register */
	_IO UBRR_H; /*!< USART Baud Rate High Register */
	_IO UDR_;   /*!< USART I/O Data Register  */
} UART_Type;

#define USART_0 ((UART_Type *)&UCSR0A)

void usart_init(uint16_t baud);

void usart_tx_init(void);
uint8_t usart_send(uint8_t *data, uint8_t size, void (*callback)(void));

void usart_rx_init(void);
uint8_t usart_receive(uint8_t size, void (*onReceived)(uint8_t *data, uint8_t received));
void usart_rx_cancell();

// Error Codes
#define USART_BUFFER_OVF 1
#define USART_BUSY 2

#endif
