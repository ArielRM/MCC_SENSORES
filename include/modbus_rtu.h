#ifndef _MODBUS_RTU_H
#define _MODBUS_RTU_H

#define MODBUS_RTU_ADDR ((uint8_t)0x15)

#define MODBUS_RTU_CMD_W ((uint8_t)0x01)
#define MODBUS_RTU_CMD_R ((uint8_t)0x02)

#define MODBUS_RTU_CMD_CRC ((uint8_t)0xFF)
#define MODBUS_RTU_CMD_REG ((uint8_t)0xFE)

typedef enum
{
	MODBUS_RTU_RECEIVING = 0,
	MODBUS_RTU_READY,
	MODBUS_RTU_OK,
	MODBUS_RTU_CRC_ERR,
	MODBUS_RTU_REG_ERR,
	MODBUS_RTU_FRAME_ERR,
	MODBUS_RTU_UNK_ERR,
	MODBUS_RTU_TIMED_OUT,
	MODBUS_RTU_STATE_SIZE,
} modbus_rtu_status_t;

void modbus_rtu_init(void);
uint8_t modbus_rtu_write(uint16_t reg, uint16_t data);
uint8_t modbus_rtu_read(uint16_t reg, uint16_t n /* always 1 */);
modbus_rtu_status_t modbus_rtu_check();
uint16_t modbus_rtu_data();

#endif //_MODBUS_RTU_H