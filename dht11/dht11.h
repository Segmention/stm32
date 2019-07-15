#ifndef _DHT11_H 
#define _DHT11_H

//ͷ�ļ�
#include "stm32f4xx.h"
#include "systick.h"

//�궨��
#define DHT11_DQ(a);		if(a) 	\
								GPIO_SetBits(GPIOG, GPIO_Pin_9); \
							else 	\
								GPIO_ResetBits(GPIOG, GPIO_Pin_9);	

#define DHT11_IN()			GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_9)
							
							
							
//��������
/*
 * ��  ��: DHT11��ʼ��
 * ��  ��: ��
 * ����ֵ: ��
 */
void DHT11_Init(void);
/*
 * ��  ��
 * 		DataBuf		--->���������ȡ����ʪ������
 * ����ֵ: 
 * 		ʧ��	---: DHT11��ȡ���ݺ���
 * ��  ��: >-1
 * 		�ɹ�	--->0
 */
int DHT11_ReadData(uint8_t DataBuf[]);

#endif
