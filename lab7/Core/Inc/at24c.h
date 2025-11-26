/*
 * at24c.h
 *
 *  Created on: Sep 22, 2023
 *      Author: phamv
 */

#ifndef INC_AT24C_H_
#define INC_AT24C_H_

#include "i2c.h"

#define AT24C01		127
#define AT24C02		255
#define AT24C04		511
#define AT24C08		1023
#define AT24C16		2047
#define AT24C32		4095
#define AT24C64		8191
#define AT24C128	16383
#define AT24C256	32767
#define AT24C512	65535


#define EE_TYPE AT24C512

uint8_t at24c_ReadOneByte(uint16_t ReadAddr);
void at24c_WriteOneByte(uint16_t WriteAddr,uint8_t DataToWrite);
void at24c_Write(uint16_t WriteAddr,uint8_t *pBuffer,uint16_t NumToWrite);
void at24c_Read(uint16_t ReadAddr,uint8_t *pBuffer,uint16_t NumToRead);
void at24c_init(void);

#endif /* INC_AT24C_H_ */
