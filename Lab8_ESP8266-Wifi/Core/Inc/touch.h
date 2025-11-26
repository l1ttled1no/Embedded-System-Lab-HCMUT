/*
 * touch.h
 *
 *  Created on: Sep 22, 2023
 *      Author: phamv
 */

#ifndef INC_TOUCH_H_
#define INC_TOUCH_H_

#include <stdio.h>
#include "software_timer.h"

#define TP_PRES_DOWN 0x80
#define TP_CATH_PRES 0x40
#define CT_MAX_TOUCH  5

typedef struct
{
	void (*init)(void);
	uint8_t (*scan)(uint8_t);
	void (*adjust)(void);
	uint16_t x[CT_MAX_TOUCH];
	uint16_t y[CT_MAX_TOUCH];

	uint8_t  sta;

	float xfac;
	float yfac;
	short xoff;
	short yoff;
	uint8_t touchtype;
}_m_tp_dev;

extern _m_tp_dev tp_dev;

void touch_init(void);
void touch_Adjust(void);
void touch_Scan(void);
uint8_t touch_IsTouched();
uint16_t touch_GetX();
uint16_t touch_GetY();

#endif /* INC_TOUCH_H_ */
