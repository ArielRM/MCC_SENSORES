#include "mbrtu.h"
#include "avr_usart.h"

static uint16_t CRC16_2(uint8_t *buf, int len)
{
	uint32_t crc = 0xFFFF;
	int i;
	for (i = 0; i < len; i++)
	{
		crc ^= (uint16_t)buf[i]; // XOR byte into least sig. byte of crc
		for (int i = 8; i != 0; i--)
		{ // Loop over each bit
			if ((crc & 0x0001) != 0)
			{			   // If the LSB is set
				crc >>= 1; // Shift right and XOR 0xA001
				crc ^= 0xA001;
			}
			else		   // Else LSB is not set
				crc >>= 1; // Just shift right
		}
	}
	return crc;
}

static uint8_t mbrtu_frame[8];

static enum {
	MBRTU_READY,
	MBRTU_SENDING,
	MBRTU_RECEIVING,
	MBRTU_DONE,
	MBRTU_CRC_ERROR
} mbrtu_state;

void mbrtu_init()
{
	usart_init(MBRTU_BAUD);

	MBRTU_TIMER_CFG->TCCRA.BITS.WGM1 = 1;
	MBRTU_TIMER_CFG->TCCRB.BITS.CS0 = 1;
	MBRTU_TIMER_CFG->TCCRB.BITS.CS2 = 1;
	MBRTU_TIMER_CFG->OCRA = MBRTU_OCRA;

	mbrtu_state = MBRTU_READY;
}

static void onReceive(uint8_t *data, uint8_t size)
{
	static uint8_t i = 0;
	mbrtu_frame[i++] = data[0];
	if (i < 8)
	{
		usart_receive(1, onReceive);
		uint16_t crc = (mbrtu_frame[7] << 8) | mbrtu_frame[6];
		if (crc == CRC16_2(mbrtu_frame, 6))
			mbrtu_state = MBRTU_DONE;
		else
			mbrtu_state = MBRTU_CRC_ERROR;
	}
}

static void onSent(void)
{
	usart_receive(1, onReceive);
	mbrtu_state = MBRTU_RECEIVING;
}

uint8_t mbrtu_send(uint8_t addr, uint16_t reg, uint16_t data)
{
	if (mbrtu_state == MBRTU_READY)
	{
		mbrtu_frame[0] = addr;
		mbrtu_frame[1] = MBRTU_CMD_WRITE;
		mbrtu_frame[2] = reg >> 8;
		mbrtu_frame[3] = reg;
		mbrtu_frame[4] = data >> 8;
		mbrtu_frame[5] = data;

		uint16_t crc = CRC16_2(mbrtu_frame, 6);
		mbrtu_frame[6] = crc;
		mbrtu_frame[7] = crc >> 8;

		usart_send(&mbrtu_frame, 8, onSent);
		mbrtu_state = MBRTU_SENDING;
		return 0;
	}
	return 1;
}

uint8_t mbrtu_get(uint8_t *frame)
{
	if (mbrtu_state == MBRTU_DONE)
	{
		for (uint8_t i = 0; i < 8; i++)
			frame[i] = mbrtu_frame[i];
		mbrtu_state = MBRTU_READY;
		return 0;
	}
	else if (mbrtu_state == MBRTU_CRC_ERROR)
	{
		mbrtu_state = MBRTU_READY;
		return 2;
	}
	return 1;
}