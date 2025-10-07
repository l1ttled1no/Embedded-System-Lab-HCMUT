#include "Lab2.h"

#define RED_TIMER 5000
#define YEL_TIMER 1000
#define GRN_TIMER 3000

enum trafficStateDef{
	RED = 1,
	YEL = 2,
	GRN = 3
};
uint8_t trafficState = 0;

void init_exercise1(){
    // Initialize the timer
    timerInit(); 
    HAL_GPIO_WritePin(output_debugLED_GPIO_Port, output_debugLED_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(output_Y0_GPIO_Port, output_Y0_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(output_Y1_GPIO_Port, output_Y1_Pin, GPIO_PIN_SET);
    setTimer1(2000);
    setTimer2(2000);
    setTimer3(5000);
}

// Helper functions for exercise 2

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

void init_exercise2(){
    trafficState = RED; 
    setTimer1(RED_TIMER);
    redLedOn();
}





// Exercise 1
uint16_t timer2_ = 0;
uint16_t timer3_ = 0;
void exercise1(){
    if(timerFlag1 == 1){
        HAL_GPIO_TogglePin(output_debugLED_GPIO_Port, output_debugLED_Pin);
        setTimer1(2000);
    }
    if(timerFlag2 == 1){
        if (HAL_GPIO_ReadPin(output_Y0_GPIO_Port, output_Y0_Pin) == GPIO_PIN_SET){
            setTimer2(4000);
        } else 
            setTimer2(2000);
        HAL_GPIO_TogglePin(output_Y0_GPIO_Port, output_Y0_Pin);
    }
    if(timerFlag3 == 1){
        if (HAL_GPIO_ReadPin(output_Y1_GPIO_Port, output_Y1_Pin) == GPIO_PIN_SET){
            setTimer3(1000);
        } else 
            setTimer3(5000);
        HAL_GPIO_TogglePin(output_Y1_GPIO_Port, output_Y1_Pin);

    }
}

// Exercise 2

void exercise2(){
    switch(trafficState){
        case RED:
            if(timerFlag1 == 1){
                trafficState = GRN;
                grnLedOn();
                setTimer1(GRN_TIMER);
            }
            break;
        case YEL:
            if(timerFlag1 == 1){
                trafficState = RED;
                redLedOn();
                setTimer1(RED_TIMER);
            }
            break;
        case GRN:
            if(timerFlag1 == 1){
                trafficState = YEL;
                yelLedOn();
                setTimer1(YEL_TIMER);
            }
            break;
        default:
            error();
            break;
    }
}