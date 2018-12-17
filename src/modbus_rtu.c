#include <stdint.h>
#include <string.h>
#include <avr/interrupt.h>

#include "modbus_rtu.h"
#include "avr_usart.h"
#include "avr_timer.h"

typedef struct
{
	const uint8_t addr;
	const uint8_t err_code;
	uint16_t crc;
} modbus_rtu_frame_err_t;

typedef struct
{
	const uint8_t addr;
	uint8_t cmd;
	uint16_t reg;
	uint16_t data;
	uint16_t crc;
} modbus_rtu_frame_t;

static modbus_rtu_frame_err_t modbus_crc_err_frame = {MODBUS_RTU_ADDR, MODBUS_RTU_CMD_CRC};
static modbus_rtu_frame_err_t modbus_reg_err_frame = {MODBUS_RTU_ADDR, MODBUS_RTU_CMD_REG};

static volatile modbus_rtu_frame_t modbus_rtu_frame = {MODBUS_RTU_ADDR};
static modbus_rtu_status_t modbus_rtu_status;
static volatile uint8_t modbus_rtu_timed;

static void start_timer()
{
	TIMER0_CFG->TCNT = 0;
	TIMER0_CFG->TCCRB.BITS.CS0 = 1;
	TIMER0_CFG->TCCRB.BITS.CS2 = 1;
	TIMER0_IRQ->BITS.TOIE = 1;
}

static void stop_timer()
{
	TIMER0_CFG->TCCRB.BITS.CS0 = 0;
	TIMER0_CFG->TCCRB.BITS.CS2 = 0;
	TIMER0_IRQ->BITS.TOIE = 0;
	TIMER0_CFG->TCNT = 0;
}

static uint16_t swap_endianness(uint16_t n)
{
	return (n << 8) | (n >> 8);
}

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

static void modbus_rtu_transceive()
{
	modbus_rtu_frame.crc = swap_endianness(CRC16_2((uint8_t *)&modbus_rtu_frame, 6));

	for (uint8_t i = 0; i < 8; i++)
		USART_tx(((uint8_t *)&modbus_rtu_frame)[i]);

	USART_read((uint8_t *)&modbus_rtu_frame, 8);
	start_timer();
}

void modbus_rtu_init(void)
{
	USART_init(B9600);
	modbus_rtu_status = MODBUS_RTU_READY;

	modbus_crc_err_frame.crc = swap_endianness(CRC16_2((uint8_t *)&modbus_crc_err_frame, 2));
	modbus_reg_err_frame.crc = swap_endianness(CRC16_2((uint8_t *)&modbus_reg_err_frame, 2));

	modbus_rtu_timed = 0;
	TIMER0_CFG->TCCRA.BITS.WGM1 = 1;
	TIMER0_CFG->OCRA = 0xFF;
}

uint8_t modbus_rtu_write(uint16_t reg, uint16_t data)
{
	if (modbus_rtu_check())
	{
		modbus_rtu_frame.cmd = MODBUS_RTU_CMD_W;
		modbus_rtu_frame.data = swap_endianness(data);
		modbus_rtu_frame.reg = swap_endianness(reg);

		modbus_rtu_transceive();

		modbus_rtu_status = MODBUS_RTU_RECEIVING;
		return 1;
	}
	return 0;
}

uint8_t modbus_rtu_read(uint16_t reg, uint16_t n /* always 1 */)
{
	n = 1;
	if (modbus_rtu_check())
	{
		modbus_rtu_frame.cmd = MODBUS_RTU_CMD_R;
		modbus_rtu_frame.data = swap_endianness(n);
		modbus_rtu_frame.reg = swap_endianness(reg);

		modbus_rtu_transceive();

		modbus_rtu_status = MODBUS_RTU_RECEIVING;
		return 1;
	}
	return 0;
}

modbus_rtu_status_t modbus_rtu_check()
{
	uint8_t hold = USART_data_size();
	if (modbus_rtu_status == MODBUS_RTU_RECEIVING)
	{
		if (hold == 8)
		{
			modbus_rtu_timed = 0;
			stop_timer();
			if (modbus_rtu_frame.crc == swap_endianness(CRC16_2((uint8_t *)&modbus_rtu_frame, 6)))
				modbus_rtu_status = MODBUS_RTU_OK;
			else
				modbus_rtu_status = MODBUS_RTU_FRAME_ERR;
		}
		if (modbus_rtu_timed)
		{
			modbus_rtu_timed = 0;
			stop_timer();
			modbus_rtu_status = MODBUS_RTU_TIMED_OUT;
			if (hold == 4)
			{
				modbus_rtu_frame_err_t *frame = (modbus_rtu_frame_err_t *)&modbus_rtu_frame;
				if (frame->crc == modbus_crc_err_frame.crc)
				{
					modbus_rtu_status = MODBUS_RTU_CRC_ERR;
				}
				else if (frame->crc == modbus_reg_err_frame.crc)
				{
					modbus_rtu_status = MODBUS_RTU_REG_ERR;
				}
			}
		}
	}
	return modbus_rtu_status;
}

uint16_t modbus_rtu_data()
{
	return swap_endianness(modbus_rtu_frame.data);
}

ISR(TIMER0_OVF_vect)
{
	static uint8_t i = 0;
	if (++i == 62 * 3)
	{
		modbus_rtu_timed = 1;
		stop_timer();
		i = 0;
	}
}