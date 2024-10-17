#ifndef __DELAY_H
#define __DELAY_H
#include "FreeRTOS.h"


void Delay_us(uint32_t us);
void Delay_ms(uint32_t ms);
void Delay_s(uint32_t s);


void delay_ms_duo(uint32_t ms);
void delay_us_duo(u32 nus);

#endif
