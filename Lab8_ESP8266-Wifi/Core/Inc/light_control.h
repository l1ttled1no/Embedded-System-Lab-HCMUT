/*
 * light_control.h
 *
 *  Created on: Nov 15, 2023
 *      Author: phamv
 */

#ifndef INC_LIGHT_CONTROL_H_
#define INC_LIGHT_CONTROL_H_

#include "main.h"
#include "gpio.h"
#include "uart.h"
#include "button.h"
#include "lcd.h"
#include "sensor.h"
#include <stdio.h>
#include <string.h>

extern uint8_t light_status;

void lightProcess();

void test_Esp();

void temperatureProcess();

#endif /* INC_LIGHT_CONTROL_H_ */
