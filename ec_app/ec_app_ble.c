/******************************************************************************
 *  Copyright © 2019 Shenzhen ECIOT Technology Co., Ltd All Rights Reserved
 *-----------------------------------------------------------------------------
 * @file        ec_app_ble.c
 * @brief       ble control
 * @author      mo@eciot.cn (qq:2201920828,v:eciot_mo)
 * @date        2022-02-13
 * @version     1.0
 ******************************************************************************/

#include "ec_core.h"
#include "ec_app_ble_peripheral.h"

void ec_app_ble_param_init(void) //蓝牙参数初始化
{
    // ec_core_ble_set_power(EC_CORE_BLE_POWER_3DBM);//配置发射功率，默认3dbm

    // 配置UUID
    // 16bit uuid
    // ec_core_ble_set_suuid("FFF0");
    // ec_core_ble_set_ruuid("FFF1");
    // ec_core_ble_set_wuuid("FFF2");
    // 128bit uuid
    ec_core_ble_set_suuid("59462F12-9543-9999-12C8-58B459A2712D");
    ec_core_ble_set_ruuid("5C3A659E-897E-45E1-B016-007107C96DF6");
  //  ec_core_ble_set_wuuid("5C3A659E-897E-45E1-B016-007107C96DF6");

    ec_core_ble_peripheral_set_ota_en(ec_app_ble_peripheral_ota_en); //打开或关闭OTA升级

    uint8_t mac[6] = {0};
    ec_core_ble_get_mac(mac); //获取MAC地址
    char buf[25] = {0};
    sprintf(buf, "BT_%02X%02X%02X%02X%02X%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    ec_core_ble_peripheral_set_name((uint8_t *)buf, 15); //根据MAC地址修改蓝牙名字
    ec_core_ble_peripheral_set_name("BT_123", strlen("BT_123"));

    ec_app_ble_peripheral_register_event(); //注册蓝牙回调
}
