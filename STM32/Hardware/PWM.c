#include "stm32f10x.h"                  // Device header


void PWM_Init(void)
{
	//Time Base初始化结构定义
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	
	TIM_OCInitTypeDef TIM_OCInitStructure;
	
	GPIO_InitTypeDef GPIO_Inistructure;	//关于GPIO口的结构体定义,初始化GPIO用的
	//***********************************************************************
	
	
	//***********************************************************************
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);	//用来开启或关闭APB2外设时钟
	GPIO_Inistructure.GPIO_Mode = GPIO_Mode_AF_PP;	//GPIO_Mode 模式//GPIO_Mode_AF_PP复用推挽输出
	GPIO_Inistructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;	
	GPIO_Inistructure.GPIO_Speed = GPIO_Speed_50MHz;	//指定所选引脚的速度
	GPIO_Init(GPIOA,&GPIO_Inistructure);	//根据指定的参数初始化GPIOx外围设备 ，GPIO_InitStruct中的参数 
	//***********************************************************************
	
	
	
	//开启RCC TIM2 时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	
	//配置TIMx内部时钟 TIM2时钟
	TIM_InternalClockConfig(TIM2);
	
	//指定时钟分频 1分频
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	//计数模式 选择向上模式
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	//周期，ARR自动重装的值
	TIM_TimeBaseInitStructure.TIM_Period = 20000 - 1;	//ARR
	//PSC预分频器的值
	TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;	//PSC
	//重复计数器的值  指定重复计数器值。 每次RCR向下计数器  达到零，生成一个更新事件并重新开始计数  从RCR值 
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	
	//初始化TIMx时间基准单元外设  * TIM_TimeBaseInitStruct中指定的参数
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);
	
	

	TIM_OCStructInit(&TIM_OCInitStructure);
	//输出的模式
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	//设置输出比较的极性	高电平
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	//设置输出使能
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	//设置CCR
	TIM_OCInitStructure.TIM_Pulse = 0;
	
	//通道1的初始化
	TIM_OC1Init(TIM2,&TIM_OCInitStructure);
	//通道2的初始化
	TIM_OC2Init(TIM2,&TIM_OCInitStructure);
	
	
	//启用或禁用指定TIM外设
	TIM_Cmd(TIM2,ENABLE);	
}
//抬头低头
void PWM_SetCompare1(uint16_t compare)
{
	
	TIM_SetCompare1(TIM2,compare*2000/180+500);
}
//水平旋转
void PWM_SetCompare2(uint16_t compare)
{
	TIM_SetCompare2(TIM2,compare*2000/180+500);
}
