#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdarg.h>

uint8_t flag;


char Rx[100];

void CHUANKOU_Send_Init(void)
{
	GPIO_InitTypeDef GPIO_Inistructure;	//关于GPIO口的结构体定义,初始化GPIO用的
	
	NVIC_InitTypeDef NVIC_InitStructure;	//NVIC 的结构体
	
	USART_InitTypeDef USART_InitStructure;
	

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_Inistructure.GPIO_Mode = GPIO_Mode_AF_PP;	
	GPIO_Inistructure.GPIO_Pin = GPIO_Pin_9;	
	GPIO_Inistructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_Init(GPIOA,&GPIO_Inistructure);
	
	GPIO_Inistructure.GPIO_Mode = GPIO_Mode_IPU;	
	GPIO_Inistructure.GPIO_Pin = GPIO_Pin_10;	
	GPIO_Inistructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_Inistructure);
	
	//波特率
	USART_InitStructure.USART_BaudRate = 115200;
	//不使用控流
	USART_InitStructure.USART_HardwareFlowControl= USART_HardwareFlowControl_None;
	//USART_Mode_Rx接收模式		USART_Mode_Tx发送模式
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	//USART_Parity_No不需要校验
	USART_InitStructure.USART_Parity = USART_Parity_No;
	//停止位选择1
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	//字长选择 8位
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	//初始化函数
	USART_Init(USART1,&USART_InitStructure);
	
	
	//开启串口中断
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//NVIC 分组
		//NVIC结构体的成员变量
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;	//指定通道
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	//使能
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	//优先级设置
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;	//优先级设置
	NVIC_Init(&NVIC_InitStructure);	
	
	USART_Cmd(USART1, ENABLE);
}
void Send_Imf(uint8_t Byte)
{
	//发送数据
	USART_SendData(USART1,Byte);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);
}

uint8_t Getflag(void)
{
	if(flag == 1)
	{
		flag = 0;
		return 1;
	}
	return 0;
}

void Send_Gstring(uint8_t *str, uint16_t Length)
{
	uint16_t i;
	for (i = 0; i < Length; i ++)
	{
		Send_Imf(str[i]);
	}
}

void Send_String(char *str)
{
	uint16_t i;
	for(i = 0;str[i] != '\0'; i++)
	{
		Send_Imf(str[i]);
	}
}

uint32_t Serial_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;
	while (Y --)
	{
		Result *= X;
	}
	return Result;
}

void Send_Number(uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i ++)
	{
		Send_Imf(Number / Serial_Pow(10, Length - i - 1) % 10 + '0');
	}
}

int fputc(int ch, FILE *f)
{
	Send_Imf(ch);
	return ch;
}

void chuanKou_SendStream(void)
{
	//先发送头部标记
	Send_Imf(0xFF);
	//发送数据本身
	//Send_Gstring(Tx,4);
	//最后发送尾部标记
	Send_Imf(0xFE);
}

void Serial_Printf(char *format, ...)
{
	char String[100];
	va_list arg;
	va_start(arg, format);
	vsprintf(String, format, arg);
	va_end(arg);
	Send_String(String);
}


//中断函数
void USART1_IRQHandler()
{
	static uint8_t sbuf = 0;
	static uint8_t mark = 0;
	if(USART_GetFlagStatus(USART1,USART_FLAG_RXNE) == SET)
	{
		uint8_t Rxdata = USART_ReceiveData(USART1);
		if (sbuf == 0)
		{
			if (Rxdata == '@'&& flag == 0)
			{
				sbuf = 1;
				mark = 0;
			}
		}
		else if (sbuf == 1)
		{
			if(Rxdata == '*')
			{
				sbuf = 0;
				Rx[mark] = '\0';
				flag = 1;
			}
			else 
			{
				Rx[mark] = Rxdata;
				mark ++;
			}
		}
		//清除USARTx的中断挂起位
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
	}
}





