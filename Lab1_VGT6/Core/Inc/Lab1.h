/*
 * exercise1.h
 *
 *  Created on: Oct 6, 2025
 *      Author: l1ttled1no
 */

#ifndef INC_LAB1_H_
#define INC_LAB1_H_

#include "main.h"


void init_Exercise2();
void init_Exercise3();
void Exercise1();
void Exercise2();
void Exercise3();

/*
 * The LED is pull-up
 */

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


#endif /* INC_LAB1_H_ */
