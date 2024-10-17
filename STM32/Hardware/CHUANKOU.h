#ifndef __CHUANKOU_H
#define __CHUANKOU_H
#include  <stdio.h>

extern char Rx[];

void CHUANKOU_Send_Init(void);
void Send_Imf(uint8_t Byte);
void Send_String(char *str);
uint32_t Serial_Pow(uint32_t X, uint32_t Y);
void Send_Number(uint32_t Number, uint8_t Length);
int fputc(int ch, FILE *f);
void Serial_Printf(char *format, ...);

void Send_Gstring(uint8_t *str, uint16_t Length);
void chuanKou_SendStream(void);

uint8_t Getflag(void);
uint8_t GetRxData(void);


#endif
