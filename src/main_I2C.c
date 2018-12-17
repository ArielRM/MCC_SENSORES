#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>

#define F_CPU 16000000UL

#include "mpu6050.h"
#include "../include/avr_usart.h"


int main()
{
    DDRB |= (1<<PB5);
    PORTB &= ~(1<<PB5);
    FILE *usart = USART_init(B9600);
    
    sei();

    mpu6050_init();

    fprintf(usart, "start\r\n");

    while(1)
    {

        mpu6050_request();

        mpu6050_convertData();

        fprintf(usart, "ax = %d, ay = %d, az = %d\r\n"
                       "gx = %d, gy = %d, gz = %d\r\n", 
                    mpu.acer_x, mpu.acer_y, mpu.acer_z,
                    mpu.gyro_x, mpu.gyro_y, mpu.gyro_z);

        _delay_ms(1000);
    }
}