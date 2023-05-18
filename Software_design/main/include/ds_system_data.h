#ifndef _DS_SYSTEM_DATA_H_
#define _DS_SYSTEM_DATA_H_

#include <stdio.h>
#include <string.h>

#define MAX_SETTING_SSID_LEN 32
#define MAX_SETTING_PSW_LEN 64
// 定义最大的账号和密码长度

typedef enum
{
    SETTING_DATA_INIT = 0,
    SETTING_DATA_HAS_WIFI_INFO,
} SETTING_DATA_E;
// 数据设置返回枚举值

typedef struct
{
    char setting_ssid[MAX_SETTING_SSID_LEN];
    uint8_t setting_ssid_len;
    char setting_psw[MAX_SETTING_PSW_LEN];
    uint8_t setting_psw_len;
} SYSTRM_DATA_T;
// WIFI账号密码信息结构体，包含账号、密码、账号长度、密码长度

//////////////////WIFI INFO MANAGE //////////////////////////////

void set_system_data_wifi_info(char *p_ssid, uint8_t p_ssidlen, char *p_psw, uint8_t p_pswlen);
// 设置WIFI信息

void print_system_data_wifi_info(void);
// 打印WIFI信息

SETTING_DATA_E check_system_data_wifi_info(void);
// 检查WIFI信息的设置情况

SYSTRM_DATA_T get_system_data(void);
//获取WIFI设置信息

void ds_system_data_init(void);
//初始化

#endif
