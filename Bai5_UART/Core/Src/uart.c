/*
 * uart.c
 *
 *  Created on: Sep 26, 2023
 *      Author: HaHuyen
 */
#include "uart.h"
#include "ds3231.h"

uint8_t receive_buffer1 = 0;
uint8_t msg[100];

extern uint16_t head, tail;
extern uint8_t data_available_flag;
void uart_init_rs232(){
	HAL_UART_Receive_IT(&huart1, &receive_buffer1, 1);
}

void uart_Rs232SendString(uint8_t* str){
	HAL_UART_Transmit(&huart1, (void*)msg, sprintf((void*)msg,"%s",str), 10);
}

void uart_Rs232SendBytes(uint8_t* bytes, uint16_t size){
	HAL_UART_Transmit(&huart1, bytes, size, 10);
}

void uart_Rs232SendNum(uint32_t num){
	if(num == 0){
		uart_Rs232SendString("0");
		return;
	}
    uint8_t num_flag = 0;
    int i;
	if(num < 0) uart_Rs232SendString("-");
    for(i = 10; i > 0; i--)
    {
        if((num / mypow(10, i-1)) != 0)
        {
            num_flag = 1;
            sprintf((void*)msg,"%d",num/mypow(10, i-1));
            uart_Rs232SendString(msg);
        }
        else
        {
            if(num_flag != 0)
            	uart_Rs232SendString("0");
        }
        num %= mypow(10, i-1);
    }
}

void uart_Rs232SendNumPercent(uint32_t num)
{
	sprintf((void*)msg,"%ld",num/100);
    uart_Rs232SendString(msg);
    uart_Rs232SendString(".");
    sprintf((void*)msg,"%ld",num%100);
    uart_Rs232SendString(msg);
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART1) {
        // Add the received byte to the ring buffer
        ring_buffer[head] = receive_buffer1;
        head = (head + 1) % RING_BUFFER_SIZE;

        // Check if the buffer is full (head == tail indicates overflow)
        if (head == tail) {
            // Handle overflow: adjust tail to discard oldest data
            tail = (tail + 1) % RING_BUFFER_SIZE;
        }

        // Set flag to indicate new data is available for processing
        data_available_flag = 1;

        // Re-enable UART interrupt for the next byte
        HAL_UART_Receive_IT(&huart1, &receive_buffer1, 1);
    }
}


