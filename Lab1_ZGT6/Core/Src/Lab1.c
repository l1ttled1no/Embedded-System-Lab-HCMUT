/*
 * Lab1.c
 *
 *  Created on: Oct 6, 2025
 *      Author: l1ttled1no
 */

#include "Lab1.h"
#if defined(output_debugLED_Pin) || defined(output_Y0_Pin) || defined(output_Y1_Pin)

#define RED_TIMER 5
#define YEL_TIMER 1
#define GRN_TIMER 3

volatile uint8_t timerCount = 0; // in seconds
enum trafficStateDef{
	RED = 1,
	YEL = 2,
	GRN = 3
};

uint8_t trafficState = 0;

void Exercise1(){
	/*
	 * Implementing debug LED on for 2 secs, then turn off for 4 secs
	 */
	HAL_GPIO_WritePin(output_debugLED_GPIO_Port, output_debugLED_Pin, 1); // Turn on LED
	HAL_Delay(2000); // Delay for 2 seconds
	HAL_GPIO_WritePin(output_debugLED_GPIO_Port, output_debugLED_Pin, 0);
	HAL_Delay(4000); // Delay for 4 seconds
}

void init_Exercise2(){
	HAL_GPIO_WritePin(output_debugLED_GPIO_Port, output_debugLED_Pin, 1);
	timerCount = 2;
}

void Exercise2(){
	/*
	 * Implementing debug LED on for 2 secs, then turn off for 4 secs with 1 HAL_Delay
	 */
	if (timerCount == 0){ // If the counter is counted to 0, set back the timer
		if (HAL_GPIO_ReadPin(output_debugLED_GPIO_Port, output_debugLED_Pin) == 1) {
			timerCount = 4; // Timer for turn off state
		} else {
			timerCount = 2; // Timer for turn on state
		}
		HAL_GPIO_TogglePin(output_debugLED_GPIO_Port, output_debugLED_Pin);
	}
	timerCount = timerCount - 1; // Reduce the timer

#if defined(EVAL_BOARD) // for debugging
	HAL_GPIO_TogglePin(LD6_GPIO_Port, LD6_Pin);
#endif
	HAL_Delay(1000);
}
//-------------------------------------------------------------

/*
 * Support functions for exercise 3
 */

/*
	 * output_debugLED: Red
	 * output_Y0: Yel
	 * output_Y1: Grn
	 */

void redLedOn(){
	HAL_GPIO_WritePin(output_debugLED_GPIO_Port, output_debugLED_Pin, 1);
	HAL_GPIO_WritePin(output_Y0_GPIO_Port, output_Y0_Pin, 0);
	HAL_GPIO_WritePin(output_Y1_GPIO_Port, output_Y1_Pin, 0);

}

void yelLedOn(){
	HAL_GPIO_WritePin(output_debugLED_GPIO_Port, output_debugLED_Pin, 0);
	HAL_GPIO_WritePin(output_Y0_GPIO_Port, output_Y0_Pin, 1);
	HAL_GPIO_WritePin(output_Y1_GPIO_Port, output_Y1_Pin, 0);
}

void grnLedOn(){
	HAL_GPIO_WritePin(output_debugLED_GPIO_Port, output_debugLED_Pin, 0);
	HAL_GPIO_WritePin(output_Y0_GPIO_Port, output_Y0_Pin, 0);
	HAL_GPIO_WritePin(output_Y1_GPIO_Port, output_Y1_Pin, 1);
}

void error(){
	HAL_GPIO_WritePin(output_debugLED_GPIO_Port, output_debugLED_Pin, 1);
	HAL_GPIO_WritePin(output_Y0_GPIO_Port, output_Y0_Pin, 1);
	HAL_GPIO_WritePin(output_Y1_GPIO_Port, output_Y1_Pin, 1);

}

void init_Exercise3(){
	// set to red LED and set the timer
	trafficState = RED;
	timerCount 	 = RED_TIMER;
	redLedOn();
}

void Exercise3(){
	/*
	 Using LED3 and output Y0, Y1 as LED. Make the traffic light with
	 Red as 5s, Green is 3s, and Yellow is 1 seconds
	 */
	if (timerCount == 0){
		switch(trafficState){
		case RED:
			trafficState = GRN;
#ifdef GRN_TIMER
			timerCount   = GRN_TIMER;
#else
			timerCount   = 3;
#endif
			grnLedOn();
			break;
		case GRN:
			trafficState = YEL;
#ifdef YEL_TIMER
			timerCount   = YEL_TIMER;
#else
			timerCount   = 1;
#endif
			yelLedOn();
			break;
		case YEL:
			trafficState = RED;
#ifdef RED_TIMER
			timerCount  = RED_TIMER;
#else
			timerCount  = 5;
#endif
			redLedOn();
			break;
		default: //Should not be enter this section
			trafficState = 255;
			timerCount = 255;
			error();
			break;
		}
	}
	timerCount = timerCount - 1;
#if defined(EVAL_BOARD) // for debugging
	HAL_GPIO_TogglePin(LD6_GPIO_Port, LD6_Pin);
#endif
	HAL_Delay(1000);
}

#else

#endif
