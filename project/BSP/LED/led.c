#include "led.h"
#include "delay.h"

//绿灯PC0
//4G模块供电PC1
//4G模块复位PB1
void GPIOINIT(void)
{

    GPIO_InitTypeDef  GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD  | RCC_APB2Periph_GPIOE, ENABLE);	 //使能PA,PD端口时钟
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_SetBits(GPIOC,GPIO_Pin_0);                                  //关闭绿灯
    GPIO_ResetBits(GPIOC,GPIO_Pin_1);                                //开启4G模块电源

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB,&GPIO_InitStructure);

    GPIO_SetBits(RELAY_IO,Relay_B);     //高电平关闭继电器
    GPIO_ResetBits(GPIOB,GPIO_Pin_1);   //控制模块复位
}

void MIC29302PWRKEY(void)
{
    GPIO_SetBits(GPIOC,GPIO_Pin_1);		    //关闭模块电源
    delay_ms(200);
    GPIO_ResetBits(GPIOC,GPIO_Pin_1);       //开启4G模块电源
    delay_ms(300);
}

void RESET4G(void)
{
    GPIO_SetBits(GPIOB,GPIO_Pin_1);		        //高电平复位
    delay_ms(2000);
    GPIO_ResetBits(GPIOB,GPIO_Pin_1);           //低电平工作
    delay_ms(300);
}
