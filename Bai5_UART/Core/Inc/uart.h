/*
 * uart.h
 *
 *  Created on: Sep 26, 2023
 *      Author: HaHuyen
 */
#ifndef INC_UART_H_
#define INC_UART_H_

#include "usart.h"
#include <stdio.h>
#include "utils.h"

#define RING_BUFFER_SIZE 10
extern uint8_t ring_buffer[RING_BUFFER_SIZE];

void uart_init_rs232();

void uart_Rs232SendString(uint8_t* str);

void uart_Rs232SendBytes(uint8_t* bytes, uint16_t size);

void uart_Rs232SendNum(uint32_t num);

void uart_Rs232SendNumPercent(uint32_t num);

void ProcessRingBufferData();

#endif /* INC_UART_H_ */

