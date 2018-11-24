/*
 * avr_timer.h
 *
 *  Created on: Mar 27, 2018
 *      Author: Renan Augusto Starke
 *      Instituto Federal de Santa Catarina
 */

#ifndef LIB_AVR_TIMER_H_
#define LIB_AVR_TIMER_H_

#include <stdint.h>
#include <avr/io.h>

#ifndef _IO
#define _IO volatile uint8_t
#endif

#ifndef _IO16
#define _IO16 volatile uint16_t
#endif

typedef struct
{
	union {
		_IO MASK;
		struct
		{
			_IO WGM0 : 1;
			_IO WGM1 : 1;
			_IO _unused : 2;
			_IO COMB0 : 1;
			_IO COMB1 : 1;
			_IO COMA0 : 1;
			_IO COMA1 : 1;
		} BITS;
	} TCCRA; /*!< Config Register A */

	union {
		_IO MASK;
		struct
		{
			_IO CS0 : 1;
			_IO CS1 : 1;
			_IO CS2 : 1;
			_IO WGM2 : 1;
			_IO WGM3 : 1;
			_IO _unused : 1;
			_IO ICES : 1;
			_IO ICNC : 1;
		} BITS;
	} TCCRB; /*!< Config Register B */

	union {
		_IO MASK;
		struct
		{
			_IO _unused : 6;
			_IO FOCB : 1;
			_IO FOCA : 1;
		} BITS;
	} TCCRC; /*!< Config Register C */

	_IO unused;
	_IO16 TCNT; /*!< Counter Register */
	_IO16 ICR;  /*!< Input Capture Register */
	_IO16 OCRA; /*!< Compare Register: channel A */
	_IO16 OCRB; /*!< Compare Register: channel B */
} TIMER16_CFG_t;

typedef struct
{
	union {
		_IO MASK;
		struct
		{
			_IO WGM0 : 1;
			_IO WGM1 : 1;
			_IO _unused : 2;
			_IO COMB0 : 1;
			_IO COMB1 : 1;
			_IO COMA0 : 1;
			_IO COMA1 : 1;
		} BITS;
	} TCCRA; /*!< Config Register A */

	union {
		_IO MASK;
		struct
		{
			_IO CS0 : 1;
			_IO CS1 : 1;
			_IO CS2 : 1;
			_IO WGM2 : 1;
			_IO _unused : 2;
			_IO FOCB : 1;
			_IO FOCA : 1;
		} BITS;
	} TCCRB;  /*!< Config Register B */
	_IO TCNT; /*!< Counter Register */
	_IO OCRA; /*!< Compare Register: channel A */
	_IO OCRB; /*!< Compare Register: channel B */
} TIMER8_CFG_t;

typedef union {
	_IO MASK;
	struct
	{
		_IO TOIE : 1;  /*!< Overflow IRQ enable */
		_IO OCIEA : 1; /*!< Compare IRQ: channel A */
		_IO OCIEB : 1; /*!< Compare IRQ: channel B */
	} BITS;
} TIMER8_IRQ_t;

typedef union {
	_IO MASK;
	struct
	{
		_IO TOIE : 1;  /*!< Overflow IRQ enable */
		_IO OCIEA : 1; /*!< Compare IRQ: channel A enable */
		_IO OCIEB : 1; /*!< Compare IRQ: channel B enable */
		_IO _unused : 2;
		_IO ICIE : 1; /*!< Capture IRQ enable  */
	} BITS;
} TIMER16_IRQ_t;

#define TIMER0_CFG ((TIMER8_CFG_t *)&TCCR0A)
#define TIMER1_CFG ((TIMER16_CFG_t *)&TCCR1A)
#define TIMER2_CFG ((TIMER8_CFG_t *)&TCCR2A)

#define TIMER0_IRQ ((TIMER8_IRQ_t *)&TIMSK0)
#define TIMER1_IRQ ((TIMER16_IRQ_t *)&TIMSK1)
#define TIMER2_IRQ ((TIMER8_IRQ_t *)&TIMSK2)

#endif /* LIB_AVR_TIMER_H_ */
