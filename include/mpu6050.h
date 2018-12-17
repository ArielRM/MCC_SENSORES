#ifndef MPU6050_H_INCLUDED
#define MPU6050_H_INCLUDED

#define ACER_CONV 5		// g*1000/sensitivity ->raw*ACER_CONV = value*1000 m/s^2
#define GYRO_CONV 61	// 1000/sensitivity -> raw*GYRO_CONV = value*1000 degrees/s

#define ADDR 0xD0       //8-bit adreesss
#define READ ADDR | 1
#define WRITE ADDR

typedef struct mpu6050
{
    uint16_t acer_x;
    uint16_t acer_y;
    uint16_t acer_z;

    uint16_t gyro_x;
    uint16_t gyro_y;
    uint16_t gyro_z;
}mpu6050_t;

void mpu6050_init();

void mpu6050_request();

void mpu6050_convertData();

extern volatile mpu6050_t mpu;

#endif //MPU6050_H_INCLUDED