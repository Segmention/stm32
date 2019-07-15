#ifndef _DHT11_H 
#define _DHT11_H

//头文件
#include "stm32f4xx.h"
#include "systick.h"

//宏定义
#define DHT11_DQ(a);		if(a) 	\
								GPIO_SetBits(GPIOG, GPIO_Pin_9); \
							else 	\
								GPIO_ResetBits(GPIOG, GPIO_Pin_9);	

#define DHT11_IN()			GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_9)
							
							
							
//函数声明
/*
 * 功  能: DHT11初始化
 * 参  数: 无
 * 返回值: 无
 */
void DHT11_Init(void);
/*
 * 功  能
 * 		DataBuf		--->用来保存读取的温湿度数据
 * 返回值: 
 * 		失败	---: DHT11读取数据函数
 * 参  数: >-1
 * 		成功	--->0
 */
int DHT11_ReadData(uint8_t DataBuf[]);

#endif
