#include "timer.h"

#define SW_TIMER_KHZ 1
#define SW_TIMER_HZ  SW_TIMER_KHZ * 1000 // Software timer Hertz
#define SW_TIMER_MS  1 / SW_TIMER_KHZ
#define SW_TIMER_S   SW_TIMER_MS / 1000

uint8_t timerFlag1 = 0;
uint8_t timerFlag2 = 0;
uint8_t timerFlag3 = 0;
uint8_t timerFlag4 = 0;
uint8_t timerFlag5 = 0;

uint16_t timer1 = 0;
uint16_t timer2 = 0;
uint16_t timer3 = 0;
uint16_t timer4 = 0;
uint16_t timer5 = 0;

void timerInit(){
    // Set all the flags and timers at 0. 
    

    timerFlag1 = 0;
    timerFlag2 = 0;
    timerFlag3 = 0;
    timerFlag4 = 0;
    timerFlag5 = 0;

    timer1 = 0;
    timer2 = 0;
    timer3 = 0;
    timer4 = 0;
    timer5 = 0;

}

void setTimer1(uint16_t duration_ms){
    timer1 = duration_ms/(SW_TIMER_MS); 
    timerFlag1 = 0; 
}

void setTimer2(uint16_t duration_ms){
    timer2 = duration_ms/(SW_TIMER_MS); 
    timerFlag2 = 0; 
}

void setTimer3(uint16_t duration_ms){
    timer3 = duration_ms/(SW_TIMER_MS); 
    timerFlag3 = 0; 
}

void setTimer4(uint16_t duration_ms){
    timer4 = duration_ms/(SW_TIMER_MS); 
    timerFlag4 = 0; 
}

void setTimer5(uint16_t duration_ms){
    timer5 = duration_ms/(SW_TIMER_MS); 
    timerFlag5 = 0; 
}

void timer1Run(){
    if(timer1 > 0){
        timer1--;
        if(timer1 == 0) timerFlag1 = 1;
    }
}
void timer2Run(){
    if(timer2 > 0){
        timer2--;
        if(timer2 == 0) timerFlag2 = 1;
    }
}
void timer3Run(){
    if(timer3 > 0){
        timer3--;
        if(timer3 == 0) timerFlag3 = 1;
    }
}
void timer4Run(){
    if(timer4 > 0){
        timer4--;
        if(timer4 == 0) timerFlag4 = 1;
    }
}
void timer5Run(){
    if(timer5 > 0){
        timer5--;
        if(timer5 == 0) timerFlag5 = 1;
    }
}

void resetTimer1(){
    timer1 = 0;
    timerFlag1 = 0;
}
void resetTimer2(){
    timer2 = 0;
    timerFlag2 = 0;
}
void resetTimer3(){
    timer3 = 0;
    timerFlag3 = 0;
}
void resetTimer4(){
    timer4 = 0;
    timerFlag4 = 0;
}
void resetTimer5(){
    timer5 = 0;
    timerFlag5 = 0;
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	if(htim->Instance == TIM2){
		// HAL_GPIO_TogglePin(LD6_GPIO_Port, LD6_Pin);
        timer1Run();
        timer2Run();
        timer3Run();
        timer4Run();
        timer5Run();
	}
}
