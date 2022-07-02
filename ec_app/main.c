/******************************************************************************
 *  Copyright © 2019 Shenzhen ECIOT Technology Co., Ltd All Rights Reserved
 *-----------------------------------------------------------------------------
 * @file        main.c
 * @brief       app demo
 * @author      mo@eciot.cn (qq:2201920828,v:eciot_mo)
 * @date        2022-02-13
 * @version     1.0
 ******************************************************************************/

#include "main.h"

#ifdef EC_APP_MAIN

#include "ec_core.h"
#include "ec_app_flash.h"
#include "ec_app_ble_peripheral.h"
#include "ec_app_ble.h"
#include "ec_core_adc.h"
#define EC_APP_UART0_TX_BUF_SIZE 1024                 //串口0发送缓冲区大小，可以根据需要调整
#define EC_APP_UART0_RX_BUF_SIZE 1024                 //串口0接收缓冲区大小，可以根据需要调整
#define EC_APP_UART1_TX_BUF_SIZE 1024                 //串口1发送缓冲区大小，可以根据需要调整
#define EC_APP_UART1_RX_BUF_SIZE 1024                 //串口1接收缓冲区大小，可以根据需要调整
uint8_t uart0_tx_buf[EC_APP_UART0_TX_BUF_SIZE] = {0}; //串口0发送缓冲区
uint8_t uart0_rx_buf[EC_APP_UART0_RX_BUF_SIZE] = {0}; //串口0接收缓冲区
uint8_t uart1_tx_buf[EC_APP_UART1_TX_BUF_SIZE] = {0}; //串口1发送缓冲区
uint8_t uart1_rx_buf[EC_APP_UART1_RX_BUF_SIZE] = {0}; //串口1接收缓冲区

void uart0_rx(uint8_t *buf, uint16_t len)
{
    ec_core_ble_send(buf, len); //串口数据转发到蓝牙

    if (strcmp((const char *)buf, "DISC") == 0)
        ec_core_ble_disconnect(); //主动断开蓝牙连接
}

void uart1_rx(uint8_t *buf, uint16_t len) //串口1接收数据中断
{
    ec_core_ble_send(buf, len); //串口数据转发到蓝牙
}

void uart1_init(void) //串口1初始化 波特率精度受时钟频率影响
{
    ec_core_uart_init(EC_CORE_UART1, 115200, EC_CORE_UART_PARITY_NONE,
                      EC_CORE_GPIO_P2, EC_CORE_GPIO_P1,
                      uart1_tx_buf, EC_APP_UART1_TX_BUF_SIZE, uart1_rx_buf, EC_APP_UART1_RX_BUF_SIZE,
                      uart1_rx);
}








static ec_core_sw_timer_e A_timer = EC_CORE_SW_TIMER6;



int a = 0;

uint16_t v1 = 0;
uint16_t v2 = 0;
char sendBuf[100];

int modex = 0;
int adcx = 3200;

int bbc = 0;
int fs[70];
int fsd[70];
int gcc = 0;

static void A_task(void)
{



    //  ec_core_adc_get(EC_CORE_ADC_CH1_P6, EC_CORE_ADC_RANGE_3200MV, EC_CORE_ADC_CALIBRATION_DISABLE, &v1, &v2);
    // sprintf(sendBuf, "%d", gcc);
//   ec_core_ble_send(sendBuf, strlen(sendBuf));

    if(v1 < adcx)
    {

        a = 1 - a;
    }

    if(modex == 0)
    {
        if(a == 0)
        {
            ec_core_gpio_out_init(EC_CORE_GPIO_P1, EC_CORE_GPIO_LEVEL_L);
            ec_core_gpio_out_init(EC_CORE_GPIO_P2, EC_CORE_GPIO_LEVEL_H);
        }
        else
        {

            ec_core_gpio_out_init(EC_CORE_GPIO_P1, EC_CORE_GPIO_LEVEL_H);
            ec_core_gpio_out_init(EC_CORE_GPIO_P2, EC_CORE_GPIO_LEVEL_L);
        }


    }
    else
    {

        ec_core_gpio_out_init(EC_CORE_GPIO_P1, EC_CORE_GPIO_LEVEL_L);
        ec_core_gpio_out_init(EC_CORE_GPIO_P2, EC_CORE_GPIO_LEVEL_L);
    }




}


int diffCount = 0;

#define DIFF  2000

int inRange(int a, int b)
{
    if(a > b)
    {
        if((a - b) < DIFF)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        if((b - a) < DIFF)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
}


#define DIFF2  800

int inRange2(int a, int b)
{
    if(a > b)
    {
        if((a - b) < DIFF2)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        if((b - a) < DIFF)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
}



void rising(void)
{
    if(diffCount == 0)
    {
        return;
    }



    fs[diffCount] = ec_core_hw_timer_get_count(EC_CORE_HW_TIMER1);
    fsd[diffCount - 1] = fs[diffCount - 1] - fs[diffCount];
    diffCount++;

    if((diffCount >= 68))
    {
        ec_core_hw_timer_stop(EC_CORE_HW_TIMER1);
        diffCount = 0;
    }
}

void failling(void)
{


    if(diffCount == 0)
    {
        ec_core_hw_timer_start(EC_CORE_HW_TIMER1, 100000, NULL);
        fs[0] = ec_core_hw_timer_get_count(EC_CORE_HW_TIMER1);
    }
    else
    {
        fs[diffCount] = ec_core_hw_timer_get_count(EC_CORE_HW_TIMER1);
        fsd[diffCount - 1] = fs[diffCount - 1] - fs[diffCount];
    }



    int good = 0;

    if(diffCount == 2)
    {
        if(inRange(fsd[0], 36000))
        {
            if(inRange(fsd[1], 18000))
            {
                good = 1;
            }
        }

        if(good == 0)
        {
            ec_core_hw_timer_stop(EC_CORE_HW_TIMER1);
            diffCount = 0;
            return;
        }
    }
    else if(diffCount == 66)
    {
        int s1 = 0;
        int s2 = 0;

        for(int k = 0; k < 8; k++)
        {
            if(inRange2(fsd[35 + k * 2], 6720))
            {
                s1 += (1 << k);
            }

            if(inRange2(fsd[51 + k * 2], 6720))
            {
                s2 += (1 << k);
            }
        }

        sprintf(sendBuf, "%d,%d", s1, s2);
        ec_core_ble_send(sendBuf, strlen(sendBuf));
    }


    diffCount++;



}



int main(void)
{
    ec_core_sys_clk_set(EC_CORE_SYS_CLK_48M); //配置系统时钟

    ec_app_flash_sys_param_read(); // 从flash读取系统参数
    ec_app_ble_param_init();       // 初始化蓝牙相关的参数

    ec_core_init(); //蓝牙内核初始化
    ec_core_adc_init();
    ec_core_hw_timer_init();



    uint8_t ver[3] = {0};
    ec_core_ver(ver);                                                       //读取软件版本
//    ec_core_uart0_printf("ECB02 SDK %d.%d.%d\r\n", ver[0], ver[1], ver[2]); //串口0 printf打印

    ec_core_sw_watchdog_init(EC_CORE_SW_TIMER6, 2, 3); //初始化软件看门狗，广播超时时间2分钟，蓝牙连接超时时间3分钟

    ec_core_sleep_disable(); //禁止睡眠，串口可以接收数据

    //----------------p2是1， p1是0输出高电平
    ec_core_gpio_out_init(EC_CORE_GPIO_P1, EC_CORE_GPIO_LEVEL_L);
    ec_core_gpio_out_init(EC_CORE_GPIO_P2, EC_CORE_GPIO_LEVEL_L);
//    ec_core_gpio_out_init(EC_CORE_GPIO_P3, EC_CORE_GPIO_LEVEL_L);
//    ec_core_gpio_out_init(EC_CORE_GPIO_P4, EC_CORE_GPIO_LEVEL_L);

    ec_core_gpio_in_init( EC_CORE_GPIO_P9, EC_CORE_GPIO_FLOATING);
    ec_core_gpio_int_register(EC_CORE_GPIO_P9, rising, failling);
    ec_core_sw_timer_start(A_timer, 1000, A_task);

    ec_core_main_loop_run(); //系统主循环


}

#endif
