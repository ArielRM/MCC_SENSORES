#ifndef MPU6050_H_INCLUDED
#define MPU6050_H_INCLUDED

#define ACER_CONV 5		// g*1000/sensitivity ->raw*ACER_CONV = value*1000 m/s^2
#define GYRO_CONV 61	// 1000/sensitivity -> raw*GYRO_CONV = value*1000 degrees/s

#define ADDR 0xD0       //8-bit adrees
#define READ ADDR | 1
#define WRITE ADDR

typedef struct mpu6050 mpu6050_t;

extern volatile mpu;

#endif //MPU6050_H_INCLUDED