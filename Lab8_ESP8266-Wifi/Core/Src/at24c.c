/*
 * at24c.c
 *
 *  Created on: Sep 22, 2023
 *      Author: phamv
 */

#include "at24c.h"


static uint8_t at24c_Check(void);

void at24c_init(void)
{
	at24c_Check();
}

uint8_t at24c_ReadOneByte(uint16_t ReadAddr)
{
	uint8_t temp=0;
    HAL_I2C_Mem_Read(&hi2c1, 0xA0, ReadAddr, I2C_MEMADD_SIZE_16BIT, &temp, 1, 10);
	return temp;
}

void at24c_WriteOneByte(uint16_t WriteAddr,uint8_t DataToWrite)
{
	HAL_I2C_Mem_Write(&hi2c1, 0xA0, WriteAddr, I2C_MEMADD_SIZE_16BIT, &DataToWrite, 1, 10);
	HAL_Delay(5);
}

uint8_t at24c_Check(void)
{
	uint8_t temp;
	temp=at24c_ReadOneByte(65535);
	if(temp==0X55)return 0;
	else
	{
		at24c_WriteOneByte(65535,0X55);
	    temp=at24c_ReadOneByte(65535);
		if(temp==0X55)return 0;
	}
	return 1;
}


void at24c_Read(uint16_t ReadAddr,uint8_t *pBuffer,uint16_t NumToRead)
{
	while(NumToRead)
	{
		*pBuffer++=at24c_ReadOneByte(ReadAddr++);
		NumToRead--;
	}
}

void at24c_Write(uint16_t WriteAddr,uint8_t *pBuffer,uint16_t NumToWrite)
{
	while(NumToWrite--)
	{
		at24c_WriteOneByte(WriteAddr,*pBuffer);
		WriteAddr++;
		pBuffer++;
	}
}
