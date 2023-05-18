
#include "sdkconfig.h"
#include "esp_system.h"
#include "esp_log.h"

#include "ds_system_data.h"//需要包含"string.h"

static const char *TAG = "ds_system_data";

SYSTRM_DATA_T g_system_data;

void ds_system_data_init()
{
    memset(&g_system_data, 0, sizeof(SYSTRM_DATA_T)); // 将信息全部清空填充为0，进行初始化
    // 需要包含"string.h"
}

SYSTRM_DATA_T get_system_data()
{
    return g_system_data;
}

void set_system_data_wifi_info(char *p_ssid, uint8_t p_ssidlen, char *p_psw, uint8_t p_pswlen)
{
    if (p_pswlen >= MAX_SETTING_SSID_LEN || p_ssidlen >= MAX_SETTING_PSW_LEN)
    {
        ESP_LOGE(TAG, "MAX_SETTING_SSID_PWD_LEN ERROR");
    }
    g_system_data.setting_ssid_len = p_ssidlen;
    g_system_data.setting_psw_len = p_pswlen;
    memcpy(g_system_data.setting_ssid, p_ssid, p_ssidlen);//内存块复制
    memcpy(g_system_data.setting_psw, p_psw, p_pswlen);
    g_system_data.setting_ssid[p_ssidlen] = '\0'; // 记得要填充字符串结束字符'\0'
    g_system_data.setting_psw[p_pswlen] = '\0';
}

void print_system_data_wifi_info()
{
    printf("\r\nwifi_ssid:");
    for (int i = 0; i < g_system_data.setting_ssid_len; i++)
    {
        printf("%c", g_system_data.setting_ssid[i]);
    }

    printf("\r\nwifi_password:");
    for (int i = 0; i < g_system_data.setting_psw_len; i++)
    {
        printf("%c", g_system_data.setting_psw[i]);
    }
    printf("\r\n");
}

SETTING_DATA_E check_system_data_wifi_info()
{
    if (g_system_data.setting_ssid_len != 0)
    {
        return SETTING_DATA_HAS_WIFI_INFO;
    }
    return SETTING_DATA_INIT;
} // 查看当前WIFI信息的设置状态，如果长度不为0则返回已经设置，否则返回当前为初始化状态
