#ifndef __LED_H
#define __LED_H	 
#include "sys.h"
//绿灯PC0
//4G模块供电PC1
//4G模块复位PB1

#define RELAY_IO        GPIOB   //继电器PB12
#define Relay_B      	GPIO_Pin_12
void GPIOINIT(void);	//初始化
void RESET4G(void);
void MIC29302PWRKEY(void);
#endif


