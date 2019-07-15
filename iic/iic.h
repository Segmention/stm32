#ifndef _IIC_H 
#define _IIC_H

//ͷ�ļ�
#include "stm32f4xx.h"
#include "systick.h"

//�궨��

#define IIC_SCL_OUT(a);		if(a) 	\
						GPIO_SetBits(GPIOD, GPIO_Pin_9); \
					else 	\
						GPIO_ResetBits(GPIOD, GPIO_Pin_9);	
#define IIC_SDA_OUT(a);		if(a) 	\
						GPIO_SetBits(GPIOE, GPIO_Pin_15); \
					else 	\
						GPIO_ResetBits(GPIOE, GPIO_Pin_15);	
	
#define IIC_SDA_IN()		GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_15)
					
//��������
/*
 * ��  ��: IIC_GPIO ��ʼ��
 * ��  ��: ��
 * ����ֵ: ��
 */
void IIC_GPIO_Init(void);
/*
 * ��  ��: IIC_SDA �������ģʽ
 * ��  ��: ��
 * ����ֵ: ��
 */
void IIC_SDAModeOut(void);
/*
 * ��  ��: IIC_SDA ��������ģʽ
 * ��  ��: ��
 * ����ֵ: ��
 */
void IIC_SDAModeIn(void);
/*
 * ��  ��: IIC��ʼ�ź�:�� SCL Ϊ�ߵ�ƽʱ,SDA�ɸߵ�ƽ��ɵ͵�ƽ
 * ��  ��: ��
 * ����ֵ: ��
 */
void IIC_Sart(void);
/*
 * ��  ��: IICֹͣ�ź�:�� SCL Ϊ�ߵ�ƽʱ,SDA�ɵ͵�ƽ��ɸߵ�ƽ
 * ��  ��: ��
 * ����ֵ: ��
 */
void IIC_Stop(void);
/*
 * ��  ��: IIC��һ���ֽں��� : STM32 ��ȡ AT24C02 ���� 
 * ��  ��: ��
 * ����ֵ: ���ȡ��һ���ֽ�����
 */
uint8_t IIC_ReadByte(void);
/*
 * ��  ��: IICдһ���ֽں��� : STM32 д���ݸ� AT24C02
 * ��  ��: 
 * 		data	---����Ҫд���һ���ֽ�����
 * ����ֵ: ��
 */
void IIC_WriteByte(uint8_t data) ;
/*
 * ��  ��: IIC�ȴ�Ӧ���� : STM32 д���ݸ� AT24C02  ��AT24C02 �� STM32 һ���ظ� 
 * ��  ��: ��
 * ����ֵ: ��ȡ����Ӧ���źţ�Ӧ��(0) ,��Ӧ��(1)
 */
uint8_t IIC_WaitAck(void);
/*
 * ��  ��: IIC����Ӧ���� : STM32 ��ȡ AT24C02 ���� ��STM32 �� AT24C02 һ���ظ� 
 * ��  ��: 
 * 		ack		---��STM32 �� AT24C02 �Ļظ���Ӧ��(0) ,��Ӧ��(1)
 * ����ֵ: ��
 */
void IIC_Ack(uint8_t ack);



#endif
