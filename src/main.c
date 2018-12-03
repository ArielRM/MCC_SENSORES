#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "mbrtu.h"

int main(void)
{
	mbrtu_init();
	sei();

	DDRB |= (1 << PB5);

	mbrtu_send(0x15, 0x01, 0xAA);
	while (1)
	{
		uint8_t frame[8];
		if (!mbrtu_get(frame))
		{
			if (frame[3] == 1 )
				PORTB |= (1 << PB5);
			else
				PORTB &= ~(1 << PB5);
		}
	}

	return 0;
}