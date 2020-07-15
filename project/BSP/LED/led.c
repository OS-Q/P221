#include "led.h"
#include "delay.h"

//�̵�PC0
//4Gģ�鹩��PC1
//4Gģ�鸴λPB1
void GPIOINIT(void)
{

    GPIO_InitTypeDef  GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD  | RCC_APB2Periph_GPIOE, ENABLE);	 //ʹ��PA,PD�˿�ʱ��
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_SetBits(GPIOC,GPIO_Pin_0);                                  //�ر��̵�
    GPIO_ResetBits(GPIOC,GPIO_Pin_1);                                //����4Gģ���Դ

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB,&GPIO_InitStructure);

    GPIO_SetBits(RELAY_IO,Relay_B);     //�ߵ�ƽ�رռ̵���
    GPIO_ResetBits(GPIOB,GPIO_Pin_1);   //����ģ�鸴λ
}

void MIC29302PWRKEY(void)
{
    GPIO_SetBits(GPIOC,GPIO_Pin_1);		    //�ر�ģ���Դ
    delay_ms(200);
    GPIO_ResetBits(GPIOC,GPIO_Pin_1);       //����4Gģ���Դ
    delay_ms(300);
}

void RESET4G(void)
{
    GPIO_SetBits(GPIOB,GPIO_Pin_1);		        //�ߵ�ƽ��λ
    delay_ms(2000);
    GPIO_ResetBits(GPIOB,GPIO_Pin_1);           //�͵�ƽ����
    delay_ms(300);
}
