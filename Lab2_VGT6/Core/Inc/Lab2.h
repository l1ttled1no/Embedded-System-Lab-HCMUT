/*
 * Lab2.h
 *
 *  Created on: Oct 7, 2025
 *      Author: l1ttled1no
 */

#ifndef INC_LAB2_H_
#define INC_LAB2_H_

#include "main.h"
#include "timer.h"

#define SEG_REFRESH_RATE_HZ 100

void init_exercise1();
void init_exercise2(); 


void exercise1();
void exercise2();
void exercise3();
void exercise4();
void exercise5();

#if defined(EVAL_BOARD)
	#define output_debugLED_Pin LD5_Pin
	#define output_debugLED_GPIO_Port LD5_GPIO_Port
	#define output_Y0_Pin LD3_Pin
	#define output_Y0_GPIO_Port LD3_GPIO_Port
	#define output_Y1_Pin LD4_Pin
	#define output_Y1_GPIO_Port LD4_GPIO_Port
#else
	#define output_debugLED_Pin GPIO_PIN_4
	#define output_debugLED_GPIO_Port GPIOE
	#define output_Y0_Pin GPIO_PIN_5
	#define output_Y0_GPIO_Port GPIOE
	#define output_Y1_Pin GPIO_PIN_6
	#define output_Y1_GPIO_Port GPIOE
#endif


#endif /* INC_LAB2_H_ */
