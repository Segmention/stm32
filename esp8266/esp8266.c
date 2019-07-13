#include "esp8266.h" 

unsigned int Wifi_lastcnt = 0;;
extern unsigned int Wifi_cnt;
extern unsigned char Wifi_receive[128];
extern uint8_t connect_out;

EdpPacket *send_pkg,*send_pkg1;	//Э��� 

/*
 * ESP8266-01 ���Ž���˵��
 * ESP8266			STM32
 * GND				GND
 * TXD				USART_RXD
 * RXD 				USART_TXD
 * VCC				3.3V
 * CH_PD			3.3V	--->PC11 --->DCMI_D4
 * GPIO16			3.3V	--->PE5  --->DCMI_D6
 */
/*
 * ��  �ܣ���ʼ�� ESP8266
 * ��  ������
 * ����ֵ����
 */
void ESP8266_Init(void)
{
	ESP8266_GPIO_Init();
	ESP8266_UART_Init();
	
	ESP8266_AT_Init();
	UART2_SendString("ESP8266_Init Success!\r\n") ;
}



/*
 * ��  �ܣ�ESP8266��ʼ�� AT ָ��
 * ��  ������
 * ����ֵ����
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
 * ��  �ܣ�ESP8266ģ�鷢������
 * ��  ����
 * 		cmd		--->����
 * 		res		--->��Ҫ���ķ���ָ��
 * ����ֵ��
 * 		0		--->�ɹ�
 *  	1		--->ʧ��
 */
uint8_t ESP8266_SendCmd(char *cmd ,uint8_t *res)  
{
	unsigned char timeout = 50;
	
	UART2_SendString(cmd);   //ʹ�� ����2 ���� AT ָ��
	printf("ESP8266_SendCmd:%s",cmd); //�������ʹ�ã�ͨ������1����
	
	while(timeout--)
	{
		if(ESP8266_WaitRecive() == Wifi_OK)
		{
			printf("Wifi_receive:%s\r\n",Wifi_receive);
			
			if(strstr((const char *)Wifi_receive , (const char *)res) != NULL)
			{
				//��⵽�йؼ���
				ESP8266_Clear();   //��ջ���
				return 0;
			}
			
			ESP8266_Clear();
		}
		
		delay_ms(10); 
	}
	
	return 1;
}

/*
 * ��  �ܣ�����жϽ��յ��ַ���
 * ��  ������
 * ����ֵ����
 */
void ESP8266_Clear(void) 
{
	memset(Wifi_receive,0,sizeof(Wifi_receive));
	Wifi_cnt = 0;
}

/*
 * ��  �ܣ��ȴ��������
 * ��  ������
 * ����ֵ��
 *		Wifi_OK		--->�������
 *		Wifi_wait	--->���ճ�ʱδ���
 * ˵  ����ѭ���ü���Ƿ�������
 */
uint8_t ESP8266_WaitRecive(void) 
{
	if(Wifi_cnt == 0)	//������ռ���Ϊ0 ��˵��û�д��ڽ��������У�����ֱ����������������
		return Wifi_wait;
	
	if(Wifi_cnt == Wifi_lastcnt)	//�����һ�ε�ֵ�������ͬ����˵���������
	{	
		Wifi_cnt = 0;				//��0���ռ���
		return Wifi_OK;				//���ؽ�����ɱ�־
	}
	
	Wifi_lastcnt = Wifi_cnt;		//��Ϊ��ͬ
						
	return Wifi_wait;				//���ؽ���δ��ɱ�־
}

/*
 * ��  �ܣ���ʼ�� ESP8266�� CH_PD GPIO16 �������ţ���������Ϊ�ߵ�ƽ
 * ��  ������
 * ����ֵ����
 */
void ESP8266_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	//1������ GPIOC GPIOE ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	
	//2������ GPIO ����ģʽ
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_25MHz;
	
	//3����ʼ�� PC11 
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_11;
	GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	//3����ʼ�� PE5
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_5;
	GPIO_Init(GPIOE, &GPIO_InitStruct);
	
	//4����ʼ����
	GPIO_SetBits(GPIOC,GPIO_Pin_11);
	GPIO_SetBits(GPIOE,GPIO_Pin_5);
}

/*
 * ��  �ܣ���ʼ�� ESP8266�� UART ����
 * ��  ������
 * ����ֵ����
 */
void ESP8266_UART_Init(void)
{
	UART2_Init(115200);
}



//==========================================================
//	�������ƣ�	A7_EnterTrans
//
//	�������ܣ�	����͸��ģʽ
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void ESP8266_EnterTrans(void)
{	
	ESP8266_SendCmd("AT+CIPMODE=1\r\n", (uint8_t *)"OK");//ʹ��͸��
	delay_ms(500); 
	ESP8266_Clear(); 
	ESP8266_SendCmd("AT+CIPSEND\r\n", (uint8_t *)">");  //ͨ������2����ATָ��
	delay_ms(500);
}

//==========================================================
//	�������ƣ�	A7_QuitTrans
//
//	�������ܣ�	�˳�͸��ģʽ
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����		��������������+����Ȼ��ر�͸��ģʽ
//==========================================================
void ESP8266_QuitTrans(void)
{
	delay_ms(500); 
	while((USART2->SR & 0X40) == 0);	//�ȴ����Ϳ�
	USART2->DR = '+';
	delay_ms(15); 					//���ڴ�����֡ʱ��(10ms)

	while((USART2->SR & 0X40) == 0);	//�ȴ����Ϳ�
	USART2->DR = '+';        
	delay_ms(15); 					//���ڴ�����֡ʱ��(10ms)

	while((USART2->SR & 0X40) == 0);	//�ȴ����Ϳ�
	USART2->DR = '+';        
	delay_ms(500); 					//���ڴ�����֡ʱ��(10ms)

	//usartx_puchar(USART1,"+++\r\n",5);  //��������ã�ͨ������1�鿴
	printf("+++\r\n");

}


//==========================================================
//	�������ƣ�	A7_SendData
//
//	�������ܣ�	��������
//
//	��ڲ�����	data������
//				len������
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void ESP8266_SendData(unsigned char *data, int len)
{
	ESP8266_Clear();								//��ս��ջ���
	ESP8266_EnterTrans();
	//usartx_puchar(USART1, data, len);	//�����豸������������
	//usartx_puchar(USART3, data, len);	//�����豸������������
	printf("data:%s\r\n",data);
	UART2_SendString((char *)data) ;
	//usartx_send(USART1,"\r\n");
	printf("\r\n");
	printf("ESP8266_QuitTrans Start...\r\n");
	ESP8266_QuitTrans();
}


_Bool OneNet_EDPKitCmd(unsigned char *data)
{
	if(data[0] == CONNRESP) //������Ӧ
	{	
		printf("DevLink: ");
		//0		���ӳɹ�
		//1		��֤ʧ�ܣ�Э�����
		//2		��֤ʧ�ܣ��豸ID��Ȩʧ��
		//3		��֤ʧ�ܣ�������ʧ��
		//4		��֤ʧ�ܣ��û�ID��Ȩʧ��
		//5		��֤ʧ�ܣ�δ��Ȩ
		//6		��֤ʧ�ܣ�������δ����
		//7		��֤ʧ�ܣ����豸�ѱ�����
		//8		��֤ʧ�ܣ��ظ��������������
		//9		��֤ʧ�ܣ��ظ��������������
		if(data[3] == 0)
			return 1;
		else
			return 0;
	}
	
	return 0;

}
//==========================================================
//	�������ƣ�	OneNet_DevLink
//
//	�������ܣ�	��onenet��������
//
//	��ڲ�����	��
//
//	���ز�����	1-�ɹ�	0-ʧ��
//
//	˵����		��onenCIPTMODEetƽ̨��������
//==========================================================
_Bool OneNet_DevLink(const char* devid, const char* auth_key)
{
	_Bool status = 0;
	send_pkg = PacketConnect1(devid, auth_key);					                         	   //����devid �� apikey��װЭ���     
	ESP8266_Clear();			                                                           //��ջ���
	ESP8266_SendData(send_pkg->_data, send_pkg->_write_pos);                         //�������ݵ�ƽ̨
	delay_ms(100);	
	//usartx_send(USART1,"\r\n{");
	printf("\r\n{");
	//usartx_puchar(USART1, (u8*)Wifi_receive, strlen((const char *)Wifi_receive));usartx_send(USART1,"}\r\n");   
	printf("OneNet_DevLink:%s",Wifi_receive);
	printf("\r\n{");
	DeleteBuffer(&send_pkg);										//ɾ��
	ESP8266_Clear();
	return status;
}

//����������Ϊƽ̨��ʶ��ĸ�ʽ
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
//	�������ƣ�	OneNet_SendData
//
//	�������ܣ�	�ϴ����ݵ�ƽ̨
//
//	��ڲ�����	type���������ݵĸ�ʽ
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void OneNet_SendData(char *buf,char *buf1,u8 number)
{
	OneNet_FillBuf(buf,buf1,number);														//��װ������	

	send_pkg = PacketSavedataSimpleString(NULL,(const char *) buf);							//���-Type5

	ESP8266_SendData(send_pkg->_data, send_pkg->_write_pos);

	DeleteBuffer(&send_pkg);		 		//ɾ��
	
}



