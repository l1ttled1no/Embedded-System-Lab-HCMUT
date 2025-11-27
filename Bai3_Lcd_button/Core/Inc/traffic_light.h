/*
 * traffic_light.h
 *
 * Created on: Nov 12, 2024
 * Author: Admin
 */

#ifndef INC_TRAFFIC_LIGHT_H_
#define INC_TRAFFIC_LIGHT_H_

#include <stdint.h>

// Enum to define traffic light states
typedef enum {
    NS_GREEN_EW_RED,
    NS_YELLOW_EW_RED,
    NS_RED_EW_GREEN,
    NS_RED_EW_YELLOW
} TrafficState;

// Enum to define the system state
typedef enum {
    NORMAL,
    MODIFY_RED_LIGHT,
    MODIFY_GREEN_LIGHT,
    MODIFY_YELLOW_LIGHT
} SystemState;

// Function prototypes
void TrafficLight_Update(void);
void handleSwitchState(void);
void handleIncreaseCounter(void);
void handleConfirmValue(void);
void Blink(void) ;

// Extern variable for system state
extern SystemState systemState;

#endif /* INC_TRAFFIC_LIGHT_H_ */
