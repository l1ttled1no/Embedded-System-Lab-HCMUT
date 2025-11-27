#include "traffic_light.h"
#include "main.h"
#include "lcd.h"

// Light durations
int GREEN_DURATION = 5;
int YELLOW_DURATION = 2;
int RED_DURATION = 7;

// Traffic light and system states
TrafficState currentState = NS_GREEN_EW_RED;


// Counters
uint8_t stateCounter = 1;
uint8_t counter = 0;
uint8_t counter_blink =0;
uint8_t GREEN_COUNTER = 1;
uint8_t YELLOW_COUNTER = 1;
uint8_t RED_COUNTER = 1;

void Blink(void) {
    counter_blink = (counter_blink + 1) % 10;  // Increment and wrap around every 10 ticks
    if(systemState != NORMAL){
		if (counter_blink == 0) {  // Toggle every 10 ticks
			static uint8_t isLightOn = 1;  // Track light state (on/off)

			if (isLightOn) {
				// Turn off the lights (white circle)
				lcd_DrawCircle(60, 120, WHITE, 40, 1);
				lcd_DrawCircle(160, 120, WHITE, 40, 1);
			} else {
				// Display the current light state in modify mode
				switch (systemState) {
					case MODIFY_RED_LIGHT:
						lcd_DrawCircle(160, 120, RED, 40, 1);  // Show RED
						lcd_ShowIntNum(140, 105, RED_DURATION, 2, WHITE, RED, 32);
						break;
					case MODIFY_GREEN_LIGHT:
						lcd_DrawCircle(60, 120, GREEN, 40, 1);  // Show GREEN
						lcd_ShowIntNum(40, 105, GREEN_DURATION, 2, WHITE, GREEN, 32);
						break;
					case MODIFY_YELLOW_LIGHT:
						lcd_DrawCircle(160, 120, YELLOW, 40, 1);  // Show YELLOW
						lcd_ShowIntNum(140, 105, YELLOW_DURATION, 2, WHITE, YELLOW, 32);
						break;
					default:
						break;
				}
			}

			isLightOn = !isLightOn;  // Toggle light state
		}
    }
}



void TrafficLight_Update(void) {
    counter = (counter + 1) % 20;
//    counter_blink =(counter_blink+1)%10;
    if (counter == 0) {
        if (systemState == NORMAL) {
            stateCounter++;
            GREEN_COUNTER++;
            YELLOW_COUNTER++;
            RED_COUNTER++;

            // Traffic light state transitions
            switch (currentState) {
                case NS_GREEN_EW_RED:
					lcd_DrawCircle(60, 120, GREEN, 40, 1);
					lcd_DrawCircle(160, 120, RED, 40, 1);
                    lcd_ShowIntNum(40, 105, GREEN_COUNTER, 2, WHITE, GREEN, 32);
                    lcd_ShowIntNum(140, 105, RED_COUNTER, 2, WHITE, RED, 32);
                    if (stateCounter > GREEN_DURATION) {
                        currentState = NS_YELLOW_EW_RED;
                        stateCounter = 1;
                        GREEN_COUNTER = 1;
                        YELLOW_COUNTER = 1;

						lcd_DrawCircle(60, 120, YELLOW, 40, 1);
						lcd_DrawCircle(160, 120, RED, 40, 1);


                        lcd_ShowIntNum(40, 105, YELLOW_COUNTER, 2, WHITE, YELLOW, 32);
                        lcd_ShowIntNum(140, 105, RED_COUNTER, 2, WHITE, RED, 32);
                    }
                    break;

                case NS_YELLOW_EW_RED:
                	lcd_DrawCircle(60, 120, YELLOW, 40, 1);
                	lcd_DrawCircle(160, 120, RED, 40, 1);
                    lcd_ShowIntNum(40, 105, YELLOW_COUNTER, 2, WHITE, YELLOW, 32);
                    lcd_ShowIntNum(140, 105, RED_COUNTER, 2, WHITE, RED, 32);
                    if (stateCounter > YELLOW_DURATION) {
                        currentState = NS_RED_EW_GREEN;
                        stateCounter = 1;
                        YELLOW_COUNTER = 1;
                        RED_COUNTER = 1;
                        GREEN_COUNTER = 1;

						lcd_DrawCircle(60, 120, RED, 40, 1);
						lcd_DrawCircle(160, 120, GREEN, 40, 1);



                        lcd_ShowIntNum(40, 105, RED_COUNTER, 2, WHITE, RED, 32);
                        lcd_ShowIntNum(140, 105, GREEN_COUNTER, 2, WHITE, GREEN, 32);
                    }
                    break;

                case NS_RED_EW_GREEN:
                	lcd_DrawCircle(60, 120, RED, 40, 1);
                	lcd_DrawCircle(160, 120, GREEN, 40, 1);
                    lcd_ShowIntNum(40, 105, RED_COUNTER, 2, WHITE, RED, 32);
                    lcd_ShowIntNum(140, 105, GREEN_COUNTER, 2, WHITE, GREEN, 32);
                    if (stateCounter > GREEN_DURATION) {
                        currentState = NS_RED_EW_YELLOW;
                        stateCounter = 1;
                        GREEN_COUNTER = 1;
                        YELLOW_COUNTER = 1;

						lcd_DrawCircle(60, 120, RED, 40, 1);
						lcd_DrawCircle(160, 120, YELLOW, 40, 1);

                        lcd_ShowIntNum(40, 105, RED_COUNTER, 2, WHITE, RED, 32);
                        lcd_ShowIntNum(140, 105, YELLOW_COUNTER, 2, WHITE, YELLOW, 32);
                    }
                    break;

                case NS_RED_EW_YELLOW:
        			lcd_DrawCircle(60, 120, RED, 40, 1);
        			lcd_DrawCircle(160, 120, YELLOW, 40, 1);
                    lcd_ShowIntNum(40, 105, RED_COUNTER, 2, WHITE, RED, 32);
                    lcd_ShowIntNum(140, 105, YELLOW_COUNTER, 2, WHITE, YELLOW, 32);
                    if (stateCounter > YELLOW_DURATION) {
                        currentState = NS_GREEN_EW_RED;
                        stateCounter = 1;
                        GREEN_COUNTER = 1;
                        RED_COUNTER = 1;

						lcd_DrawCircle(60, 120, GREEN, 40, 1);
						lcd_DrawCircle(160, 120, RED, 40, 1);


                        lcd_ShowIntNum(40, 105, GREEN_COUNTER, 2, WHITE, GREEN, 32);
                        lcd_ShowIntNum(140, 105, RED_COUNTER, 2, WHITE, RED, 32);
                    }
                    break;
            }
        }
    }
//    else if (counter_blink == 0){
//    	lcd_DrawCircle(60, 120, WHITE, 40, 1);
//    	lcd_DrawCircle(160, 120, WHITE, 40, 1);
//    }
}

// Button handling functions
void handleSwitchState(void) {
    switch (systemState) {
        case NORMAL:
            systemState = MODIFY_RED_LIGHT;
            RED_DURATION =0;
            GREEN_DURATION=0;
            YELLOW_DURATION=0;
        	lcd_StrCenter(0, 2, "Modify state", RED, BLUE, 16, 1);
        	lcd_ShowStr(20, 30, "MODIFY_RED_LIGHT", WHITE, RED, 24, 0);
            break;
        case MODIFY_RED_LIGHT:
            systemState = MODIFY_GREEN_LIGHT;
        	lcd_StrCenter(0, 2, "Modify state", RED, BLUE, 16, 1);
        	lcd_ShowStr(20, 30, "MODIFY_GREEN_LIGHT", WHITE, RED, 24, 0);
            break;
        case MODIFY_GREEN_LIGHT:
            systemState = MODIFY_YELLOW_LIGHT;
        	lcd_StrCenter(0, 2, "Modify state", RED, BLUE, 16, 1);
        	lcd_ShowStr(20, 30, "MODIFY_YELLOW_LIGHT", WHITE, RED, 24, 0);
            break;
        case MODIFY_YELLOW_LIGHT:
            systemState = NORMAL;
        	lcd_StrCenter(0, 2, "Normal state", RED, BLUE, 16, 1);
        	lcd_ShowStr(20, 30, "Traffic light", WHITE, RED, 24, 0);
            break;
    }
}

void handleIncreaseCounter(void) {
    switch (systemState) {
        case MODIFY_RED_LIGHT:
            RED_DURATION++;
//            lcd_ShowIntNum(140, 105, RED_DURATION, 2, WHITE, RED, 32);
            break;
        case MODIFY_GREEN_LIGHT:
            GREEN_DURATION++;
//            lcd_ShowIntNum(40, 105, GREEN_DURATION, 2, WHITE, GREEN, 32);
            break;
        case MODIFY_YELLOW_LIGHT:
            YELLOW_DURATION++;
//            lcd_ShowIntNum(140, 105, YELLOW_DURATION, 2, WHITE, YELLOW, 32);
            break;
        default:
            break;
    }
}

void handleConfirmValue(void) {
    // Confirm and save the new values, and return to NORMAL state
    systemState = NORMAL;
	lcd_StrCenter(0, 2, "Normal state", RED, BLUE, 16, 1);
	lcd_ShowStr(20, 30, "Traffic light", WHITE, RED, 24, 0);
}
