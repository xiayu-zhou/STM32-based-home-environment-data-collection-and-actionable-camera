#include "FreeRTOS.h"
#include "task.h"
#include "stm32f10x.h"                  // Device header

#include "Delay.h"
#include "OLED.h"
#include "CHUANKOU.h"
#include "PWM.h"
#include "dht11.h"
#include "MQ2.h"
#include "MQ135.h"
#include "bsp_spi_nrf.h"
#include "MY_TIME.h"
#include "main.h"
/**************************** 任务句柄 ********************************/
/* 
 * 任务句柄是一个指针，用于指向一个任务，当任务创建好之后，它就具有了一个任务句柄
 * 以后我们要想操作这个任务都需要通过这个任务句柄，如果是自身的任务操作自己，那么
 * 这个句柄可以为NULL。
 */
/* 创建任务句柄 */
static TaskHandle_t AppTaskCreate_Handle = NULL;
/* LED1任务句柄 */
static TaskHandle_t Serial_Task_Handle = NULL;
/* PWM任务句柄 */
static TaskHandle_t PWM_Up_and_Down_Task_Handle = NULL;
/* PWM任务句柄 */
static TaskHandle_t PWM_Left_and_Right_Task_Handle = NULL;
/* PWM任务句柄 */
static TaskHandle_t DHT11_Task_Handle = NULL;
/*
*************************************************************************
*							全局变量
*************************************************************************
*/
u8 rxbuf[32];

extern uint8_t Send_Data_Flag;

/*
*************************************************************************
*                             函数声明
*************************************************************************
*/
static void AppTaskCreate(void);/* 用于创建任务 */

static void Serial_Task(void* parameter);/* LED1_Task任务实现 */

static void PWM_Up_and_Down_Task(void* pvParameters);/* PWM_Task任务实现 */

static void DHT11_Task(void* pvParameters);/* PWM_Task任务实现 */

static void PWM_Left_and_Right_Task(void* pvParameters);/* PWM_Task任务实现 */

static void BSP_Init(void);/* 用于初始化板载相关资源 */


int main(void)
{
	/* 定义一个创建信息返回值，默认为pdPASS */
	BaseType_t xReturn = pdPASS;
	
	  /* 开发板硬件初始化 */
	BSP_Init();
	
	TIM_Cmd(TIM3,ENABLE);	
	
	/* 创建AppTaskCreate任务 */
	xReturn = xTaskCreate((TaskFunction_t )AppTaskCreate,  /* 任务入口函数 */
						(const char*    )"AppTaskCreate",/* 任务名字 */
						(uint16_t       )512,  /* 任务栈大小 */
						(void*          )NULL,/* 任务入口函数参数 */
						(UBaseType_t    )1, /* 任务的优先级 */
						(TaskHandle_t*  )&AppTaskCreate_Handle);/* 任务控制块指针 */ 
	/* 启动任务调度 */           
	if(pdPASS == xReturn)
	{
		vTaskStartScheduler();   /* 启动任务，开启调度 */
	}
	else
	return -1;  
			
	while(1);   /* 正常不会执行到这里 */ 
		
}

static void AppTaskCreate(void)
{	
	BaseType_t xReturn = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */
  
	taskENTER_CRITICAL();           //进入临界区
						
					
	/* 创建 PWM_Up_and_Down_Task 任务 */
	xReturn = xTaskCreate((TaskFunction_t )PWM_Up_and_Down_Task, /* 任务入口函数 */
                        (const char*    )"PWM_Up_and_Down_Task",/* 任务名字 */
                        (uint16_t       )128,   /* 任务栈大小 */
                        (void*          )NULL,	/* 任务入口函数参数 */
                        (UBaseType_t    )1,	    /* 任务的优先级 */
                        (TaskHandle_t*  )&PWM_Up_and_Down_Task_Handle);/* 任务控制块指针 */
	
	/* 创建 PWM_Up_and_Down_Task 任务 */
	xReturn = xTaskCreate((TaskFunction_t )PWM_Left_and_Right_Task, /* 任务入口函数 */
						(const char*    )"PWM_Left_and_Right_Task",/* 任务名字 */
						(uint16_t       )128,   /* 任务栈大小 */
						(void*          )NULL,	/* 任务入口函数参数 */
						(UBaseType_t    )1,	    /* 任务的优先级 */
						(TaskHandle_t*  )&PWM_Left_and_Right_Task_Handle);/* 任务控制块指针 */	
	
							/* 创建 PWM_Up_and_Down_Task 任务 */
	xReturn = xTaskCreate((TaskFunction_t )DHT11_Task, /* 任务入口函数 */
						(const char*    )"DHT11_Task",/* 任务名字 */
						(uint16_t       )128,   /* 任务栈大小 */
						(void*          )NULL,	/* 任务入口函数参数 */
						(UBaseType_t    )1,	    /* 任务的优先级 */
						(TaskHandle_t*  )&DHT11_Task_Handle);/* 任务控制块指针 */	

	/* 创建 PWM_Up_and_Down_Task 任务 */
	xReturn = xTaskCreate((TaskFunction_t )Serial_Task, /* 任务入口函数 */
						(const char*    )"Serial_Task",/* 任务名字 */
						(uint16_t       )512,   /* 任务栈大小 */
						(void*          )NULL,	/* 任务入口函数参数 */
						(UBaseType_t    )1,	    /* 任务的优先级 */
						(TaskHandle_t*  )&Serial_Task_Handle);/* 任务控制块指针 */							
						
	vTaskDelete(AppTaskCreate_Handle); //删除AppTaskCreate任务
						
			
	taskEXIT_CRITICAL();            //退出临界区
						
					  
}
uint8_t DHT_Buffer[5];
static void Serial_Task(void* parameter)
{
	Rx[0] = '0';
	Rx[1] = '0';
	Rx[2] = '0';
	Rx[3] = '0';
	float ppm;
	float airQuality;
	char str[30];
	u8 status = 0;	
	u8 i;

	status = NRF_Check();
	if(status == SUCCESS)	   
	{}
	else	  
	{}
	NRF_RX_Mode();     //接收模式
    while (1)
    {
        if( Getflag() == SET)
		{
			Serial_Printf("%s\r\n",Rx);
			OLED_ShowString(2,1,Rx);
			if(Rx[2] == 'K'&& Rx[1] == '!' && Rx[0] == '!'){
				
				Serial_Printf("AT+CIPSEND=0,5\r\n");
				vTaskDelay(30);   /* 延时500个tick */
				Serial_Printf("22\r\n\r\n");
			}
		}
		if(Rx[2] == 'C' && Send_Data_Flag == 1)
		{
			Send_Data_Flag = 0;
			//status = NRF_Rx_Dat(rxbuf);
			if(status == RX_DR)
			{
				
			}
			ppm = getMQ135();
			
			airQuality = getMQ2();
			sprintf(str,"@%.2f#%.2f#%d.%2d#%d%d*",ppm,airQuality,DHT_Buffer[0],DHT_Buffer[1],rxbuf[0],rxbuf[2]);
			
			Serial_Printf("AT+CIPSEND=0,22\r\n");
			vTaskDelay(30);   /* 延时500个tick */
			Serial_Printf("%s",str);
			vTaskDelay(30);
			Serial_Printf("\r\n\r\n");
		}		
    }
}

static void DHT11_Task(void* pvParameters)
{
	while(1)
	{
		//Serial_Printf("1231\r\n");
		if(DHT_Get_Temp_Humi_Data(DHT_Buffer))
		{
//			Serial_Printf("Temp:%d.%d\r\n",DHT_Buffer[2],DHT_Buffer[3]);
			
			
		}
		//Serial_Printf("Humi:%d.%d\r\n",DHT_Buffer[0],DHT_Buffer[1]);
        vTaskDelay(2000);   /* 2秒刷新一次 */	
	}
}

static void PWM_Up_and_Down_Task(void* parameter)
{	
	int count = 90;
    while (1)
    {	
		if(Rx[0] == 'W' && count >= 40)
		{
			count--;
			PWM_SetCompare1(count);
			vTaskDelay(80);
		}else if(Rx[0] == 'S' && count <= 130){
			count++;
			PWM_SetCompare1(count);
			vTaskDelay(80);
		}else if(Rx[0] == '0'){
			PWM_SetCompare1(90);
			count = 90;
		}
		
		vTaskDelay(1);
    }
}
static void PWM_Left_and_Right_Task(void* parameter)
{	
	int count = 90;
    while (1)
    {	
		
		if(Rx[1] == 'D' && count >= 10)
		{
			count--;
			PWM_SetCompare2(count);
			vTaskDelay(80);
		}else if(Rx[1] == 'A' && count <= 170){
			count++;
			PWM_SetCompare2(count);
			vTaskDelay(80);
		}else if(Rx[1] == '0'){
			PWM_SetCompare2(90);
			count = 90;
		}
	}
}
//WIFI 初始化为服务器代码
void WIFI_Init()
{
	Serial_Printf("AT+CIPMUX=1\r\n");
	Delay_ms(500);	
	Serial_Printf("AT+CIPSERVER=1,6666\r\n");
	Delay_ms(500);	
	Serial_Printf("AT+CIFSR\r\n");
	Delay_ms(500);
}


/*外设初始化*/
static void BSP_Init(void)
{
	/*OLED显示屏的初始化*/
	OLED_Init();
	CHUANKOU_Send_Init();
	WIFI_Init();
	PWM_Init();//舵机初始化
	MQ135_Init();
	MQ2_Init();
	SPI_NRF_Init(); 
	Time_Init();
	//DHT_GPIO_Init(GPIO_Mode_Out_PP);
}

