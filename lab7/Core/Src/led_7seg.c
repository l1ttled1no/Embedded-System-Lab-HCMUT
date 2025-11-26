/*
 * led_7seg.c
 *
 *  Created on: Sep 25, 2023
 *      Author: HaHuyen
 */
#include <led_7seg.h>

uint8_t led7seg[4] = {0, 1, 2, 3}; //4 index
uint8_t arrayOfNum[10] = {0x03, 0x9f, 0x25, 0x0d, 0x99, 0x49, 0x41, 0x1f, 0x01, 0x09};// 9 numbers
uint16_t spi_buffer = 0xffff;

int led7_index = 0;

void led7_init(){
	  HAL_GPIO_WritePin(LD_LATCH_GPIO_Port, LD_LATCH_Pin, 1);
}

void led7_Scan(){
	spi_buffer &= 0x00ff;
	spi_buffer |= led7seg[led7_index] << 8;
	switch(led7_index){
	case 0:
		spi_buffer |= 0x00b0;
		spi_buffer &= 0xffbf;//1011
		break;
	case 1:
		spi_buffer |= 0x00d0;
		spi_buffer &= 0xffdf;//1101
		break;
	case 2:
		spi_buffer |= 0x00e0;
		spi_buffer &= 0xffef;//1110
		break;
	case 3:
		spi_buffer |= 0x0070;
		spi_buffer &= 0xff7f;//0111
		break;
	default:
		break;
	}
	led7_index = (led7_index + 1)%4;
	HAL_GPIO_WritePin(LD_LATCH_GPIO_Port, LD_LATCH_Pin, 0);
	HAL_SPI_Transmit(&hspi1, (void*)&spi_buffer, 2, 1);
	HAL_GPIO_WritePin(LD_LATCH_GPIO_Port, LD_LATCH_Pin, 1);
}

void led7_SetDigit(int num, int position, uint8_t show_dot){
	if(num >= 0 && num <= 9){
		led7seg[position] = arrayOfNum[num] - show_dot;
	}
}

void led7_SetColon(uint8_t status){
	if(status == 1) spi_buffer &= ~(1 << 3);
	else spi_buffer |= (1 << 3);
}

void led_On(uint8_t index){
	if(index >= 6 && index <=8){
		spi_buffer |= 1 << (index-6);
	}
}

void led_Off(uint8_t index){
	if(index >= 6 && index <=8){
		spi_buffer &= ~(1 << (index-6));
	}
}


