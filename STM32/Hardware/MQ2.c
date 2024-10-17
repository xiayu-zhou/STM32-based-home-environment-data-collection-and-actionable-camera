#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include <stdio.h>
#include <math.h>

#define CAL_PPM 20  // 校准环境中PPM值
#define RL			5		// RL阻值
static float R0; // 元件在洁净空气中的阻值


//////////////////////////////////////////////////////////////////
void MQ2_Init(void) {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    ADC_InitTypeDef ADC_InitStruct;
    ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStruct.ADC_ScanConvMode = DISABLE;
    ADC_InitStruct.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStruct.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStruct);

    ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 1, ADC_SampleTime_239Cycles5);
    ADC_Cmd(ADC1, ENABLE);
    ADC_ResetCalibration(ADC1);

    while(ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);

    while(ADC_GetCalibrationStatus(ADC1));
}

uint16_t ADC1_Read(void) {
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
    return ADC_GetConversionValue(ADC1);
}
// 传感器校准函数
void MQ2_PPM_Calibration(float RS)
{
    R0 = RS / pow(CAL_PPM / 613.9f, 1 / -2.074f);
}

float getMQ2()
{
	uint16_t adc_value = ADC1_Read();
	
	float Vrl = 3.3f * adc_value / 4095.f;
	float RS = (3.3f - Vrl) / Vrl * RL; 
	R0 = RS / pow(CAL_PPM / 613.9f, 1 / -2.074f);
	float ppm = 613.9f * pow(RS/R0, -2.074f);
	
	return ppm;
}
