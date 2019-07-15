#include "mpu6050.h"
#include "math.h"
/*
 IIC_SDA  	--->PB9  
 IIC_SCL	--->PB8
*/

/*
 * ��  ��: IIC_GPIO ��ʼ��
 * ��  ��: ��
 * ����ֵ: ��
 */
void mpu6050_IIC_GPIO_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	
	//1 ���� GPIOB ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	//2 �����Լ���ʼ�� GPIO ģʽ
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_OUT;					//���
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;					//����
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_8 | GPIO_Pin_9;		//����
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_UP;   				//����
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_25MHz;				//���
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	//��ʼΪ�ߵ�ƽ
	GPIO_SetBits(GPIOB ,GPIO_Pin_8 | GPIO_Pin_9);
}

/*
 * ��  ��: IIC_SDA �������ģʽ
 * ��  ��: ��
 * ����ֵ: ��
 */
void mpu6050_IIC_SDAModeOut(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	
	//1 ���� GPIOB ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	//2 �����Լ���ʼ�� GPIO ģʽ
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_OUT;					//���
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;					//����
	GPIO_InitStruct.GPIO_Pin   =  GPIO_Pin_9;					//����
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_UP;   				//����
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_25MHz;				//���
	GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/*
 * ��  ��: IIC_SDA ��������ģʽ
 * ��  ��: ��
 * ����ֵ: ��
 */
void mpu6050_IIC_SDAModeIn(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	
	//1 ���� GPIOB ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	//2 �����Լ���ʼ�� GPIO ģʽ
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_IN;					//����
	GPIO_InitStruct.GPIO_Pin   =  GPIO_Pin_9;					//����
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_UP;   				//����
	GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/*
 * ��  ��: IIC��ʼ�ź�:�� SCL Ϊ�ߵ�ƽʱ,SDA�ɸߵ�ƽ��ɵ͵�ƽ
 * ��  ��: ��
 * ����ֵ: ��
 */
void mpu6050_IIC_Sart(void)
{
	//1 ����Ϊ���ģʽ
	mpu6050_IIC_SDAModeOut();
	
	mpu6050_IIC_SDA_OUT(1);    //SDA = 1
	mpu6050_IIC_SCL_OUT(1);
	delay_us(5);
	
	mpu6050_IIC_SDA_OUT(0);    //SDA = 0
	delay_us(5);
	
	mpu6050_IIC_SCL_OUT(0);    
	delay_us(5);
}

/*
 * ��  ��: IICֹͣ�ź�:�� SCL Ϊ�ߵ�ƽʱ,SDA�ɵ͵�ƽ��ɸߵ�ƽ
 * ��  ��: ��
 * ����ֵ: ��
 */
void mpu6050_IIC_Stop(void)
{
	//1 ����Ϊ���ģʽ
	mpu6050_IIC_SDAModeOut();
	
	mpu6050_IIC_SDA_OUT(0);    	//SDA = 0
	mpu6050_IIC_SCL_OUT(0);
	delay_us(5);
	
	mpu6050_IIC_SCL_OUT(1);    	//SCL = 1
	delay_us(5);
	
	mpu6050_IIC_SDA_OUT(1);		//SDA = 1
}




/*
 * ��  ��: IIC�ȴ�Ӧ���� : STM32 д���ݸ� mpu6050  ��mpu6050 �� STM32 һ���ظ� 
 * ��  ��: ��
 * ����ֵ: ��ȡ����Ӧ���źţ�Ӧ��(0) ,��Ӧ��(1)
 */
uint8_t mpu6050_IIC_WaitAck(void)    
{
	uint8_t ack;
	
	//1 ����Ϊ����ģʽ
	mpu6050_IIC_SDAModeIn();
	
	mpu6050_IIC_SCL_OUT(0);    	//SCL = 0    --->mpu6050 ���� Ӧ���ź� ׼��
	delay_us(5);
	
	mpu6050_IIC_SCL_OUT(1);    	//SCL = 1    --->STM32 ��ʼʶ�� Ӧ���ź�
	delay_us(5);
	
	if( mpu6050_IIC_SDA_IN() )   //��ȡ���ߵ�ƽ
	{
		ack = 1;
	}else{
		ack = 0;
	}
	
	mpu6050_IIC_SCL_OUT(0);    	//SCL = 0   
	delay_us(5);
	
	
	return ack;
}


/*
 * ��  ��: IIC����Ӧ���� : STM32 ��ȡ mpu6050 ���� ��STM32 �� mpu6050 һ���ظ� 
 * ��  ��: 
 * 		ack		---��STM32 �� mpu6050 �Ļظ���Ӧ��(0) ,��Ӧ��(1)
 * ����ֵ: ��
 */
void mpu6050_IIC_Ack(uint8_t ack)    
{
	
	//1 ����Ϊ���ģʽ
	mpu6050_IIC_SDAModeOut();
	
	mpu6050_IIC_SCL_OUT(0);    	//SCL = 0    --->STM32 ��ʱ��������׼��
	delay_us(5);
	
	if(ack)   //��Ӧ��
	{
		mpu6050_IIC_SDA_OUT(1);
	}else{
		mpu6050_IIC_SDA_OUT(0);
	}
	
	mpu6050_IIC_SCL_OUT(1);    	//SCL = 1    --->mpu6050 ��ʱ��ʼʶ������
	delay_us(5);
	
	mpu6050_IIC_SCL_OUT(0);    	//SCL = 0
	delay_us(5);
}

/*
 * ��  ��: IIC��һ���ֽں��� : STM32 ��ȡ mpu6050 ���� 
 * ��  ��: ��
 * ����ֵ: ���ȡ��һ���ֽ�����
 */
uint8_t mpu6050_IIC_ReadByte(void)
{
	uint8_t i;
	uint8_t data = 0;  //0000 0000 
	
	//1 ����Ϊ����ģʽ
	mpu6050_IIC_SDAModeIn();
	
	mpu6050_IIC_SCL_OUT(0);    	//SCL = 0    --->mpu6050 ��ʱ��������׼��
	delay_us(5);
	
	for(i=0 ;i<8 ;i++)
	{
		//�� SCL ���߱�ʾ STM32 ��ʼ��ȡ����
		mpu6050_IIC_SCL_OUT(1);    	
		delay_us(5);
		
		//��ȡһλ����
		if( mpu6050_IIC_SDA_IN() )   //��ȡ���ߵ�ƽ
		{
			data |= (0x1<<(7-i));
		}else{
			data &= ~(0x1<<(7-i));
		}
		
		mpu6050_IIC_SCL_OUT(0);    	//SCL = 0    --->mpu6050 ��ʱ���н�����һλ����
		delay_us(5);	
	}
	
	return data;
}

/*
 * ��  ��: IICдһ���ֽں��� : STM32 д���ݸ� mpu6050
 * ��  ��: 
 * 		data	---����Ҫд���һ���ֽ�����
 * ����ֵ: ��
 */
void mpu6050_IIC_WriteByte(uint8_t data)    //10101011
{
	uint8_t i;
	
	//1 ����Ϊ���ģʽ
	mpu6050_IIC_SDAModeOut();
	
	mpu6050_IIC_SCL_OUT(0);    	//SCL = 0    --->STM32 ��������׼��
	delay_us(5);
	
	for(i=0 ;i<8 ;i++)
	{
		//��ʼ׼����  7-i λ����
		if( data & (0x1<<(7-i)) )  //10000000
		{
			mpu6050_IIC_SDA_OUT(1);
		}else{
			mpu6050_IIC_SDA_OUT(0);
		}
		
		mpu6050_IIC_SCL_OUT(1);    	//SCL = 1    --->mpu6050 ��ʼ��ȡ����
		delay_us(5);
		
		mpu6050_IIC_SCL_OUT(0);    	//SCL = 0    --->STM32 ������һλ���ݽ���
		delay_us(5);	
	}
}


// ���������������ݶ�ȡ\USER\src\mpu6050.c 
/****************************************************************************** 
* �������ܣ� MPU6050 д�Ĵ������� 
* ��������� regAddr���Ĵ�����ַ regData����д��Ĵ���ֵ 
* ��������� �� 
* ����ֵ �� �� 
******************************************************************************/
void MPU6050_Write_Reg(uint8_t regAddr, uint8_t regData)
{
    /* ������ʼ�ź� */
    mpu6050_IIC_Sart();
    
    /* �����豸��ַ */        
    mpu6050_IIC_WriteByte(DEV_ADDR); 
    if (mpu6050_IIC_WaitAck())
        goto stop;
    
    /* ���ͼĴ�����ַ */
    mpu6050_IIC_WriteByte(regAddr);
    if (mpu6050_IIC_WaitAck())
        goto stop;
    
    /* д���ݵ��Ĵ��� */
    mpu6050_IIC_WriteByte(regData);
    if (mpu6050_IIC_WaitAck())
        goto stop;
    
stop:
    mpu6050_IIC_Stop();
}

// ���������������ݶ�ȡ\USER\src\mpu6050.c 
/****************************************************************************** 
* �������ܣ� MPU6050 ���Ĵ������� 
* ��������� regAddr���Ĵ�����ַ 
* ��������� �� 
* ����ֵ �� regData�������ļĴ������� 
******************************************************************************/
uint8_t MPU6050_Read_Reg(uint8_t regAddr)
{
    uint8_t regData;
    
    /* ������ʼ�ź� */
    mpu6050_IIC_Sart();
    
    /* �����豸��ַ */        
    mpu6050_IIC_WriteByte(DEV_ADDR);
    if (mpu6050_IIC_WaitAck())
        goto stop;
    
    /* ���ͼĴ�����ַ */
    mpu6050_IIC_WriteByte(regAddr);
    if (mpu6050_IIC_WaitAck())
        goto stop;
    
    /* �����ظ���ʼ�ź� */
    mpu6050_IIC_Sart();
    
    /* ���Ͷ�ģʽ�豸��ַ */	
    mpu6050_IIC_WriteByte(DEV_ADDR | 0x01);
    if (mpu6050_IIC_WaitAck())
        goto stop;
    
    /* ���Ĵ������� */
    regData = mpu6050_IIC_ReadByte();
    mpu6050_IIC_Ack(1); // ��Ӧ���ź�     
stop:
    mpu6050_IIC_Stop();
    
    return regData;
}

// ���������������ݶ�ȡ\USER\src\mpu6050.c 
/****************************************************************************** 
* �������ܣ� MPU6050 ��ʼ������ 
* ��������� �� 
* ��������� �� 
* ����ֵ �� �� 
* �� ע�� ���� MPU6050 ������Χ���� 2000 ��/s �� 2g 
******************************************************************************/
void MPU6050_Init(void)
{
    mpu6050_IIC_GPIO_Init();   // I2C ��ʼ��     

    MPU6050_Write_Reg(PWR_MGMT_1, 0x00);    //�������״̬     
    MPU6050_Write_Reg(SMPLRT_DIV, 0x07);    //�����ǲ����ʣ�����ֵ��0x07(125Hz)     
    MPU6050_Write_Reg(CONFIG, 0x06);        //��ͨ�˲�Ƶ�ʣ�����ֵ��0x06(5Hz)     
    MPU6050_Write_Reg(GYRO_CONFIG, 0x18);   //�������Լ켰������Χ������ֵ��0x18(���Լ죬2000deg/s)     
    MPU6050_Write_Reg(ACCEL_CONFIG, 0x01);  //���ټ��Լ졢������Χ����ͨ�˲�Ƶ�ʣ�����ֵ��0x01(���Լ죬2G��5Hz) 
}

// ���������������ݶ�ȡ\USER\src\mpu6050.c 
/****************************************************************************** 
* �������ܣ� �����������Ĵ������ϳ� 16 λ���� 
* ��������� regAddr�����ݵ�λ�Ĵ�����ַ 
* ��������� �� 
* ����ֵ �� data��2 ���Ĵ����ϳɵ� 16 λ���� 
******************************************************************************/
int16_t MPU6050_Get_Data(uint8_t regAddr)
{
    uint8_t Data_H, Data_L;
    uint16_t data;
    
    Data_H = MPU6050_Read_Reg(regAddr);
    Data_L = MPU6050_Read_Reg(regAddr + 1);
    data = (Data_H << 8) | Data_L;  // �ϳ����� 
    return data;
}

// �������ݶ�ȡ\USER\src\main.c 
/* ��������������ֵ������оƬ�̶�������Ӳ�����е����� */
#define X_ACCEL_OFFSET -600 
#define Y_ACCEL_OFFSET -100 
#define Z_ACCEL_OFFSET 2900 
#define X_GYRO_OFFSET 32 
#define Y_GYRO_OFFSET -11 
#define Z_GYRO_OFFSET 1 
/****************************************************************************** 
* �������ܣ� ���ڴ�ӡ 6050 ��������ȡ��������ٶȡ������Ǽ��¶����� 
* ��������� �� 
* ��������� �� 
* ����ֵ �� �� 
******************************************************************************/
void MPU6050_Display(void)
{
    /* ��ӡ x, y, z ����ٶ� */
    printf("ACCEL_X: %d\t", MPU6050_Get_Data(ACCEL_XOUT_H) + X_ACCEL_OFFSET);
    printf("ACCEL_Y: %d\t", MPU6050_Get_Data(ACCEL_YOUT_H) + Y_ACCEL_OFFSET);
    printf("ACCEL_Z: %d\t", MPU6050_Get_Data(ACCEL_ZOUT_H) + Z_ACCEL_OFFSET);
    
    /* ��ӡ�¶� */
    printf("TEMP: %0.2f\t", MPU6050_Get_Data(TEMP_OUT_H) / 340.0 + 36.53);
    
    /* ��ӡ x, y, z ����ٶ� */
    printf("GYRO_X: %d\t", MPU6050_Get_Data(GYRO_XOUT_H) + X_GYRO_OFFSET);
    printf("GYRO_Y: %d\t", MPU6050_Get_Data(GYRO_YOUT_H) + Y_GYRO_OFFSET);
    printf("GYRO_Z: %d\t", MPU6050_Get_Data(GYRO_ZOUT_H) + Z_GYRO_OFFSET);
    
    printf("\r\n");
}


/****************************************************************************** 
* �������ܣ� ���� x, y, z �������� 
* ��������� �� 
* ��������� data���ǶȽṹ�� 
* ����ֵ �� �� 
******************************************************************************/

void MPU6050_Get_Angle(MPU6050_Angle *data)
{   
    /* ����x, y, z ����ǣ����ػ���ֵ*/
    data->X_Angle = acos((MPU6050_Get_Data(ACCEL_XOUT_H) + X_ACCEL_OFFSET) / 16384.0);
    data->Y_Angle = acos((MPU6050_Get_Data(ACCEL_YOUT_H) + Y_ACCEL_OFFSET) / 16384.0);
    data->Z_Angle = acos((MPU6050_Get_Data(ACCEL_ZOUT_H) + Z_ACCEL_OFFSET) / 16384.0);

    /* ����ֵת��Ϊ�Ƕ�ֵ */
    data->X_Angle = data->X_Angle * 57.29577;
    data->Y_Angle = data->Y_Angle * 57.29577;
    data->Z_Angle = data->Z_Angle * 57.29577;
} 
