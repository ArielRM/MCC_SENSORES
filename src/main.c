#include <avr/interrupt.h>

#include "modbus_rtu.h"

#define SENSOR_DATA_AX 0
#define SENSOR_DATA_AY 1
#define SENSOR_DATA_GX 2
#define SENSOR_DATA_GY 3

int main(void)
{
	// inicialização i2c
	modbus_rtu_init();
	sei();

	uint16_t sensor_data[4] = {0, 0, 0, 0};
	enum
	{
		SEND_A_X = 0x05,
		SEND_A_Y,
		SEND_G_X,
		SEND_G_Y,
		SEND_SIZE,
	} send_state = SEND_A_X;

	DDRB |= (1 << PB5);
	PORTB &= ~(1 << PB5);
	while (1)
	{
		/*
			Parte do sensor
			O resto da implementação espera que esse bloco seja non-blocking 
			e que os valores obtidos sejam salvos em sensor_data[SENSOR_DATA_$Sensor$Eixo];
		*/

		/* TODO: Talvez seja melhor deixar esse trecho mais elegante e portável */
		switch (modbus_rtu_check())
		{
		case MODBUS_RTU_BUSY:
			break;
		case MODBUS_RTU_OK:
			if (modbus_rtu_write(send_state, sensor_data[send_state - SEND_A_X]))
			{
				if (++send_state == SEND_SIZE)
					send_state = SEND_A_X;
			}
			break;
		case MODBUS_RTU_ERR_CRC:
			break;
		case MODBUS_RTU_ERR_REG:
			PORTB |= (1 << PB5);
			while (1)
				;
			break;
		case MODBUS_RTU_ERR:
			break;
		}
	}
	return 0;
}