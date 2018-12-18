#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "avr_twi_master.h"
#include "mpu6050.h"

static unsigned char req_acer[2] = {WRITE, 0x3B};
static unsigned char req_gyro[2] = {WRITE, 0x43};

static unsigned char read_acer[7] = {READ};
static unsigned char read_gyro[7] = {READ};

volatile mpu6050_t mpu;

//////// static functions
static void mpu6050_req_acer();
static void mpu6050_req_gyro();
static void mpu6050_convertData_acer();
static void mpu6050_convertData_gyro();
////////

void mpu6050_init()
{

    //Pacotes para envio   endereço_mpu, resgistrador_mudança, valor 
    unsigned char wakeUp[3]  = {WRITE, 0x6B, 0x08};     //Acorda o sensor e desabilita temperatura
    unsigned char gyroCfg[3] = {WRITE, 0x1B, 0x18};     //Configura o Gyroscopio para menor sensibilidade
    unsigned char acerCfg[3] = {WRITE, 0x1C, 0x18};     //Configura o Acelerometro para menor sensibilidade

    TWI_Master_Initialise();

    TWI_Start_Transceiver_With_Data(wakeUp, 3);
    TWI_Start_Transceiver_With_Data(acerCfg, 3);
    TWI_Start_Transceiver_With_Data(gyroCfg, 3);

}

void mpu6050_request()
{
    mpu6050_req_acer();

    mpu6050_req_gyro();
}

void mpu6050_convertData()
{
    mpu6050_convertData_acer();
    mpu6050_convertData_gyro();
}

static void mpu6050_req_acer()
{
    //manda o pedido para os dados do acelerometro
    TWI_Start_Transceiver_With_Data(req_acer, 2);
    //pedido de leitura dos dados 
    TWI_Start_Transceiver_With_Data(read_acer, 7);

    while(1)
    {
        if(!TWI_Transceiver_Busy())
        {
            if(TWI_statusReg.lastTransOK)
            {
                //pega os dados para um vetor
                TWI_Get_Data_From_Transceiver(read_acer, 7);
                //PORTB ^= (1<<PB5);
                break;
            }
        }

    }
}


static void mpu6050_req_gyro()
{
    while(TWI_Transceiver_Busy());

    TWI_Start_Transceiver_With_Data(req_gyro,2);
    TWI_Start_Transceiver_With_Data(read_gyro,7);

    while(1)
    {
        if(!TWI_Transceiver_Busy())
        {
            if(TWI_statusReg.lastTransOK)
            {
                //pega os dados para um vetor
                TWI_Get_Data_From_Transceiver(read_gyro, 7);
                //PORTB ^= (1<<PB5);
                break;
            }
        }

    }
}

static void mpu6050_convertData_acer()
{
    //converte e coloca os dados na estrutura 
    mpu.acer_x = (read_acer[1]<<8) | read_acer[2];
    mpu.acer_x = mpu.acer_x*ACER_CONV;

    mpu.acer_y = (read_acer[3]<<8) | read_acer[4];
    mpu.acer_y = mpu.acer_y*ACER_CONV;

    mpu.acer_z = (read_acer[5]<<8) | read_acer[6];
    mpu.acer_z = mpu.acer_z*ACER_CONV;

}

static void mpu6050_convertData_gyro()
{

    mpu.gyro_x = (read_gyro[1]<<8) | read_gyro[2];
    mpu.gyro_x = mpu.gyro_x*GYRO_CONV;

    mpu.gyro_y = (read_gyro[3]<<8) | read_gyro[4];
    mpu.gyro_y = mpu.gyro_y*GYRO_CONV;

    mpu.gyro_z = (read_gyro[5]<<8) | read_gyro[6];
    mpu.gyro_z = mpu.gyro_z*GYRO_CONV;

}