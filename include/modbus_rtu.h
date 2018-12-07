#ifndef _MODBUS_RTU_H
#define _MODBUS_RTU_H

#define MODBUS_RTU_ADDR 0x15

#define MODBUS_RTU_CMD_W 0x06
#define MODBUS_RTU_CMD_R 0x03

#define MODBUS_RTU_CMD_CRC 0xFF
#define MODBUS_RTU_CMD_REG 0xFE

typedef enum
{
	MODBUS_RTU_BUSY = 0,
	MODBUS_RTU_OK,
	MODBUS_RTU_ERR_CRC,
	MODBUS_RTU_ERR_REG,
	MODBUS_RTU_ERR,
} modbus_rtu_status_t;

void modbus_rtu_init(void);
uint8_t modbus_rtu_write(uint16_t reg, uint16_t data);
uint8_t modbus_rtu_read(uint16_t reg, uint16_t n /* always 1 */);
modbus_rtu_status_t modbus_rtu_check();
uint16_t modbus_rtu_data();

#endif //_MODBUS_RTU_H