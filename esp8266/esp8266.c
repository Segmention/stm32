#include "esp8266.h" 

unsigned int Wifi_lastcnt = 0;;
extern unsigned int Wifi_cnt;
extern unsigned char Wifi_receive[128];
extern uint8_t connect_out;

EdpPacket *send_pkg,*send_pkg1;	//协议包 

/*
 * ESP8266-01 引脚接线说明
 * ESP8266			STM32
 * GND				GND
 * TXD				USART_RXD
 * RXD 				USART_TXD
 * VCC				3.3V
 * CH_PD			3.3V	--->PC11 --->DCMI_D4
 * GPIO16			3.3V	--->PE5  --->DCMI_D6
 */
/*
 * 功  能：初始化 ESP8266
 * 参  数：无
 * 返回值：无
 */
void ESP8266_Init(void)
{
	ESP8266_GPIO_Init();
	ESP8266_UART_Init();
	
	ESP8266_AT_Init();
	UART2_SendString("ESP8266_Init Success!\r\n") ;
}



/*
 * 功  能：ESP8266初始化 AT 指令
 * 参  数：无
 * 返回值：无
 */
void ESP8266_AT_Init(void)  
{
	while(ESP8266_SendCmd((char *)AT ,(uint8_t *)answer1));
	delay_ms(500);
	
	while(ESP8266_SendCmd((char *)CWMODE ,(uint8_t *)answer1));
	delay_ms(500);

	while(ESP8266_SendCmd((char *)RST ,(uint8_t *)answer1));
	delay_ms(500);
	
	while(ESP8266_SendCmd((char *)CIFSR ,(uint8_t *)answer1));
	delay_ms(500);
	
	while(ESP8266_SendCmd((char *)CWJAP ,(uint8_t *)WIFICONNECT));
	delay_ms(500);
	
	while(ESP8266_SendCmd((char *)CIPSTART ,(uint8_t *)answer1));
	delay_ms(500);
	
	printf("WIFI Init OK\r\n");
}



/*
 * 功  能：ESP8266模块发送命令
 * 参  数：
 * 		cmd		--->命令
 * 		res		--->需要检查的返回指令
 * 返回值：
 * 		0		--->成功
 *  	1		--->失败
 */
uint8_t ESP8266_SendCmd(char *cmd ,uint8_t *res)  
{
	unsigned char timeout = 50;
	
	UART2_SendString(cmd);   //使用 串口2 发送 AT 指令
	printf("ESP8266_SendCmd:%s",cmd); //方便调试使用，通过串口1发送
	
	while(timeout--)
	{
		if(ESP8266_WaitRecive() == Wifi_OK)
		{
			printf("Wifi_receive:%s\r\n",Wifi_receive);
			
			if(strstr((const char *)Wifi_receive , (const char *)res) != NULL)
			{
				//检测到有关键字
				ESP8266_Clear();   //清空缓存
				return 0;
			}
			
			ESP8266_Clear();
		}
		
		delay_ms(10); 
	}
	
	return 1;
}

/*
 * 功  能：清除中断接收的字符串
 * 参  数：无
 * 返回值：无
 */
void ESP8266_Clear(void) 
{
	memset(Wifi_receive,0,sizeof(Wifi_receive));
	Wifi_cnt = 0;
}

/*
 * 功  能：等待接收完成
 * 参  数：无
 * 返回值：
 *		Wifi_OK		--->接收完成
 *		Wifi_wait	--->接收超时未完成
 * 说  明：循环用检测是否接收完成
 */
uint8_t ESP8266_WaitRecive(void) 
{
	if(Wifi_cnt == 0)	//如果接收计数为0 则说明没有处于接收数据中，所以直接跳出，结束函数
		return Wifi_wait;
	
	if(Wifi_cnt == Wifi_lastcnt)	//如果上一次的值和这次相同，则说明接收完毕
	{	
		Wifi_cnt = 0;				//清0接收计数
		return Wifi_OK;				//返回接收完成标志
	}
	
	Wifi_lastcnt = Wifi_cnt;		//置为相同
						
	return Wifi_wait;				//返回接收未完成标志
}

/*
 * 功  能：初始化 ESP8266的 CH_PD GPIO16 两个引脚，并且设置为高电平
 * 参  数：无
 * 返回值：无
 */
void ESP8266_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	//1、开启 GPIOC GPIOE 时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	
	//2、配置 GPIO 引脚模式
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_25MHz;
	
	//3、初始化 PC11 
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_11;
	GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	//3、初始化 PE5
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_5;
	GPIO_Init(GPIOE, &GPIO_InitStruct);
	
	//4、初始灯灭
	GPIO_SetBits(GPIOC,GPIO_Pin_11);
	GPIO_SetBits(GPIOE,GPIO_Pin_5);
}

/*
 * 功  能：初始化 ESP8266的 UART 引脚
 * 参  数：无
 * 返回值：无
 */
void ESP8266_UART_Init(void)
{
	UART2_Init(115200);
}



//==========================================================
//	函数名称：	A7_EnterTrans
//
//	函数功能：	进入透传模式
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void ESP8266_EnterTrans(void)
{	
	ESP8266_SendCmd("AT+CIPMODE=1\r\n", (uint8_t *)"OK");//使能透传
	delay_ms(500); 
	ESP8266_Clear(); 
	ESP8266_SendCmd("AT+CIPSEND\r\n", (uint8_t *)">");  //通过串口2发送AT指令
	delay_ms(500);
}

//==========================================================
//	函数名称：	A7_QuitTrans
//
//	函数功能：	退出透传模式
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：		连续发送三个‘+’，然后关闭透传模式
//==========================================================
void ESP8266_QuitTrans(void)
{
	delay_ms(500); 
	while((USART2->SR & 0X40) == 0);	//等待发送空
	USART2->DR = '+';
	delay_ms(15); 					//大于串口组帧时间(10ms)

	while((USART2->SR & 0X40) == 0);	//等待发送空
	USART2->DR = '+';        
	delay_ms(15); 					//大于串口组帧时间(10ms)

	while((USART2->SR & 0X40) == 0);	//等待发送空
	USART2->DR = '+';        
	delay_ms(500); 					//大于串口组帧时间(10ms)

	//usartx_puchar(USART1,"+++\r\n",5);  //方便调试用，通过串口1查看
	printf("+++\r\n");

}


//==========================================================
//	函数名称：	A7_SendData
//
//	函数功能：	发送数据
//
//	入口参数：	data：数据
//				len：长度
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void ESP8266_SendData(unsigned char *data, int len)
{
	ESP8266_Clear();								//清空接收缓存
	ESP8266_EnterTrans();
	//usartx_puchar(USART1, data, len);	//发送设备连接请求数据
	//usartx_puchar(USART3, data, len);	//发送设备连接请求数据
	printf("data:%s\r\n",data);
	UART2_SendString((char *)data) ;
	//usartx_send(USART1,"\r\n");
	printf("\r\n");
	printf("ESP8266_QuitTrans Start...\r\n");
	ESP8266_QuitTrans();
}


_Bool OneNet_EDPKitCmd(unsigned char *data)
{
	if(data[0] == CONNRESP) //连接响应
	{	
		printf("DevLink: ");
		//0		连接成功
		//1		验证失败：协议错误
		//2		验证失败：设备ID鉴权失败
		//3		验证失败：服务器失败
		//4		验证失败：用户ID鉴权失败
		//5		验证失败：未授权
		//6		验证失败：激活码未分配
		//7		验证失败：该设备已被激活
		//8		验证失败：重复发送连接请求包
		//9		验证失败：重复发送连接请求包
		if(data[3] == 0)
			return 1;
		else
			return 0;
	}
	
	return 0;

}
//==========================================================
//	函数名称：	OneNet_DevLink
//
//	函数功能：	与onenet创建连接
//
//	入口参数：	无
//
//	返回参数：	1-成功	0-失败
//
//	说明：		与onenCIPTMODEet平台建立连接
//==========================================================
_Bool OneNet_DevLink(const char* devid, const char* auth_key)
{
	_Bool status = 0;
	send_pkg = PacketConnect1(devid, auth_key);					                         	   //根据devid 和 apikey封装协议包     
	ESP8266_Clear();			                                                           //清空缓存
	ESP8266_SendData(send_pkg->_data, send_pkg->_write_pos);                         //发送数据到平台
	delay_ms(100);	
	//usartx_send(USART1,"\r\n{");
	printf("\r\n{");
	//usartx_puchar(USART1, (u8*)Wifi_receive, strlen((const char *)Wifi_receive));usartx_send(USART1,"}\r\n");   
	printf("OneNet_DevLink:%s",Wifi_receive);
	printf("\r\n{");
	DeleteBuffer(&send_pkg);										//删包
	ESP8266_Clear();
	return status;
}

//把数据设置为平台可识别的格式
void OneNet_FillBuf(char *buf,char *buf1,u8 number)  
{
	char text[25] = {0};
	memset(buf, 0, sizeof(buf));
	strcat((char *)buf, ",;");	
	strcat((char *)buf, buf1);
	strcat((char *)buf, ",");
	sprintf(text,"%d",number);
	strcat((char *)buf, text);
	strcat((char *)buf, ";");
}

//==========================================================
//	函数名称：	OneNet_SendData
//
//	函数功能：	上传数据到平台
//
//	入口参数：	type：发送数据的格式
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void OneNet_SendData(char *buf,char *buf1,u8 number)
{
	OneNet_FillBuf(buf,buf1,number);														//封装数据流	

	send_pkg = PacketSavedataSimpleString(NULL,(const char *) buf);							//封包-Type5

	ESP8266_SendData(send_pkg->_data, send_pkg->_write_pos);

	DeleteBuffer(&send_pkg);		 		//删包
	
}



