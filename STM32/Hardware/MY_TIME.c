#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Delay.h"
#include "MY_TIME.h"
#include "main.h"


uint8_t Send_Data_Flag = 0;

void Time_Init(void)
{
	//Time Base初始化结构定义
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	
	//NVIC是ARM Cortex-M系列处理器中的中断控制器，它负责管理和处理所有中断请求。
	//NVIC 的结构体
	NVIC_InitTypeDef NVIC_InitStructure;	
	
	//***********************************************************************
	//开启RCC TIM2 时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	
	//配置TIMx内部时钟 TIM2时钟
	TIM_InternalClockConfig(TIM3);
	
	//指定时钟分频 1分频
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	//计数模式 选择向上模式
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	//周期，ARR自动重装的值
	TIM_TimeBaseInitStructure.TIM_Period = 20000 - 1;
	//PSC预分频器的值
	TIM_TimeBaseInitStructure.TIM_Prescaler = 7200 - 1;
	//重复计数器的值  指定重复计数器值。 每次RCR向下计数器  达到零，生成一个更新事件并重新开始计数  从RCR值 
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	
	//初始化TIMx时间基准单元外设  * TIM_TimeBaseInitStruct中指定的参数
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);
	
	
	//启用或禁用指定的TIM中断	//TIM_IT_Update是更新中断
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
	
	//配置优先级分组:优先级和子优先级
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	//NVIC结构体的成员变量
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;	//指定通道
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	//使能
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	//优先级设置
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;	//优先级设置
	
	//根据NVIC_InitStruct中指定的参数初始化NVIC外设
	NVIC_Init(&NVIC_InitStructure);	
	
	//启用或禁用指定TIM外设
	TIM_Cmd(TIM3,DISABLE);	
}

void TIM3_IRQHandler(void)
{
	//检查是否是更新中断位
	if(TIM_GetITStatus(TIM3,TIM_IT_Update) == SET)
	{
		Send_Data_Flag = 1;
		//清除标志位	清除TIMx的中断挂起位
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
		
	}
	return;
}



// -----------------------------------------------------------------------------------------------
void TIM4_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);  ///使能TIM3时钟
	TIM_TimeBaseInitStructure.TIM_Period    = 50000-1; 	//自动重装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler = 72-1;     //定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStructure);//初始化TIM3
}

//微秒级延时
void TIM4_Delayus(u16 xus)
{
	TIM_Cmd(TIM4,ENABLE); //启动定时器
	while(TIM3->CNT < xus);
	TIM4->CNT = 0;
	TIM_Cmd(TIM4,DISABLE); //关闭定时器
}

//毫秒级延时
void TIM4_Delayms(u16 xms)
{
	int i;
	for(i=0;i<xms;i++)
	{
		TIM4_Delayus(1000);
	}
}



