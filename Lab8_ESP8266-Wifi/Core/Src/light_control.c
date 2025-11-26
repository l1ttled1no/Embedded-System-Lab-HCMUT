/*
 * light_control.c
 *
 *  Created on: Nov 15, 2023
 *      Author: phamv
 */
#include "light_control.h"

uint8_t light_status = 0;
uint16_t temp_counter = 0;

void lightProcess()
{
	if (button_count[13] == 1)
	{
		light_status = 1 - light_status;
		if (light_status == 1)
		{
			uart_EspSendBytes("A", 1);
		}
		else
		{
			uart_EspSendBytes("a", 1);
		}
	}
	if (light_status == 1)
	{
		HAL_GPIO_WritePin(OUTPUT_Y0_GPIO_Port, OUTPUT_Y0_Pin, 1);
	}
	else
	{
		HAL_GPIO_WritePin(OUTPUT_Y0_GPIO_Port, OUTPUT_Y0_Pin, 0);
	}
}

void test_Esp()
{
	if (uart_EspCheck() == 0)
		uart_EspSendBytes("o", 1);
	else
		lcd_ShowStr(10, 50, "ESP Connect", GREEN, BLACK, 24, 0);
}

void temperatureProcess()
{
	temp_counter++;

	// Display temperature every 1 second (20 * 50ms = 1000ms = 1s)
	if (temp_counter % 20 == 0)
	{
		// Read temperature from sensor
		float temperature = sensor_GetTemperature();

		// Convert float to integer and decimal parts (avoid %.1f formatting issue)
		int temp_int = (int)temperature;
		int temp_dec = (int)((temperature - temp_int) * 10);

		// Display temperature on LCD
		uint8_t display_msg[40];
		sprintf((char *)display_msg, "Temp: %d.%d C", temp_int, temp_dec);
		lcd_ShowStr(10, 80, (char *)display_msg, YELLOW, BLACK, 24, 0);
	}

	// Send to ESP every 30 seconds (600 * 50ms = 30000ms = 30s)
	if (temp_counter >= 600)
	{
		temp_counter = 0;

		// Read temperature from sensor
		float temperature = sensor_GetTemperature();

		// Convert float to integer and decimal parts
		int temp_int = (int)temperature;
		int temp_dec = (int)((temperature - temp_int) * 10);

		// Create message in format: !TEMP:<temperature>#
		uint8_t temp_msg[20];
		sprintf((char *)temp_msg, "!TEMP:%d.%d#", temp_int, temp_dec);

		// Send to ESP
		uart_EspSendBytes(temp_msg, strlen((char *)temp_msg));
	}
}
