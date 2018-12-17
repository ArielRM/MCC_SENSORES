#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <avr/interrupt.h>

#include "modbus_rtu.h"

static inline void crc_err()
{
	PORTB |= (1 << PB5);
	PORTB &= ~((1 << PB1) | (1 << PB0));
}

static inline void reg_err()
{
	PORTB |= (1 << PB5) | (1 << PB0);
	PORTB &= ~((1 << PB1));
}

static inline void frame_err()
{
	PORTB |= (1 << PB5) | (1 << PB1);
	PORTB &= ~((1 << PB0));
}

static inline void timeout()
{
	PORTB |= (1 << PB5) | (1 << PB1) | ((1 << PB0));
}

int main()
{
	// inicialização do sensor

	modbus_rtu_init();

	DDRB |= (1 << PB5) | (1 << PB0) | (1 << PB1);
	PORTB &= ~((1 << PB5) | (1 << PB0) | (1 << PB1));

	sei();

	uint16_t sensor_data[4] = {3, 4, 5, 6};

	uint8_t i = 0, crc_count = 0, timeout_count = 0;
	while (1)
	{
		// non-blocking code que salva informações do sensor em sensor_data;

		switch (modbus_rtu_check())
		{
		case MODBUS_RTU_RECEIVING:
			break;
		case MODBUS_RTU_READY:
		case MODBUS_RTU_OK:
			modbus_rtu_write(5 + i, sensor_data[i]);
			crc_count = 0;
			timeout_count = 0;
			PORTB &= ~((1 << PB5) | (1 << PB0) | (1 << PB1));
			i++;
			break;

		case MODBUS_RTU_CRC_ERR:
			crc_err();
			while (++crc_count == 5)
				;
			break;

		case MODBUS_RTU_REG_ERR:
			reg_err();
			break;

		case MODBUS_RTU_FRAME_ERR:
			frame_err();
			break;

		case MODBUS_RTU_TIMED_OUT:
			timeout();
			while (++timeout_count == 5)
				;
			break;

		default:
			break;
		}

		i &= 3;
	}
	return 0;
}