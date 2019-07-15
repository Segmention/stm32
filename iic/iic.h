#ifndef _IIC_H 
#define _IIC_H

//头文件
#include "stm32f4xx.h"
#include "systick.h"

//宏定义

#define IIC_SCL_OUT(a);		if(a) 	\
						GPIO_SetBits(GPIOD, GPIO_Pin_9); \
					else 	\
						GPIO_ResetBits(GPIOD, GPIO_Pin_9);	
#define IIC_SDA_OUT(a);		if(a) 	\
						GPIO_SetBits(GPIOE, GPIO_Pin_15); \
					else 	\
						GPIO_ResetBits(GPIOE, GPIO_Pin_15);	
	
#define IIC_SDA_IN()		GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_15)
					
//函数声明
/*
 * 功  能: IIC_GPIO 初始化
 * 参  数: 无
 * 返回值: 无
 */
void IIC_GPIO_Init(void);
/*
 * 功  能: IIC_SDA 设置输出模式
 * 参  数: 无
 * 返回值: 无
 */
void IIC_SDAModeOut(void);
/*
 * 功  能: IIC_SDA 设置输入模式
 * 参  数: 无
 * 返回值: 无
 */
void IIC_SDAModeIn(void);
/*
 * 功  能: IIC起始信号:当 SCL 为高电平时,SDA由高电平变成低电平
 * 参  数: 无
 * 返回值: 无
 */
void IIC_Sart(void);
/*
 * 功  能: IIC停止信号:当 SCL 为高电平时,SDA由低电平变成高电平
 * 参  数: 无
 * 返回值: 无
 */
void IIC_Stop(void);
/*
 * 功  能: IIC读一个字节函数 : STM32 读取 AT24C02 数据 
 * 参  数: 无
 * 返回值: 你读取的一个字节数据
 */
uint8_t IIC_ReadByte(void);
/*
 * 功  能: IIC写一个字节函数 : STM32 写数据给 AT24C02
 * 参  数: 
 * 		data	---》你要写入的一个字节数据
 * 返回值: 无
 */
void IIC_WriteByte(uint8_t data) ;
/*
 * 功  能: IIC等待应答函数 : STM32 写数据给 AT24C02  ，AT24C02 给 STM32 一个回复 
 * 参  数: 无
 * 返回值: 读取到的应答信号：应答(0) ,不应答(1)
 */
uint8_t IIC_WaitAck(void);
/*
 * 功  能: IIC主动应答函数 : STM32 读取 AT24C02 数据 ，STM32 给 AT24C02 一个回复 
 * 参  数: 
 * 		ack		---》STM32 给 AT24C02 的回复：应答(0) ,不应答(1)
 * 返回值: 无
 */
void IIC_Ack(uint8_t ack);



#endif
