# O Projeto

O presente projeto consiste de uma implementação do protocolo de comunicação [Modbus RTU][1] no ATmega328p, para realizar a comunicação com um ESP-01 que deve então transmitir à Internet as informações de um ou mais sensores através do protocolo MQTT. Nesse caso os dados transimitidos originam-se dos sensores de aceleração e velocidade angular do módulo [GY-521][3] que utiliza o [MPU6050][4].

---------------------------------------

## Módulo GY-521 MPU6050

Algum texto explicando o funcionamento do sensor, talvez falar um pouco sobre a comunicação  e tratamento de dados.

### I2C

Uma pequena explicação sobre a comunicação com o dispositivo.

### Acelerômetro

Falar sobre o acelerometro do MPU

### Giroscópio

Falar sobre o giroscópio do MPU

---------------------------------------

## Modbus

O módulo WiFi ESP-01 possui um firmware customizado. A comunicação entre o módulo WiFi e o ATmega328P é feita com uma versão simplificada do protocolo Modbus RTU através de uma interfaçe serial (UART) de ambos os microcontroladores. Essa versão simplificada do protocolo apresenta as seguintes características:

+ Endereço: 0x15
+ Comandos:
  + Escrita: 0x01
  + Leitura: 0x02

### Registradores

|Descrição|Endereço|Modo|
|:-:|:-:|:-:|
|estado|0x0000|Somente Leitura|
|atuador_0|0x0001|Somente Leitura|
|atuador_1|0x0002|Somente Leitura|
|atuador_2|0x0003|Somente Leitura|
|atuador_3|0x0004|Somente Leitura|
|sensor_0|0x0005|Somente Escrita|
|sensor_1|0x0006|Somente Escrita|
|sensor_2|0x0007|Somente Escrita|
|sensor_3|0x0008|Somente Escrita|

### CRC

O [CRC][2] foi calculado pela função abaixo, que foi fornecida nas especificações do projeto.

```c
uint16_t CRC16_2(uint8_t *buf, int len)
{
    uint32_t crc = 0xFFFF;
    int i;
    for (i = 0; i < len; i++)
    {
        crc ^= (uint16_t)buf[i]; // XOR byte into least sig. byte of crc
        for (int i = 8; i != 0; i--) { // Loop over each bit
            if ((crc & 0x0001) != 0) { // If the LSB is set
                crc >>= 1; // Shift right and XOR 0xA001
                crc ^= 0xA001;
            }
            else // Else LSB is not set
                crc >>= 1; // Just shift right
        }
    }
    return crc;
}
```

### Interface

A interface seria utilizada foi a USART (UART) através do próprio *hardware* do ATmega328p, o software reailiza transmições no formato *blocking*, já para recepções foi utilizada a interrupção `ISR(USART_RX_vect)`.

### Erros

Essa implementação da comunicação via modbus pode apresentar quatro erros, são eles:

+ CRC_ERROR: Quando o modulo ESP-01 responde com frame de CRC inválido.
+ REGISTER_ERROR: Quando o módulo ESP-01 responde com frame de registrador inválido.
+ FRAME_ERROR: Quando o frame de resposta do módulo ESP-01 contém um tamanho ou CRC inválido.
+ TIMEOUT_ERROR: Quando o módulo demora para responder.

A visualização dos erros é feita através de três *leds*.

|PORTB5|PORTB0 (D8)|PORTB1 (D9)|ERRO|
|:-:|:-:|:-:|:-:|
|1|0|0|CRC_ERROR|
|1|0|1|FRAME_ERROR|
|1|1|0|REGISTER_ERROR|
|1|1|1|TIMEOUT_ERROR|

## Colaboradores

+ Ariel Rigueiras Montardo
+ Raquel Dutra Kotzias

[1]: https://pt.wikipedia.org/wiki/Modbus "Wikipédia"
[2]: https://www.lammertbies.nl/comm/info/crc-calculation.html "CRC Calculation"
[3]: https://playground.arduino.cc/Main/MPU-6050 "Arduino Playground"
[4]: https://github.com/ArielRM/MCC_SENSORES/blob/master/temp/MPU-6000-Datasheet1.pdf "Datasheet"