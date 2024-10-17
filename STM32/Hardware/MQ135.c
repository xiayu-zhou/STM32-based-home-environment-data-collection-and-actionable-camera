
#include "MQ135.h"
#include "Delay.h"
#include "OLED.h"
#include <stdio.h>


// 定义MQ-135的数据引脚
#define MQ135_DATA_PIN       GPIO_Pin_2
#define MQ135_DATA_PORT      GPIOA

// 定义ADC通道
#define MQ135_ADC_CHANNEL    ADC_Channel_2

// 定义参考电压
#define VREF                 3.3f  // 参考电压为3.3V

// 定义MQ-135传感器的参数
#define RL                   10.0  // 负载电阻（单位：千欧姆）
#define RO_CLEAN_AIR_FACTOR  9.83  // 清洁空气下的RO因子
void GPIO1_Init();

// 初始化ADC
void MQ135_Init() {
	
	GPIO1_Init();
    // 使能ADC1时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    
    // 配置ADC工作模式为独立工作模式
    ADC_InitTypeDef ADC_InitStructure;
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStructure);

    // 配置ADC通道
    ADC_RegularChannelConfig(ADC1, MQ135_ADC_CHANNEL, 1, ADC_SampleTime_55Cycles5);

    // 使能ADC
    ADC_Cmd(ADC1, ENABLE);

    // 校准ADC
    ADC_ResetCalibration(ADC1);
    while (ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while (ADC_GetCalibrationStatus(ADC1));
}

// 初始化GPIO引脚
void GPIO1_Init() {
    GPIO_InitTypeDef GPIO_InitStructure;

    // 使能GPIOA时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    // 配置MQ135数据引脚为模拟输入
    GPIO_InitStructure.GPIO_Pin = MQ135_DATA_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(MQ135_DATA_PORT, &GPIO_InitStructure);
}

// 读取MQ-135传感器值
uint16_t MQ135_Read() {
    // 启动转换
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);

    // 等待转换完成
    while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));

    // 读取ADC值
    return ADC_GetConversionValue(ADC1);
}

// 计算空气质量浓度
float CalculateAirQuality(uint16_t adcValue) {
    // 计算传感器电阻
    float sensorVoltage = (float)adcValue / 4096 * VREF;
    float sensorResistance = (VREF - sensorVoltage) * RL / sensorVoltage;

    // 计算空气质量浓度
    float airQuality = sensorResistance / RO_CLEAN_AIR_FACTOR;

    return airQuality;
}

float getMQ135()
{
	float airQuality;
	// 用于存储传感器读数
    uint16_t mq135Value;
	// 读取MQ-135传感器值
	mq135Value = MQ135_Read();

	// 使用传感器值进行进一步处理，例如转换为气体浓度等
	
	airQuality = CalculateAirQuality(mq135Value);
	
	return airQuality;
}
