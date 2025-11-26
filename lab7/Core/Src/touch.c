/*
 * touch.c
 *
 *  Created on: Sep 22, 2023
 *      Author: phamv
 */

#include "touch.h"
#include "lcd.h"
#include "at24c.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

static void TP_Write_Byte(uint8_t num);
static uint16_t TP_Read_AD(uint8_t CMD);
static uint16_t TP_Read_XOY(uint8_t xy);
static uint8_t TP_Read_XY(uint16_t *x,uint16_t *y);
static uint8_t TP_Read_XY2(uint16_t *x,uint16_t *y);
static void TP_Drow_Touch_Point(uint16_t x,uint16_t y,uint16_t color);
static void TP_Draw_Big_Point(uint16_t x,uint16_t y,uint16_t color);
void LCD_DrawRoughLine(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t color);
static uint8_t TP_Scan(uint8_t tp);

_m_tp_dev tp_dev=
{
	touch_init,
	TP_Scan,
	touch_Adjust,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
};

#if (DFT_SCAN_DIR>>4)%4
uint8_t CMD_RDX=0X90;
uint8_t CMD_RDY=0XD0;
#else
uint8_t CMD_RDX=0XD0;
uint8_t CMD_RDY=0X90;
#endif

static void TP_Write_Byte(uint8_t num)
{
	uint8_t count=0;
	for(count=0;count<8;count++)
	{
		if(num&0x80) HAL_GPIO_WritePin(T_MOSI_GPIO_Port, T_MOSI_Pin, 1);
		else HAL_GPIO_WritePin(T_MOSI_GPIO_Port, T_MOSI_Pin, 0);
		num<<=1;
		HAL_GPIO_WritePin(T_CLK_GPIO_Port, T_CLK_Pin, 0);
		delay_us(1);
		HAL_GPIO_WritePin(T_CLK_GPIO_Port, T_CLK_Pin, 1);
	}
}

static uint16_t TP_Read_AD(uint8_t CMD)
{
	uint8_t count=0;
	uint16_t Num=0;
	HAL_GPIO_WritePin(T_CLK_GPIO_Port, T_CLK_Pin, 0);
	HAL_GPIO_WritePin(T_MOSI_GPIO_Port, T_MOSI_Pin, 0);
	HAL_GPIO_WritePin(T_CS_GPIO_Port, T_CS_Pin, 0);
	TP_Write_Byte(CMD);
	delay_us(6);
	HAL_GPIO_WritePin(T_CLK_GPIO_Port, T_CLK_Pin, 0);
	delay_us(1);
	HAL_GPIO_WritePin(T_CLK_GPIO_Port, T_CLK_Pin, 1);
	delay_us(1);
	HAL_GPIO_WritePin(T_CLK_GPIO_Port, T_CLK_Pin, 0);
	for(count=0;count<16;count++)
	{
		Num<<=1;
		HAL_GPIO_WritePin(T_CLK_GPIO_Port, T_CLK_Pin, 0);
		delay_us(1);
		HAL_GPIO_WritePin(T_CLK_GPIO_Port, T_CLK_Pin, 1);
 		if(HAL_GPIO_ReadPin(T_MISO_GPIO_Port, T_MISO_Pin) != 0)Num++;
	}
	Num>>=4;
	HAL_GPIO_WritePin(T_CS_GPIO_Port, T_CS_Pin, 1);
	return(Num);
}

#define READ_TIMES 5
#define LOST_VAL 1
static uint16_t TP_Read_XOY(uint8_t xy)
{
	uint16_t i, j;
	uint16_t buf[READ_TIMES];
	uint16_t sum=0;
	uint16_t temp;
	for(i=0;i<READ_TIMES;i++)buf[i]=TP_Read_AD(xy);
	for(i=0;i<READ_TIMES-1; i++)
	{
		for(j=i+1;j<READ_TIMES;j++)
		{
			if(buf[i]>buf[j])
			{
				temp=buf[i];
				buf[i]=buf[j];
				buf[j]=temp;
			}
		}
	}
	sum=0;
	for(i=LOST_VAL;i<READ_TIMES-LOST_VAL;i++)sum+=buf[i];
	temp=sum/(READ_TIMES-2*LOST_VAL);
	return temp;
}

static uint8_t TP_Read_XY(uint16_t *x,uint16_t *y)
{
	uint16_t xtemp,ytemp;
	xtemp=TP_Read_XOY(CMD_RDX);
	ytemp=TP_Read_XOY(CMD_RDY);
	*x=xtemp;
	*y=ytemp;
	return 1;
}

#define ERR_RANGE 100
static uint8_t TP_Read_XY2(uint16_t *x,uint16_t *y)
{
	uint16_t x1,y1;
 	uint16_t x2,y2;
 	uint8_t flag;
	flag=TP_Read_XY(&x1,&y1);
	if(flag==0)return(0);
	flag=TP_Read_XY(&x2,&y2);
	if(flag==0)return(0);
	if(((x2<=x1&&x1<x2+ERR_RANGE)||(x1<=x2&&x2<x1+ERR_RANGE))//前后两次采样在+-50内
	&&((y2<=y1&&y1<y2+ERR_RANGE)||(y1<=y2&&y2<y1+ERR_RANGE)))
	{
		*x=(x1+x2)/2;
		*y=(y1+y2)/2;
		return 1;
	}else return 0;
}

static void TP_Drow_Touch_Point(uint16_t x,uint16_t y,uint16_t color)
{
	lcd_DrawLine(x-12,y,x+13,y,color);
	lcd_DrawLine(x,y-12,x,y+13,color);
	lcd_DrawPoint(x+1,y+1,color);
	lcd_DrawPoint(x-1,y+1,color);
	lcd_DrawPoint(x+1,y-1,color);
	lcd_DrawPoint(x-1,y-1,color);
	lcd_DrawCircle(x,y,color,6, 0);
}

static void TP_Draw_Big_Point(uint16_t x,uint16_t y,uint16_t color)
{
	lcd_DrawPoint(x,y,color);
	lcd_DrawPoint(x+1,y,color);
	lcd_DrawPoint(x,y+1,color);
	lcd_DrawPoint(x+1,y+1,color);
}


void LCD_DrawRoughLine(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t color)
{
	uint16_t t;
	int xerr=0,yerr=0,delta_x,delta_y,distance;
	int incx,incy,uRow,uCol;
	delta_x=x2-x1;
	delta_y=y2-y1;
	uRow=x1;
	uCol=y1;
	if(delta_x>0)incx=1;
	else if (delta_x==0)incx=0;
	else {incx=-1;delta_x=-delta_x;}
	if(delta_y>0)incy=1;
	else if (delta_y==0)incy=0;
	else {incy=-1;delta_y=-delta_y;}
	if(delta_x>delta_y)distance=delta_x;
	else distance=delta_y;
	for(t=0;t<distance+1;t++)
	{
		TP_Draw_Big_Point(uRow,uCol,color);
		xerr+=delta_x;
		yerr+=delta_y;
		if(xerr>distance)
		{
			xerr-=distance;
			uRow+=incx;
		}
		if(yerr>distance)
		{
			yerr-=distance;
			uCol+=incy;
		}
	}
}

static uint8_t TP_Scan(uint8_t tp)
{
	if(HAL_GPIO_ReadPin(T_PEN_GPIO_Port, T_PEN_Pin) == GPIO_PIN_RESET)
	{
		if(tp)TP_Read_XY2(&tp_dev.x[0],&tp_dev.y[0]);
		else if(TP_Read_XY2(&tp_dev.x[0],&tp_dev.y[0]))
		{
	 		tp_dev.x[0]=tp_dev.xfac*tp_dev.x[0]+tp_dev.xoff;
			tp_dev.y[0]=tp_dev.yfac*tp_dev.y[0]+tp_dev.yoff;
	 	}
		if((tp_dev.sta&TP_PRES_DOWN)==0)
		{
			tp_dev.sta=TP_PRES_DOWN|TP_CATH_PRES;
			tp_dev.x[4]=tp_dev.x[0];
			tp_dev.y[4]=tp_dev.y[0];
		}
	}else
	{
		if(tp_dev.sta&TP_PRES_DOWN)
		{
			tp_dev.sta&=~(1<<7);
		}else
		{
			tp_dev.x[4]=0;
			tp_dev.y[4]=0;
			tp_dev.x[0]=0xffff;
			tp_dev.y[0]=0xffff;
		}
	}
	return tp_dev.sta&TP_PRES_DOWN;
}


#define SAVE_ADDR_BASE 0

static void TP_Save_Adjdata(void)
{
	at24c_Write(SAVE_ADDR_BASE,(uint8_t*)&tp_dev.xfac,14);
	at24c_WriteOneByte(SAVE_ADDR_BASE+14,DFT_SCAN_DIR);
}

static uint8_t TP_Get_Adjdata(void)
{
	uint8_t temp = 0;
	temp=at24c_ReadOneByte(SAVE_ADDR_BASE+14);

	if(temp==DFT_SCAN_DIR)
 	{
		at24c_Read(SAVE_ADDR_BASE,(uint8_t*)&tp_dev.xfac,14);
		return 1;
	}
	return 0;
}


void touch_Adjust(void)
{
	uint16_t pos_temp[4][2];
	uint8_t  cnt=0;
	uint16_t d1,d2;
	uint32_t tem1,tem2;
	double fac;
 	cnt=0;
 	lcd_Fill(0,0,lcddev.width,lcddev.height,WHITE);
 	lcd_ShowStr(5,40,"Please adjust the screen",RED,WHITE,16,0);
 	lcd_ShowStr(5,65,"by touching the marked",RED,WHITE,16,0);
 	lcd_ShowStr(5,90,"points one by one",RED,WHITE,16,0);
	TP_Drow_Touch_Point(20,20,RED);
	tp_dev.sta=0;
	tp_dev.xfac=0;
	while(1)
	{
		HAL_Delay(50);
		tp_dev.scan(1);
		if((tp_dev.sta&0xc0)==TP_CATH_PRES)
		{
			tp_dev.sta&=~(1<<6);

			pos_temp[cnt][0]=tp_dev.x[0];
			pos_temp[cnt][1]=tp_dev.y[0];
			cnt++;
			switch(cnt)
			{
				case 1:
					TP_Drow_Touch_Point(20,20,WHITE);
					TP_Drow_Touch_Point(lcddev.width-20,20,RED);
					break;
				case 2:
 					TP_Drow_Touch_Point(lcddev.width-20,20,WHITE);
					TP_Drow_Touch_Point(20,lcddev.height-20,RED);
					break;
				case 3:
 					TP_Drow_Touch_Point(20,lcddev.height-20,WHITE);
 					TP_Drow_Touch_Point(lcddev.width-20,lcddev.height-20,RED);
					break;
				case 4:
					tem1=abs(pos_temp[0][0]-pos_temp[1][0]);
					tem2=abs(pos_temp[0][1]-pos_temp[1][1]);
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);

					tem1=abs(pos_temp[2][0]-pos_temp[3][0]);
					tem2=abs(pos_temp[2][1]-pos_temp[3][1]);
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05||d1==0||d2==0)
					{
						cnt=0;
 				    TP_Drow_Touch_Point(lcddev.width-20,lcddev.height-20,WHITE);
   	 				TP_Drow_Touch_Point(20,20,RED);
						lcd_ShowStr(5,40,"Touch Adjust Failed!          ",RED,WHITE,16,0);
						lcd_ShowStr(5,60,"Please Adjust Again!         ",RED,WHITE,16,0);
						HAL_Delay(1000);
 						continue;
					}
					tem1=abs(pos_temp[0][0]-pos_temp[2][0]);
					tem2=abs(pos_temp[0][1]-pos_temp[2][1]);
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);

					tem1=abs(pos_temp[1][0]-pos_temp[3][0]);
					tem2=abs(pos_temp[1][1]-pos_temp[3][1]);
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05)
					{
						cnt=0;
 				    TP_Drow_Touch_Point(lcddev.width-20,lcddev.height-20,WHITE);
   	 				TP_Drow_Touch_Point(20,20,RED);
						lcd_ShowStr(5,40,"Touch Adjust Failed!          ",RED,WHITE,16,0);
						lcd_ShowStr(5,60,"Please Adjust Again!         ",RED,WHITE,16,0);
						HAL_Delay(1000);
						continue;
					}

					tem1=abs(pos_temp[1][0]-pos_temp[2][0]);
					tem2=abs(pos_temp[1][1]-pos_temp[2][1]);
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);

					tem1=abs(pos_temp[0][0]-pos_temp[3][0]);
					tem2=abs(pos_temp[0][1]-pos_temp[3][1]);
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05)
					{
						cnt=0;
 				    TP_Drow_Touch_Point(lcddev.width-20,lcddev.height-20,WHITE);
   	 				TP_Drow_Touch_Point(20,20,RED);
						lcd_ShowStr(5,40,"Touch Adjust Failed!          ",RED,WHITE,16,0);
						lcd_ShowStr(5,60,"Please Adjust Again!         ",RED,WHITE,16,0);

						HAL_Delay(1000);
 							continue;
					}
					tp_dev.xfac=(float)(lcddev.width-40)/(pos_temp[1][0]-pos_temp[0][0]);
					tp_dev.xoff=(lcddev.width-tp_dev.xfac*(pos_temp[1][0]+pos_temp[0][0]))/2;

					tp_dev.yfac=(float)(lcddev.height-40)/(pos_temp[2][1]-pos_temp[0][1]);
					tp_dev.yoff=(lcddev.height-tp_dev.yfac*(pos_temp[2][1]+pos_temp[0][1]))/2;

					TP_Drow_Touch_Point(lcddev.width-20,lcddev.height-20,WHITE);
					lcd_ShowStr(5,40,"Touch Screen Adjust OK!      ",RED,WHITE,16,0);
					lcd_ShowStr(5,60,"                             ",RED,WHITE,16,0);
					HAL_Delay(1000);
 					lcd_ShowStr(5,40,"                       ",RED,WHITE,16,0);
					TP_Save_Adjdata();
					TP_Get_Adjdata();
					return;
			}
		}
 	}
}

void touch_init(void)
{
	TP_Read_XY(&tp_dev.x[0],&tp_dev.y[0]);
	at24c_init();
	TP_Get_Adjdata();
}

void touch_Scan(){
	  tp_dev.scan(0);
}

uint8_t touch_IsTouched(){
	return HAL_GPIO_ReadPin(T_PEN_GPIO_Port, T_PEN_Pin) == 0;
}

uint16_t touch_GetX(){
	return tp_dev.x[0];
}

uint16_t touch_GetY(){
	return tp_dev.y[0];
}




