#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>

#include "avr_usart.h"
#include "avr_twi_master.h"

#define ADDR 0xD0
#define READ ADDR | 1
#define WRITE ADDR

static unsigned char wakeUp[3] = {WRITE, 0x6B, 0x08};
static unsigned char gyroCfg[3] = {WRITE, 0x1B, 0xF8};
static unsigned char acerCfg[3] = {WRITE, 0x1C, 0xF8};

static unsigned char req_acer[2] = {WRITE, 0x3B};
static unsigned char req_gyro[2] = {WRITE, 0x43};

static unsigned char read_acer[7] = {READ};
static unsigned char read_gyro[7] = {READ};

int main()
{
	enum
	{
		ACER_SEND,
		ACER_WAIT,
		GYRO_SEND,
		GYRO_WAIT,
		PRINT
	} sensor = ACER_SEND;

	DDRB |= (1 << PB5);
	PORTB &= ~(1 << PB5);

	USART_Init(B9600);
	FILE *f = get_usart_stream();

	TWI_Master_Initialise();

	sei();

	TWI_Start_Transceiver_With_Data(wakeUp, 3);

	TWI_Start_Transceiver_With_Data(gyroCfg, 3);
	TWI_Start_Transceiver_With_Data(acerCfg, 3);

	int16_t tax = 0, tay = 0, taz = 0, tgx = 0, tgy = 0, tgz = 0;
	for (;;)
	{
		if (!TWI_Transceiver_Busy())
		{
			switch (sensor)
			{
			case ACER_SEND:
				TWI_Start_Transceiver_With_Data(req_acer, 2);
				TWI_Start_Transceiver_With_Data(read_acer, 7);
				sensor = ACER_WAIT;
				break;
			case ACER_WAIT:
				if (TWI_statusReg.lastTransOK)
				{
					TWI_Get_Data_From_Transceiver(read_acer, 7);
					sensor = GYRO_SEND;
				}
				break;
			case GYRO_SEND:
				TWI_Start_Transceiver_With_Data(req_gyro, 2);
				TWI_Start_Transceiver_With_Data(read_gyro, 7);
				sensor = GYRO_WAIT;
				break;
			case GYRO_WAIT:
				if (TWI_statusReg.lastTransOK)
				{
					TWI_Get_Data_From_Transceiver(read_gyro, 7);
					sensor = PRINT;
				}
				break;
			case PRINT:
				tax = read_acer[1] << 8 | read_acer[2];
				tay = read_acer[3] << 8 | read_acer[4];
				taz = read_acer[5] << 8 | read_acer[6];

				tgx = read_gyro[1] << 8 | read_gyro[2];
				tgy = read_gyro[3] << 8 | read_gyro[4];
				tgz = read_gyro[5] << 8 | read_gyro[6];
				fprintf(f, "ax: %d ay: %d az: %d gx: %d gy: %d gz: %d\r\n",
						tax, tay, taz,
						tgx, tgy, tgz);
				sensor = ACER_SEND;
			}
		}
	}
}