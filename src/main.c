#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "modbus_rtu.h"

int main(void)
{
	DDRB |= (1 << PB5);

	modbus_rtu_init();

	sei();

	modbus_rtu_status_t s;
	while (1)
	{

		modbus_rtu_read(0xC3, 1);

		while (!(s = modbus_rtu_check()))
		{
			PORTB ^= (1 << PB5);
			_delay_ms(1000);
		}

		uint16_t data = modbus_rtu_data();

		modbus_rtu_write(0x2C, data);

		while (!(s = modbus_rtu_check()))
		{
			PORTB ^= (1 << PB5);
			_delay_ms(1000);
		}

		_delay_ms(5000);
	}

	return 0;
}