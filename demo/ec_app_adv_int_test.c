/******************************************************************************
 *  Copyright © 2019 Shenzhen ECIOT Technology Co., Ltd All Rights Reserved
 *-----------------------------------------------------------------------------
 * @file        ec_app_adv_int_test.c
 * @brief       advert interval demo
 * @author      mo@eciot.cn (qq:2201920828,v:eciot_mo)
 * @date        2022-02-13
 * @version     1.0
 ******************************************************************************/

#include "main.h"

#ifdef EC_APP_ADV_INT_TEST

#include "ec_core.h"

#define EC_APP_UART0_TX_BUF_SIZE 1024                 //串口0发送缓冲区大小，可以根据需要调整
#define EC_APP_UART0_RX_BUF_SIZE 1024                 //串口0接收缓冲区大小，可以根据需要调整
uint8_t uart0_tx_buf[EC_APP_UART0_TX_BUF_SIZE] = {0}; //串口0发送缓冲区
uint8_t uart0_rx_buf[EC_APP_UART0_RX_BUF_SIZE] = {0}; //串口0接收缓冲区

void uart0_rx(uint8_t *buf, uint16_t len) //串口0接收数据中断
{
    if (strcmp((char *)buf, "AA") == 0) //设置广播间隙为20ms，重新广播后生效。
    {
        ec_core_ble_peripheral_set_adv_int(32); // 20ms
        ec_core_ble_peripheral_reset_adv();
        ec_core_uart0_printf("set adv int 20ms\r\n");
    }
    if (strcmp((char *)buf, "BB") == 0) //设置广播间隙为1秒，重新广播后生效。
    {
        ec_core_ble_peripheral_set_adv_int(1600); // 1000ms
        ec_core_ble_peripheral_reset_adv();
        ec_core_uart0_printf("set adv int 1000ms\r\n");
    }
    if (strcmp((char *)buf, "CC") == 0) //设置广播间隙为200ms，重新广播后生效。系统默认为200ms
    {
        ec_core_ble_peripheral_set_adv_int(320); // 200ms
        ec_core_ble_peripheral_reset_adv();
        ec_core_uart0_printf("set adv int 200ms\r\n");
    }
}

void uart0_init(void) //串口0初始化 波特率精度受时钟频率影响
{
    ec_core_uart_init(EC_CORE_UART0, 115200, EC_CORE_UART_PARITY_NONE,
                      EC_CORE_GPIO_P4, EC_CORE_GPIO_P5,
                      uart0_tx_buf, EC_APP_UART0_TX_BUF_SIZE, uart0_rx_buf, EC_APP_UART0_RX_BUF_SIZE,
                      uart0_rx);
}

int main(void)
{
    ec_core_sys_clk_set(EC_CORE_SYS_CLK_48M); //系统时钟初始化

    ec_core_ble_peripheral_set_adv_int(320); // 初始化蓝牙广播间隙为200ms

    ec_core_init();                                  //蓝牙内核初始化
    uart0_init();                                    //串口0初始化 波特率精度受时钟频率影响
    ec_core_uart0_printf("set bt adv int test\r\n"); //串口0 printf打印
    ec_core_sleep_disable();                         //禁止睡眠，串口可以接收数据
    ec_core_main_loop_run();                         //系统主循环
}

#endif
