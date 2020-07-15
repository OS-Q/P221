/*******************************************************************************
****版本：v1.0.0
****平台：STM32+EC20
****日期：2020-07-15
****作者：Qitas
****版权：OS-Q
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
u8 dev_status_flag = 0;	//设备状态信息
u32 sys_time_flag;

void RS485_Send_on(void)
{
    u8 cx[8]={0x01,0x06,0x00,0x0B,0x00,0x01,0x39,0xC8};
    RS485_Send_Data(cx,8);      //发送5个字节
}

void RS485_Send_off(void)
{
    u8 cx[8]={0x01,0x06,0x00,0x0B,0x00,0x02,0x79,0xC9};
    RS485_Send_Data(cx,8);      //发送5个字节
}
//01 06 00 0B 00 04 B8 09
void RS485_Send_lock(void)
{
    u8 cx[8]={0x01,0x06,0x00,0x0B,0x00,0x03,0xB8,0x09};
    RS485_Send_Data(cx,8);      //发送5个字节
}
void RS485_Send_reset(void)
{
    u8 cx[8]={0x01,0x06,0x00,0x0B,0x00,0x04,0xF9,0xCB};
    RS485_Send_Data(cx,8);      //发送5个字节
}

int main(void)
{
    u8 cx[8]={0x01,0x03,0x00,0x00,0x00,0x10,0x44,0x06};
    u8 res=1;
    u32 cnt=0;
    errcont = 0;
    NVIC_Configuration(); 	        //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
    TIM3_Int_Init(999,7199);        //10Khz的计数频率，计数到999为100ms
    GPIOINIT();		                //初始化与LED连接的硬件接口
    uart_init(115200);	 	        //串口初始化为115200
    USART2_Init(115200);            //与4G模块通信
    RS485_Init(9600);               //与485模块通信 波特率看你传感器而定义
    printf("\r\n ############ https://www.OS-Q.com ############\r\n ########## ("__DATE__ " - " __TIME__ ") ##########");
    delay_init();	    	        //延时函数初始化
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
            printf("正在尝试连接: %d次\r\n",errcont);
            if(errcont > 50)
            {
                NVIC_SystemReset();	//超时重启
            }
        }
        errcont = 0;
        res=1;
        while(1)
        {
            while(res)
            {
                res=EC20_CONNECT_SERVER_CFG_INFOR((u8 *)PRODUCTKEY,(u8 *)DEVICENAME,(u8 *)DEVICESECRET);   //接入阿里云
                errcont++;
                printf("正在尝试连接: %d次\r\n",errcont);
                if(errcont > 50)
                {
                    NVIC_SystemReset();	//超时重启
                }
            }
            printf("已连阿里云接服务器\r\n");
            sys_time_flag = 0;
            while(1)
            {
				cnt++;
                if(sys_time_flag%100==0)  //10s钟发送一次
                {
                    if(EC20_MQTT_SEND((u8 *)PRODUCTKEY,(u8 *)DEVICENAME,cnt)==0)
                    {
                        check_cmd();            //不存储到串口数组清空数据
                        memset(USART2_RX_BUF, 0, sizeof(USART2_RX_BUF));
                        USART2_RX_STA=0;
                        delay_ms(2000);
                    }
                }
                if(USART2_RX_STA&0X8000)		        //接收到数据
                {
                    printf("串口2:收到服务器下发数据");
                    printf((const char*)USART2_RX_BUF,"\r\n");
                    if(strstr((const char*)USART2_RX_BUF,(const char*)"ON"))
                    {
                        GPIO_ResetBits(GPIOB,GPIO_Pin_12);
                        printf("断路器:合闸\r\n");
                        RS485_Send_on();
                        dev_status_flag = 1;
                    }
                    else if(strstr((const char*)USART2_RX_BUF,(const char*)"OFF"))
                    {
                        GPIO_SetBits(GPIOB,GPIO_Pin_12);
                        printf("断路器:分闸\r\n");
                        RS485_Send_off();
                        dev_status_flag = 2;
                    }
                    else if(strstr((const char*)USART2_RX_BUF,(const char*)"LOCK"))
                    {
                        printf("断路器:锁定\r\n");
                        RS485_Send_lock();
                        dev_status_flag = 3;
                    }
                    else if(strstr((const char*)USART2_RX_BUF,(const char*)"RESET"))
                    {
                        printf("断路器:解锁\r\n");
                        RS485_Send_reset();
                        dev_status_flag = 4;
                    }
                    check_cmd();
                    memset(USART2_RX_BUF, 0, sizeof(USART2_RX_BUF));
                    USART2_RX_STA=0;
                }
                if(USART_RX_STA&0X8000)		//接收到数据 接收到串口1的数据
                {
                    printf("串口1:收到JSON数据");
                    printf((const char*)USART_RX_BUF,"\r\n");
                    if(EC20_MQTT_SEND_DATA((u8 *)PRODUCTKEY,(u8 *)DEVICENAME,(u8 *)USART_RX_BUF)==0)    printf("数据发送成功\r\n");;//TCP
                    memset(USART_RX_BUF, 0, sizeof(USART_RX_BUF));
                    USART_RX_STA=0;
                    delay_ms(2000);
                }
            }
        }
    }
}

