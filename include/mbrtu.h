#ifndef MBRTU_H_INCLUDED
#define MBRTU_H_INCLUDED

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "avr_usart.h"
#include "avr_timer.h"

#define MBRTU_CMD_WRITE 0x01
#define MBRTU_CMD_READ 0x02

#define MBRTU_DATA_BYTES 2 

#define MBRTU_BAUD B9600

/*! Configurações do Timer */
#define MBRTU_TIMER_CFG TIMER0_CFG
#define MBRTU_TIMER_IRQ TIMER0_IRQ
#define MBRTU_TIMER_COMPA TIMER0_COMPA_vect
/*
Baud Rate -> B
Tempo de silêncio -> TS

TS = (7/2) * 8 / B

MBRTU_OCRA = (TS * F_CPU / 1024) + 1
MBRTU_OCRA = 7 * F_CPU / 256 / B + 1

MBRTU_BAUD = F_CPU / 16 / B - 1;

(MBRTU_OCRA - 1)/(MBRTU_BAUD + 1) = 7 * F_CPU * 16 * B / 256 / B / F_CPU
(MBRTU_OCRA - 1)/(MBRTU_BAUD + 1) = 7 / 16
MBRTU_OCRA = 7 * (MBRTU_BAUD + 1) / 16 + 1
*/
#define MBRTU_OCRA (7 * (MBRTU_BAUD + 1) / 16 + 1)

void mbrtu_init();
uint8_t mbrtu_send(uint8_t addr, uint16_t reg, uint16_t data);
uint8_t mbrtu_get(uint8_t *frame);

#endif //MBRTU_H_INCLUDED