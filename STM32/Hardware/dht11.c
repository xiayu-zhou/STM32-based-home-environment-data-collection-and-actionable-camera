#include "stm32f10x.h"                  // Device header
#include "FreeRTOS.h"  
#include "task.h"
#include "MY_TIME.h"
#include "CHUANKOU.h"
#include "dht11.h"

#define DHT_GPIO_PORT		GPIOB
#define DHT_GPIO_PIN		GPIO_Pin_12
#define DHT_RCC_PORT		RCC_APB2Periph_GPIOB

//在此函数中加入初始化sysytick定时器步骤
//参照正点原子例程进行修改
void delay_us(u32 nus)
{
       u32 ticks;
       u32 told,tnow,reload,tcnt=0;
       if((0x0001&(SysTick->CTRL)) ==0)    //定时器未工作
              vPortSetupTimerInterrupt();  //初始化定时器
 
       reload = SysTick->LOAD;                     //获取重装载寄存器值
       ticks = nus * (SystemCoreClock / 1000000);  //计数时间值
       told=SysTick->VAL;                          //获取当前数值寄存器值（开始时数值）
 
       while(1)
       {
              tnow=SysTick->VAL;          //获取当前数值寄存器值
              if(tnow!=told)              //当前值不等于开始值说明已在计数
              {         
 
                     if(tnow<told)             //当前值小于开始数值，说明未计到0
                          tcnt+=told-tnow;     //计数值=开始值-当前值
 
                     else                  //当前值大于开始数值，说明已计到0并重新计数
                            tcnt+=reload-tnow+told;   //计数值=重装载值-当前值+开始值  （已
                                                      //从开始值计到0） 
 
                     told=tnow;                //更新开始值
                     if(tcnt>=ticks)break;     //时间超过/等于要延迟的时间,则退出.
              } 
       }     
}
 
//SystemCoreClock为系统时钟(system_stmf4xx.c中)，通常选择该时钟作为
//systick定时器时钟，根据具体情况更改

/**
  * @brief  DHT11_GPIO初始化函数
  * @param  Mode：指定输入或输出模式
  * @return None
  */
void DHT_GPIO_Init(GPIOMode_TypeDef Mode)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(DHT_RCC_PORT,ENABLE);
	
	GPIO_SetBits(DHT_GPIO_PORT,DHT_GPIO_PIN);
	
	GPIO_InitStructure.GPIO_Mode = Mode;
	GPIO_InitStructure.GPIO_Pin = DHT_GPIO_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(DHT_GPIO_PORT,&GPIO_InitStructure);
	
}

/**
  * @brief  DHT11模块起始信号函数
  * @param  None
  * @return 1或0，标志起动信号成功与否
  */
uint8_t DHT_Start(void)
{
	DHT_GPIO_Init(GPIO_Mode_Out_PP);							//输出模式
	
	GPIO_ResetBits(DHT_GPIO_PORT,DHT_GPIO_PIN);		//输出20ms低电平后拉高
	vTaskDelay(20);
	GPIO_SetBits(DHT_GPIO_PORT,DHT_GPIO_PIN);
	
	DHT_GPIO_Init(GPIO_Mode_IN_FLOATING);					//输入模式
	delay_us(20);
	
	if(!GPIO_ReadInputDataBit(DHT_GPIO_PORT,DHT_GPIO_PIN))
	{
		
		while(!GPIO_ReadInputDataBit(DHT_GPIO_PORT,DHT_GPIO_PIN));
		
		while(GPIO_ReadInputDataBit(DHT_GPIO_PORT,DHT_GPIO_PIN));

		return 1;
	}
	return 0;
}

/**
  * @brief  接收DHT11发送来8位的数据
  * @param  None
  * @return 返回接收到的8位数据
  */
uint8_t DHT_Get_Byte_Data(void)
{
	uint8_t temp;
	for(int i = 0; i < 8; i++)
	{
		temp <<= 1;
		while(!GPIO_ReadInputDataBit(DHT_GPIO_PORT,DHT_GPIO_PIN));
		delay_us(27);
		GPIO_ReadInputDataBit(DHT_GPIO_PORT,DHT_GPIO_PIN) ? (temp |= 0x01) : (temp &= ~0x01);
		while(GPIO_ReadInputDataBit(DHT_GPIO_PORT,DHT_GPIO_PIN));
	}
			
	return temp;
}

/**
  * @brief  获取DHT11的温度湿度数据
  * @param  buffer[]：需要传入一个存储数据的数组
  * @return 返回数据校验是否正确 1：正确 0：失败
  */
uint8_t DHT_Get_Temp_Humi_Data(uint8_t buffer[])
{
	if(DHT_Start())
	{
		buffer[0] = DHT_Get_Byte_Data();
		buffer[1] = DHT_Get_Byte_Data();
		//Serial_Printf("Humi:%d.%d\r\n",buffer[0],buffer[1]);
		//delay_us(224);
		//buffer[2] = DHT_Get_Byte_Data();	
		//buffer[3] = DHT_Get_Byte_Data();
		//Serial_Printf("12sdcsd1\r\n");	
		//Serial_Printf("Humi:%d.%d\r\n",buffer[0],buffer[2]);
		//buffer[4] = DHT_Get_Byte_Data();
				
	}
	return (buffer[0]+buffer[1]+buffer[2]+buffer[3] == buffer[4]) ? 1 : 0;
}
