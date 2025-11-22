/*
 * sensor.c
 *
 *  Created on: Sep 26, 2023
 *      Author: HaHuyen
 */
#include "sensor.h"
//#include "stdbool.h"
#include "global.h"



uint16_t adc_receive[5];

void sensor_init(){
	HAL_ADC_Start_DMA(&hadc1, (uint16_t*)adc_receive, 5);
}

uint8_t sensor_Read(){
	if (HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_receive, 5) == HAL_OK)
		return 1;
	else return 0; //
}

uint16_t sensor_GetLight(){
	glb_Light = adc_receive[2];
	return adc_receive[2];
}

uint16_t sensor_GetPotentiometer(){
	glb_Potentiometer = adc_receive[3];
	glb_pct_Potentiometer = ((float)glb_Potentiometer/4095)*100;
	return adc_receive[3];
}

float sensor_GetVoltage(){
	glb_Voltage = ((float)adc_receive[0]*3.3*12)/(4095*1.565);
	return ((float)adc_receive[0]*3.3*12)/(4095*1.565);
}

float sensor_GetCurrent(){
	glb_Current = (((float)adc_receive[1]*3.3*1000)/(4095*0.647)-2.5)*5/2.5;
	return (((float)adc_receive[1]*3.3*1000)/(4095*0.647)-2.5)*5/2.5;
}

float sensor_GetTemperature(){
	glb_Temperature = ((float)adc_receive[4]*330)/(4095);
	return ((float)adc_receive[4]*330)/(4095);
}


