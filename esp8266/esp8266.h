#ifndef _ESP8266_H
#define _ESP8266_H

//头文件
#include "stm32f4xx.h"
#include "uart2.h"
#include "uart1.h"
#include "string.h"
#include "stdio.h"
#include "systick.h"
#include "EdpKit.h"

//宏定义
#define   Wifi_OK    0
#define   Wifi_wait  1
//以下都是AT指令，具体功能看ESP8266的手册
#define   AT          "AT\r\n"	      
#define   CWMODE      "AT+CWMODE=1\r\n"
#define   RST         "AT+RST\r\n"
#define   CIFSR       "AT+CIFSR\r\n"
#define   CWJAP       "AT+CWJAP=\"OPPOLLH\",\"01234567\"\r\n"
#define   CIPSTART    "AT+CIPSTART=\"TCP\",\"183.230.40.39\",876\r\n"
//#define   CIPSTART    "AT+CIPSTART=\"TCP\",\"183.230.40.33\",80\r\n"
#define   CIPMODE     "AT+CIPMODE=1\r\n"
#define   CIPSEND     "AT+CIPSEND\r\n"
#define   CIPSTATUS   "AT+CIPSTATUS\r\n"

//以下都是发送AT指令后wifi模块返回的值
#define   WIFICONNECT  "WIFI CONNECT"
#define   CONNECTING  "STATUS:3"
#define   UNCONNECT1  "STATUS:2"
#define   UNCONNECT2  "STATUS:4"
#define   answer1     "OK"
#define   answer2     "no change"
#define   answer3     "+++"
#define   answer4     "ready"
#define   CONNECT     "CONNECT"
#define   CLOSE       "CIPCLOSE"

#define DEVID  	"524290107"        //设备号 可以改
#define APIKEY	"8K6Mqlr8llff5uSi7W1gFJAFsnc=" //API钥匙 可以改

/***************** 函数声明 *****************/
void ESP8266_GPIO_Init(void);
void ESP8266_AT_Init(void);
void ESP8266_Init(void);
void ESP8266_UART_Init(void);
uint8_t ESP8266_SendCmd(char *cmd ,uint8_t *res);
void ESP8266_Clear(void);
uint8_t ESP8266_WaitRecive(void);

void ESP8266_EnterTrans(void);
void ESP8266_QuitTrans(void);
void ESP8266_SendData(unsigned char *data, int len);
_Bool OneNet_EDPKitCmd(unsigned char *data);
_Bool OneNet_DevLink(const char* devid, const char* auth_key);
void OneNet_FillBuf(char *buf,char *buf1,u8 number)  ;
void OneNet_SendData(char *buf,char *buf1,u8 number);

#endif
