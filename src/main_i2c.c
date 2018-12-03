#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>

#include "avr_twi_master.h"

#define ADDR 0xD0
#define READ ADDR | 1
#define WRITE ADDR

int main()
{

	unsigned char pk[8];
	pk[0] = READ;

	DDRD = 0xFF;
	PORTD = 0x00;

	TWI_Master_Initialise();

	sei();

	TWI_Start_Transceiver_With_Data(pk, 8);

	for (;;)
	{
		if (!TWI_Transceiver_Busy())
		{
			if (TWI_statusReg.lastTransOK)
			{
				TWI_Get_Data_From_Transceiver(pk, 8);
				PORTD = pk[1]; 
			}
		}
	}
}
