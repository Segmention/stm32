#include "iic.h"
/*
	IIC_SDA  	--->PE15  
	IIC_SCL		--->PD9
*/

/*
 * 功  能: IIC_GPIO 初始化
 * 参  数: 无
 * 返回值: 无
 */
void IIC_GPIO_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	
	//1 开启 GPIOB 时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOD, ENABLE);
	
	//2 配置以及初始化 GPIO 模式
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_OUT;					//输出
	GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;					//推挽
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;   				//上拉
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_25MHz;				//输出
	
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_9;		//引脚
	GPIO_Init(GPIOD, &GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_15;		//引脚
	GPIO_Init(GPIOE, &GPIO_InitStruct);
}

/*
 * 功  能: IIC_SDA 设置输出模式
 * 参  数: 无
 * 返回值: 无
 */
void IIC_SDAModeOut(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	
	//1 开启 GPIOB 时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	
	//2 配置以及初始化 GPIO 模式
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_OUT;					//输出
	GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;					//推挽
	GPIO_InitStruct.GPIO_Pin   =  GPIO_Pin_15;					//引脚
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;   				//上拉
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_25MHz;				//输出
	GPIO_Init(GPIOE, &GPIO_InitStruct);

}

/*
 * 功  能: IIC_SDA 设置输入模式
 * 参  数: 无
 * 返回值: 无
 */
void IIC_SDAModeIn(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	
	//1 开启 GPIOB 时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	
	//2 配置以及初始化 GPIO 模式
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_IN;					//输入
	GPIO_InitStruct.GPIO_Pin   =  GPIO_Pin_15;					//引脚
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_UP;   				//上拉
	GPIO_Init(GPIOE, &GPIO_InitStruct);

}


/*
 * 功  能: IIC起始信号:当 SCL 为高电平时,SDA由高电平变成低电平
 * 参  数: 无
 * 返回值: 无
 */
void IIC_Sart(void)
{
	//1 设置为输出模式
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
 * 功  能: IIC停止信号:当 SCL 为高电平时,SDA由低电平变成高电平
 * 参  数: 无
 * 返回值: 无
 */
void IIC_Stop(void)
{
	//1 设置为输出模式
	IIC_SDAModeOut();
	
	IIC_SDA_OUT(0);    	//SDA = 0
	IIC_SCL_OUT(0);
	delay_us(5);
	
	IIC_SCL_OUT(1);    	//SCL = 1
	delay_us(5);
	
	IIC_SDA_OUT(1);		//SDA = 1
}

/*
 * 功  能: IIC读一个字节函数 : STM32 读取 AT24C02 数据 
 * 参  数: 无
 * 返回值: 你读取的一个字节数据
 */
uint8_t IIC_ReadByte(void)
{
	uint8_t i;
	uint8_t data = 0;  //0000 0000 
	
	//1 设置为输入模式
	IIC_SDAModeIn();
	
	IIC_SCL_OUT(0);    	//SCL = 0    --->AT24C02 此时进行数据准备
	delay_us(5);
	
	for(i=0 ;i<8 ;i++)
	{
		//将 SCL 拉高表示 STM32 开始读取数据
		IIC_SCL_OUT(1);    	
		delay_us(5);
		
		//读取一位数据
		if( IIC_SDA_IN() )   //读取到高电平
		{
			data |= (0x1<<(7-i));
		}else{
			data &= ~(0x1<<(7-i));
		}
		
		IIC_SCL_OUT(0);    	//SCL = 0    --->AT24C02 此时进行交换下一位数据
		delay_us(5);	
	}
	
	return data;
}

/*
 * 功  能: IIC写一个字节函数 : STM32 写数据给 AT24C02
 * 参  数: 
 * 		data	---》你要写入的一个字节数据
 * 返回值: 无
 */
void IIC_WriteByte(uint8_t data)    //10101011
{
	uint8_t i;
	
	//1 设置为输出模式
	IIC_SDAModeOut();
	
	IIC_SCL_OUT(0);    	//SCL = 0    --->STM32 进行数据准备
	delay_us(5);
	
	for(i=0 ;i<8 ;i++)
	{
		//开始准备第  7-i 位数据
		if( data & (0x1<<(7-i)) )  //10000000
		{
			IIC_SDA_OUT(1);
		}else{
			IIC_SDA_OUT(0);
		}
		
		IIC_SCL_OUT(1);    	//SCL = 1    --->AT24C02 开始读取数据
		delay_us(5);
		
		IIC_SCL_OUT(0);    	//SCL = 0    --->STM32 进行下一位数据交换
		delay_us(5);	
	}
}


/*
 * 功  能: IIC等待应答函数 : STM32 写数据给 AT24C02  ，AT24C02 给 STM32 一个回复 
 * 参  数: 无
 * 返回值: 读取到的应答信号：应答(0) ,不应答(1)
 */
uint8_t IIC_WaitAck(void)    
{
	uint8_t ack;
	
	//1 设置为输入模式
	IIC_SDAModeIn();
	
	IIC_SCL_OUT(0);    	//SCL = 0    --->AT24C02 进行 应答信号 准备
	delay_us(5);
	
	IIC_SCL_OUT(1);    	//SCL = 1    --->STM32 开始识别 应答信号
	delay_us(5);
	
	if( IIC_SDA_IN() )   //读取到高电平
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
 * 功  能: IIC主动应答函数 : STM32 读取 AT24C02 数据 ，STM32 给 AT24C02 一个回复 
 * 参  数: 
 * 		ack		---》STM32 给 AT24C02 的回复：应答(0) ,不应答(1)
 * 返回值: 无
 */
void IIC_Ack(uint8_t ack)    
{
	
	//1 设置为输出模式
	IIC_SDAModeOut();
	
	IIC_SCL_OUT(0);    	//SCL = 0    --->STM32 此时进行数据准备
	delay_us(5);
	
	if(ack)   //不应答
	{
		IIC_SDA_OUT(1);
	}else{
		IIC_SDA_OUT(0);
	}
	
	IIC_SCL_OUT(1);    	//SCL = 1    --->AT24C02 此时开始识别数据
	delay_us(5);
	
	IIC_SCL_OUT(0);    	//SCL = 0
	delay_us(5);
}





















