#ifndef __LED_H
#define __LED_H	 
#include "sys.h"
//�̵�PC0
//4Gģ�鹩��PC1
//4Gģ�鸴λPB1

#define RELAY_IO        GPIOB   //�̵���PB12
#define Relay_B      	GPIO_Pin_12
void GPIOINIT(void);	//��ʼ��
void RESET4G(void);
void MIC29302PWRKEY(void);
#endif


