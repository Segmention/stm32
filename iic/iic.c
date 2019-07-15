#include "iic.h"
/*
	IIC_SDA  	--->PE15  
	IIC_SCL		--->PD9
*/

/*
 * ��  ��: IIC_GPIO ��ʼ��
 * ��  ��: ��
 * ����ֵ: ��
 */
void IIC_GPIO_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	
	//1 ���� GPIOB ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOD, ENABLE);
	
	//2 �����Լ���ʼ�� GPIO ģʽ
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_OUT;					//���
	GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;					//����
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;   				//����
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_25MHz;				//���
	
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_9;		//����
	GPIO_Init(GPIOD, &GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_15;		//����
	GPIO_Init(GPIOE, &GPIO_InitStruct);
}

/*
 * ��  ��: IIC_SDA �������ģʽ
 * ��  ��: ��
 * ����ֵ: ��
 */
void IIC_SDAModeOut(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	
	//1 ���� GPIOB ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	
	//2 �����Լ���ʼ�� GPIO ģʽ
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_OUT;					//���
	GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;					//����
	GPIO_InitStruct.GPIO_Pin   =  GPIO_Pin_15;					//����
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;   				//����
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_25MHz;				//���
	GPIO_Init(GPIOE, &GPIO_InitStruct);

}

/*
 * ��  ��: IIC_SDA ��������ģʽ
 * ��  ��: ��
 * ����ֵ: ��
 */
void IIC_SDAModeIn(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	
	//1 ���� GPIOB ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	
	//2 �����Լ���ʼ�� GPIO ģʽ
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_IN;					//����
	GPIO_InitStruct.GPIO_Pin   =  GPIO_Pin_15;					//����
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_UP;   				//����
	GPIO_Init(GPIOE, &GPIO_InitStruct);

}


/*
 * ��  ��: IIC��ʼ�ź�:�� SCL Ϊ�ߵ�ƽʱ,SDA�ɸߵ�ƽ��ɵ͵�ƽ
 * ��  ��: ��
 * ����ֵ: ��
 */
void IIC_Sart(void)
{
	//1 ����Ϊ���ģʽ
	IIC_SDAModeOut();
	
	IIC_SDA_OUT(1);    //SDA = 1
	IIC_SCL_OUT(1);
	delay_us(5);
	
	IIC_SDA_OUT(0);    //SDA = 0
	delay_us(5);
	
	IIC_SCL_OUT(0);    
	delay_us(5);
}

/*
 * ��  ��: IICֹͣ�ź�:�� SCL Ϊ�ߵ�ƽʱ,SDA�ɵ͵�ƽ��ɸߵ�ƽ
 * ��  ��: ��
 * ����ֵ: ��
 */
void IIC_Stop(void)
{
	//1 ����Ϊ���ģʽ
	IIC_SDAModeOut();
	
	IIC_SDA_OUT(0);    	//SDA = 0
	IIC_SCL_OUT(0);
	delay_us(5);
	
	IIC_SCL_OUT(1);    	//SCL = 1
	delay_us(5);
	
	IIC_SDA_OUT(1);		//SDA = 1
}

/*
 * ��  ��: IIC��һ���ֽں��� : STM32 ��ȡ AT24C02 ���� 
 * ��  ��: ��
 * ����ֵ: ���ȡ��һ���ֽ�����
 */
uint8_t IIC_ReadByte(void)
{
	uint8_t i;
	uint8_t data = 0;  //0000 0000 
	
	//1 ����Ϊ����ģʽ
	IIC_SDAModeIn();
	
	IIC_SCL_OUT(0);    	//SCL = 0    --->AT24C02 ��ʱ��������׼��
	delay_us(5);
	
	for(i=0 ;i<8 ;i++)
	{
		//�� SCL ���߱�ʾ STM32 ��ʼ��ȡ����
		IIC_SCL_OUT(1);    	
		delay_us(5);
		
		//��ȡһλ����
		if( IIC_SDA_IN() )   //��ȡ���ߵ�ƽ
		{
			data |= (0x1<<(7-i));
		}else{
			data &= ~(0x1<<(7-i));
		}
		
		IIC_SCL_OUT(0);    	//SCL = 0    --->AT24C02 ��ʱ���н�����һλ����
		delay_us(5);	
	}
	
	return data;
}

/*
 * ��  ��: IICдһ���ֽں��� : STM32 д���ݸ� AT24C02
 * ��  ��: 
 * 		data	---����Ҫд���һ���ֽ�����
 * ����ֵ: ��
 */
void IIC_WriteByte(uint8_t data)    //10101011
{
	uint8_t i;
	
	//1 ����Ϊ���ģʽ
	IIC_SDAModeOut();
	
	IIC_SCL_OUT(0);    	//SCL = 0    --->STM32 ��������׼��
	delay_us(5);
	
	for(i=0 ;i<8 ;i++)
	{
		//��ʼ׼����  7-i λ����
		if( data & (0x1<<(7-i)) )  //10000000
		{
			IIC_SDA_OUT(1);
		}else{
			IIC_SDA_OUT(0);
		}
		
		IIC_SCL_OUT(1);    	//SCL = 1    --->AT24C02 ��ʼ��ȡ����
		delay_us(5);
		
		IIC_SCL_OUT(0);    	//SCL = 0    --->STM32 ������һλ���ݽ���
		delay_us(5);	
	}
}


/*
 * ��  ��: IIC�ȴ�Ӧ���� : STM32 д���ݸ� AT24C02  ��AT24C02 �� STM32 һ���ظ� 
 * ��  ��: ��
 * ����ֵ: ��ȡ����Ӧ���źţ�Ӧ��(0) ,��Ӧ��(1)
 */
uint8_t IIC_WaitAck(void)    
{
	uint8_t ack;
	
	//1 ����Ϊ����ģʽ
	IIC_SDAModeIn();
	
	IIC_SCL_OUT(0);    	//SCL = 0    --->AT24C02 ���� Ӧ���ź� ׼��
	delay_us(5);
	
	IIC_SCL_OUT(1);    	//SCL = 1    --->STM32 ��ʼʶ�� Ӧ���ź�
	delay_us(5);
	
	if( IIC_SDA_IN() )   //��ȡ���ߵ�ƽ
	{
		ack = 1;
	}else{
		ack = 0;
	}
	
	IIC_SCL_OUT(0);    	//SCL = 0   
	delay_us(5);
	
	
	return ack;
}


/*
 * ��  ��: IIC����Ӧ���� : STM32 ��ȡ AT24C02 ���� ��STM32 �� AT24C02 һ���ظ� 
 * ��  ��: 
 * 		ack		---��STM32 �� AT24C02 �Ļظ���Ӧ��(0) ,��Ӧ��(1)
 * ����ֵ: ��
 */
void IIC_Ack(uint8_t ack)    
{
	
	//1 ����Ϊ���ģʽ
	IIC_SDAModeOut();
	
	IIC_SCL_OUT(0);    	//SCL = 0    --->STM32 ��ʱ��������׼��
	delay_us(5);
	
	if(ack)   //��Ӧ��
	{
		IIC_SDA_OUT(1);
	}else{
		IIC_SDA_OUT(0);
	}
	
	IIC_SCL_OUT(1);    	//SCL = 1    --->AT24C02 ��ʱ��ʼʶ������
	delay_us(5);
	
	IIC_SCL_OUT(0);    	//SCL = 0
	delay_us(5);
}





















