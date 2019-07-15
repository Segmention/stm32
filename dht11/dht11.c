#include "dht11.h"
/*
 DHT11  	--->PG9  
 */

/*
 * 功  能: DHT11初始化
 * 参  数: 无
 * 返回值: 无
 */
void DHT11_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	
	//1 开启 GPIOG 时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
	
	//2 配置以及初始化 GPIO 模式
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_OUT;		//输出
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;		//推挽
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_9;		//引脚
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_UP;   	//上拉
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_25MHz;	//输出
	GPIO_Init(GPIOG, &GPIO_InitStruct);
}

/*
 * 功  能: DHT11_DQ 设置为输出模式
 * 参  数: 无
 * 返回值: 无
 */
void DHT11_ModeOut(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	
	//2 配置以及初始化 GPIO 模式
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_OUT;		//输出
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;		//推挽
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_9;		//引脚
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_UP;   	//上拉
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_25MHz;	//输出
	GPIO_Init(GPIOG, &GPIO_InitStruct);
}

/*
 * 功  能: DHT11_DQ 设置为输入模式
 * 参  数: 无
 * 返回值: 无 

 */
void DHT11_ModeIn(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;

	//2 配置以及初始化 GPIO 模式
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_IN;		//输出
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_9;		//引脚
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_UP;   	//上拉
	GPIO_Init(GPIOG, &GPIO_InitStruct);
}

/*
 * 功  能: DHT11读取读取一个字节数据函数
 * 参  数: 无
 * 返回值: 成功读取的一个字节数据
 */
uint8_t DHT11_ReadByte(void)   //10011110
{
	uint8_t data = 0;   //0000 0000
	//uint8_t data;  //xxxx xxxx
	uint8_t i;
	
	//循环8次,每次到来的都是一位数据,我们需要去检测是0还是1
	for(i=0 ;i<8 ;i++)
	{
		//等待每一位数据前面的50us低电平结束
		while( DHT11_IN() == 0 );
		
		//满足这个条件说明高电平到来
		if( DHT11_IN() == 1 )
		{
			delay_us(40);
			 
			if( DHT11_IN() == 1 )    //等待 40us 之后再次判断是不是高电平
			{
				data |= 0x1<<(7-i);   //i = 0  --->data == 1000 0000
			}else{
				data &= ~(0x1<<(7-i));
			}
			
			//等待剩余的高电平结束
			while(DHT11_IN() == 1);
		}
	}
	
	return data;
}




/*
 * 功  能: DHT11读取数据函数
 * 参  数: 
 * 		DataBuf		--->用来保存读取的温湿度数据
 * 返回值: 
 * 		失败	--->-1
 * 		成功	--->0
 */
int DHT11_ReadData(uint8_t DataBuf[])
{
	int ret = -1;
	uint8_t cnt = 0;
	
	//1 STM32发送起始信号给DHT11
	DHT11_ModeOut(); 	//设置DQ引脚为输出模式
	DHT11_DQ(1);
	delay_us(30);
	DHT11_DQ(0);
	delay_ms(20);    	//拉低至少18ms
	DHT11_DQ(1);
	delay_us(30);		//拉高至少30us
	
	//2 DHT11给STM32发送回复信号
	DHT11_ModeIn();		//设置DQ引脚为输入模式
	
	while( DHT11_IN() == 0 )	//等待 80us 低电平结束
	{
		delay_us(1);
		cnt++;
		if(cnt > 100)		//超时判断
			ret = -1;
	}
	
	cnt = 0;
	while( DHT11_IN() == 1 )	//等待 80us 高电平结束
	{
		delay_us(1);
		cnt++;
		if(cnt > 100)		//超时判断
			ret = -1;
	}
	
	//3 DHT11给STM32发送40bit数据
	for(cnt=0;cnt<5;cnt++)
	{
		DataBuf[cnt] = DHT11_ReadByte();
	}

	//4 STM32检测读取的数据是否正常
	if( DataBuf[4] == (DataBuf[0]+DataBuf[1]+DataBuf[2]+DataBuf[3]) )
	{
		ret = 0;
	}
	
	delay_ms(1000);   //注意:采样周期间隔不得低于1秒钟
	delay_ms(300);   //注意:采样周期间隔不得低于1秒钟
	return ret;
}



























