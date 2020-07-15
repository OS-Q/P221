/*******************************************************************************
****�汾��v1.0.0
****ƽ̨��STM32+EC20
****���ڣ�2020-07-15
****���ߣ�Qitas
****��Ȩ��OS-Q
*******************************************************************************/

#include "delay.h"
#include "sys.h"
#include "led.h"
#include "key.h"
#include "usart.h"
#include "MODBUS.h"
#include "string.h"
#include "usart2.h"
#include "usart3.h"
#include "EC20.h"
#include <stdio.h>
#include <stdlib.h>
#include "timer.h"

#define PRODUCTKEY 		"a1g94F9f4wi"
#define DEVICENAME   	"TEST01"
#define DEVICESECRET    "kvHmPZADuZgn727YObUtYHB8oiKsScp1"

int errcont = 0;
u8 dev_status_flag = 0;	//�豸״̬��Ϣ
u32 sys_time_flag;

void RS485_Send_on(void)
{
    u8 cx[8]={0x01,0x06,0x00,0x0B,0x00,0x01,0x39,0xC8};
    RS485_Send_Data(cx,8);      //����5���ֽ�
}

void RS485_Send_off(void)
{
    u8 cx[8]={0x01,0x06,0x00,0x0B,0x00,0x02,0x79,0xC9};
    RS485_Send_Data(cx,8);      //����5���ֽ�
}
//01 06 00 0B 00 04 B8 09
void RS485_Send_lock(void)
{
    u8 cx[8]={0x01,0x06,0x00,0x0B,0x00,0x03,0xB8,0x09};
    RS485_Send_Data(cx,8);      //����5���ֽ�
}
void RS485_Send_reset(void)
{
    u8 cx[8]={0x01,0x06,0x00,0x0B,0x00,0x04,0xF9,0xCB};
    RS485_Send_Data(cx,8);      //����5���ֽ�
}

int main(void)
{
    u8 cx[8]={0x01,0x03,0x00,0x00,0x00,0x10,0x44,0x06};
    u8 res=1;
    u32 cnt=0;
    errcont = 0;
    NVIC_Configuration(); 	        //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
    TIM3_Int_Init(999,7199);        //10Khz�ļ���Ƶ�ʣ�������999Ϊ100ms
    GPIOINIT();		                //��ʼ����LED���ӵ�Ӳ���ӿ�
    uart_init(115200);	 	        //���ڳ�ʼ��Ϊ115200
    USART2_Init(115200);            //��4Gģ��ͨ��
    RS485_Init(9600);               //��485ģ��ͨ�� �����ʿ��㴫����������
    printf("\r\n ############ https://www.OS-Q.com ############\r\n ########## ("__DATE__ " - " __TIME__ ") ##########");
    delay_init();	    	        //��ʱ������ʼ��
    delay_ms(100);
    MIC29302PWRKEY();
    RESET4G();
    RS485_Send_Data(cx,8);
    // delay_ms(2000);
    // ClearRAM(rs485buf,9);
    while(1)
    {
        while(res)
        {
            res=EC20_INIT();
            delay_ms(2000);
            errcont++;
            printf("���ڳ�������: %d��\r\n",errcont);
            if(errcont > 50)
            {
                NVIC_SystemReset();	//��ʱ����
            }
        }
        errcont = 0;
        res=1;
        while(1)
        {
            while(res)
            {
                res=EC20_CONNECT_SERVER_CFG_INFOR((u8 *)PRODUCTKEY,(u8 *)DEVICENAME,(u8 *)DEVICESECRET);   //���밢����
                errcont++;
                printf("���ڳ�������: %d��\r\n",errcont);
                if(errcont > 50)
                {
                    NVIC_SystemReset();	//��ʱ����
                }
            }
            printf("���������ƽӷ�����\r\n");
            sys_time_flag = 0;
            while(1)
            {
				cnt++;
                if(sys_time_flag%100==0)  //10s�ӷ���һ��
                {
                    if(EC20_MQTT_SEND((u8 *)PRODUCTKEY,(u8 *)DEVICENAME,cnt)==0)
                    {
                        check_cmd();            //���洢�����������������
                        memset(USART2_RX_BUF, 0, sizeof(USART2_RX_BUF));
                        USART2_RX_STA=0;
                        delay_ms(2000);
                    }
                }
                if(USART2_RX_STA&0X8000)		        //���յ�����
                {
                    printf("����2:�յ��������·�����");
                    printf((const char*)USART2_RX_BUF,"\r\n");
                    if(strstr((const char*)USART2_RX_BUF,(const char*)"ON"))
                    {
                        GPIO_ResetBits(GPIOB,GPIO_Pin_12);
                        printf("��·��:��բ\r\n");
                        RS485_Send_on();
                        dev_status_flag = 1;
                    }
                    else if(strstr((const char*)USART2_RX_BUF,(const char*)"OFF"))
                    {
                        GPIO_SetBits(GPIOB,GPIO_Pin_12);
                        printf("��·��:��բ\r\n");
                        RS485_Send_off();
                        dev_status_flag = 2;
                    }
                    else if(strstr((const char*)USART2_RX_BUF,(const char*)"LOCK"))
                    {
                        printf("��·��:����\r\n");
                        RS485_Send_lock();
                        dev_status_flag = 3;
                    }
                    else if(strstr((const char*)USART2_RX_BUF,(const char*)"RESET"))
                    {
                        printf("��·��:����\r\n");
                        RS485_Send_reset();
                        dev_status_flag = 4;
                    }
                    check_cmd();
                    memset(USART2_RX_BUF, 0, sizeof(USART2_RX_BUF));
                    USART2_RX_STA=0;
                }
                if(USART_RX_STA&0X8000)		//���յ����� ���յ�����1������
                {
                    printf("����1:�յ�JSON����");
                    printf((const char*)USART_RX_BUF,"\r\n");
                    if(EC20_MQTT_SEND_DATA((u8 *)PRODUCTKEY,(u8 *)DEVICENAME,(u8 *)USART_RX_BUF)==0)    printf("���ݷ��ͳɹ�\r\n");;//TCP
                    memset(USART_RX_BUF, 0, sizeof(USART_RX_BUF));
                    USART_RX_STA=0;
                    delay_ms(2000);
                }
            }
        }
    }
}

