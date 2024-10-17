#include "stm32f10x.h"
#include "FreeRTOS.h"
/**
  * @brief  微秒级延时
  * @param  xus 延时时长，范围：0~233015
  * @retval 无
  */
void Delay_us(uint32_t xus)
{
	SysTick->LOAD = 72 * xus;				//设置定时器重装值
	SysTick->VAL = 0x00;					//清空当前计数值
	SysTick->CTRL = 0x00000005;				//设置时钟源为HCLK，启动定时器
	while(!(SysTick->CTRL & 0x00010000));	//等待计数到0
	SysTick->CTRL = 0x00000004;				//关闭定时器
}

/**
  * @brief  毫秒级延时
  * @param  xms 延时时长，范围：0~4294967295
  * @retval 无
  */
void Delay_ms(uint32_t xms)
{
	while(xms--)
	{
		Delay_us(1000);
	}
}
 
/**
  * @brief  秒级延时
  * @param  xs 延时时长，范围：0~4294967295
  * @retval 无
  */
void Delay_s(uint32_t xs)
{
	while(xs--)
	{
		Delay_ms(1000);
	}
} 

//void delay_us_duo(u32 nus)
//{
//       u32 ticks;
//       u32 told,tnow,reload,tcnt=0;
//       if((0x0001&(SysTick->CTRL)) ==0)    //定时器未工作
//              vPortSetupTimerInterrupt();  //初始化定时器
// 
//       reload = SysTick->LOAD;                     //获取重装载寄存器值
//       ticks = nus * (SystemCoreClock / 1000000);  //计数时间值
//       told=SysTick->VAL;                          //获取当前数值寄存器值（开始时数值）
// 
//       while(1)
//       {
//              tnow=SysTick->VAL;          //获取当前数值寄存器值
//              if(tnow!=told)              //当前值不等于开始值说明已在计数
//              {         
// 
//                     if(tnow<told)             //当前值小于开始数值，说明未计到0
//                          tcnt+=told-tnow;     //计数值=开始值-当前值
// 
//                     else                  //当前值大于开始数值，说明已计到0并重新计数
//                            tcnt+=reload-tnow+told;   //计数值=重装载值-当前值+开始值  （已
//                                                      //从开始值计到0） 
// 
//                     told=tnow;                //更新开始值
//                     if(tcnt>=ticks)break;     //时间超过/等于要延迟的时间,则退出.
//              } 
//       }     
//}


//void delay_ms_duo(uint32_t ms)
//{
//	vTaskDelay(ms);
//}

