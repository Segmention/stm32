#include "dht11.h"
/*
 DHT11  	--->PG9  
 */

/*
 * ��  ��: DHT11��ʼ��
 * ��  ��: ��
 * ����ֵ: ��
 */
void DHT11_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	
	//1 ���� GPIOG ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
	
	//2 �����Լ���ʼ�� GPIO ģʽ
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_OUT;		//���
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;		//����
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_9;		//����
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_UP;   	//����
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_25MHz;	//���
	GPIO_Init(GPIOG, &GPIO_InitStruct);
}

/*
 * ��  ��: DHT11_DQ ����Ϊ���ģʽ
 * ��  ��: ��
 * ����ֵ: ��
 */
void DHT11_ModeOut(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	
	//2 �����Լ���ʼ�� GPIO ģʽ
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_OUT;		//���
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;		//����
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_9;		//����
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_UP;   	//����
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_25MHz;	//���
	GPIO_Init(GPIOG, &GPIO_InitStruct);
}

/*
 * ��  ��: DHT11_DQ ����Ϊ����ģʽ
 * ��  ��: ��
 * ����ֵ: �� 

 */
void DHT11_ModeIn(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;

	//2 �����Լ���ʼ�� GPIO ģʽ
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_IN;		//���
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_9;		//����
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_UP;   	//����
	GPIO_Init(GPIOG, &GPIO_InitStruct);
}

/*
 * ��  ��: DHT11��ȡ��ȡһ���ֽ����ݺ���
 * ��  ��: ��
 * ����ֵ: �ɹ���ȡ��һ���ֽ�����
 */
uint8_t DHT11_ReadByte(void)   //10011110
{
	uint8_t data = 0;   //0000 0000
	//uint8_t data;  //xxxx xxxx
	uint8_t i;
	
	//ѭ��8��,ÿ�ε����Ķ���һλ����,������Ҫȥ�����0����1
	for(i=0 ;i<8 ;i++)
	{
		//�ȴ�ÿһλ����ǰ���50us�͵�ƽ����
		while( DHT11_IN() == 0 );
		
		//�����������˵���ߵ�ƽ����
		if( DHT11_IN() == 1 )
		{
			delay_us(40);
			 
			if( DHT11_IN() == 1 )    //�ȴ� 40us ֮���ٴ��ж��ǲ��Ǹߵ�ƽ
			{
				data |= 0x1<<(7-i);   //i = 0  --->data == 1000 0000
			}else{
				data &= ~(0x1<<(7-i));
			}
			
			//�ȴ�ʣ��ĸߵ�ƽ����
			while(DHT11_IN() == 1);
		}
	}
	
	return data;
}




/*
 * ��  ��: DHT11��ȡ���ݺ���
 * ��  ��: 
 * 		DataBuf		--->���������ȡ����ʪ������
 * ����ֵ: 
 * 		ʧ��	--->-1
 * 		�ɹ�	--->0
 */
int DHT11_ReadData(uint8_t DataBuf[])
{
	int ret = -1;
	uint8_t cnt = 0;
	
	//1 STM32������ʼ�źŸ�DHT11
	DHT11_ModeOut(); 	//����DQ����Ϊ���ģʽ
	DHT11_DQ(1);
	delay_us(30);
	DHT11_DQ(0);
	delay_ms(20);    	//��������18ms
	DHT11_DQ(1);
	delay_us(30);		//��������30us
	
	//2 DHT11��STM32���ͻظ��ź�
	DHT11_ModeIn();		//����DQ����Ϊ����ģʽ
	
	while( DHT11_IN() == 0 )	//�ȴ� 80us �͵�ƽ����
	{
		delay_us(1);
		cnt++;
		if(cnt > 100)		//��ʱ�ж�
			ret = -1;
	}
	
	cnt = 0;
	while( DHT11_IN() == 1 )	//�ȴ� 80us �ߵ�ƽ����
	{
		delay_us(1);
		cnt++;
		if(cnt > 100)		//��ʱ�ж�
			ret = -1;
	}
	
	//3 DHT11��STM32����40bit����
	for(cnt=0;cnt<5;cnt++)
	{
		DataBuf[cnt] = DHT11_ReadByte();
	}

	//4 STM32����ȡ�������Ƿ�����
	if( DataBuf[4] == (DataBuf[0]+DataBuf[1]+DataBuf[2]+DataBuf[3]) )
	{
		ret = 0;
	}
	
	delay_ms(1000);   //ע��:�������ڼ�����õ���1����
	delay_ms(300);   //ע��:�������ڼ�����õ���1����
	return ret;
}



























