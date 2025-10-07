/*
 * timer.h
 *
 *  Created on: Oct 7, 2025
 *      Author: l1ttled1no
 */

#ifndef INC_TIMER_H_
#define INC_TIMER_H_

#include "main.h"


extern uint8_t timerFlag1;
extern uint8_t timerFlag2;
extern uint8_t timerFlag3;
extern uint8_t timerFlag4;
extern uint8_t timerFlag5;


void timerInit();

void setTimer1(uint16_t); 
void setTimer2(uint16_t); 
void setTimer3(uint16_t); 
void setTimer4(uint16_t); 
void setTimer5(uint16_t); 

void timer1Run();
void timer2Run();
void timer3Run();
void timer4Run();
void timer5Run();

void resetTimer1();
void resetTimer2();
void resetTimer3();
void resetTimer4();
void resetTimer5();


#endif /* INC_TIMER_H_ */
