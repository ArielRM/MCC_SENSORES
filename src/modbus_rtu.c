#include <stdint.h>

#include "modbus_rtu.h"
#include "avr_usart.h"

typedef volatile struct
{
	volatile struct
	{
		volatile uint8_t addr;
		volatile uint8_t errc;
	} content;
	volatile uint16_t crc;
} modbus_rtu_frame_err_t;

typedef struct
{
	volatile struct
	{
		volatile uint8_t addr;
		volatile uint8_t cmd;
		volatile uint16_t reg;
		volatile uint16_t data;
	} content;
	volatile uint16_t crc;
} modbus_rtu_frame_t;

static volatile modbus_rtu_frame_t modbus_rtu_frame;
static FILE *modbus_rtu_tx;

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

void modbus_rtu_init(void)
{
	modbus_rtu_tx = USART_init(B9600);
}

uint8_t modbus_rtu_write(uint16_t reg, uint16_t data)
{
	if (USART_data_size() == 0)
	{
		modbus_rtu_frame_t frame;
		frame.content.addr = MODBUS_RTU_ADDR;
		frame.content.cmd = MODBUS_RTU_CMD_W;
		frame.content.reg = reg << 8 | reg >> 8;
		frame.content.data = data << 8 | data >> 8;
		frame.crc = CRC16_2((uint8_t *)&frame.content, sizeof(frame.content));

		fwrite(&frame, sizeof(modbus_rtu_frame_t), 1, modbus_rtu_tx);
		USART_read((uint8_t *)&modbus_rtu_frame, 8);
		return 1;
	}
	return 0;
}

uint8_t modbus_rtu_read(uint16_t reg, uint16_t n /* always 1 */)
{
	if (USART_data_size() == 0)
	{
		modbus_rtu_frame_t frame;
		frame.content.addr = MODBUS_RTU_ADDR;
		frame.content.cmd = MODBUS_RTU_CMD_R;
		frame.content.reg = reg << 8 | reg >> 8;
		frame.content.data = n << 8 | n >> 8;
		frame.crc = CRC16_2((uint8_t *)&frame.content, sizeof(frame.content));

		fwrite(&frame, sizeof(modbus_rtu_frame_t), 1, modbus_rtu_tx);
		USART_read((uint8_t *)&modbus_rtu_frame, 8);
		return 1;
	}
	return 0;
}

modbus_rtu_status_t modbus_rtu_check()
{
	if (USART_data_size() == 8)
	{
		uint16_t crc = CRC16_2((uint8_t *)&modbus_rtu_frame.content, sizeof(modbus_rtu_frame.content));
		USART_read((uint8_t *)&modbus_rtu_frame, 0);
		if (crc == modbus_rtu_frame.crc)
			return MODBUS_RTU_OK;
		else
			return MODBUS_RTU_ERR;
	}
	else if (USART_data_size() == 4)
	{
		// try (buffer2error)
		modbus_rtu_frame_err_t *frame = (modbus_rtu_frame_err_t *)&modbus_rtu_frame;
		uint16_t crc = CRC16_2((uint8_t *)&(frame->content), 2);
		if (crc == frame->crc)
		{
			USART_read((uint8_t *)&modbus_rtu_frame, 0);

			if (frame->content.errc == MODBUS_RTU_CMD_CRC)
				return MODBUS_RTU_ERR_CRC;
			if (frame->content.errc == MODBUS_RTU_CMD_REG)
				return MODBUS_RTU_ERR_REG;
			return MODBUS_RTU_ERR;
		}
	}
	return MODBUS_RTU_BUSY;
}

uint16_t modbus_rtu_data()
{
	return modbus_rtu_frame.content.data >> 8 | modbus_rtu_frame.content.data << 8;
}