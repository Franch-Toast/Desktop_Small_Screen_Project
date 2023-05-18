
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"

#include "ds_nvs.h"
#include "ds_system_data.h"

static const char *TAG = "ds_nvs";

NVS_WIFI_INFO_E wifi_config_flag = NVS_WIFI_INFO_NULL;

void ds_nvs_save_wifi_info() // 以键值对形式存储WIFI账号密码信息
{
    esp_err_t err;
    nvs_handle_t nvs_handle;
    err = nvs_open("wificonfig", NVS_READWRITE, &nvs_handle); // 使用默认 NVS 分区中的给定命名空间打开非易失性存储
    if (err != ESP_OK)
    {
        ESP_LOGI(TAG, "Error (%s) opening NVS handle!\n", esp_err_to_name(err));
        return;
    }
    wifi_config_flag = NVS_WIFI_INFO_SAVE; // 更改WIFI配置状态标识位为定义的枚举值2
    // 下面的句柄nvs_handle都是由nvs_open得到的，且不能使用只读打开的句柄
    ESP_ERROR_CHECK(nvs_set_u8(nvs_handle, "wifi_flag", wifi_config_flag));           // 为给定键(wifi_flag)设置uint8_t值(2)
    ESP_ERROR_CHECK(nvs_set_str(nvs_handle, "ssid", get_system_data().setting_ssid)); // 为给定键(ssid)设置字符串
    ESP_ERROR_CHECK(nvs_set_str(nvs_handle, "psw", get_system_data().setting_psw));   // 为给定键(psw)设置字符串
    ESP_ERROR_CHECK(nvs_commit(nvs_handle));                                          // 将任何挂起的更改写入非易失性存储。
    nvs_close(nvs_handle);                                                            // 关闭存储句柄并释放所有分配的资源。
}

NVS_WIFI_INFO_E ds_nvs_read_wifi_info() // 读取账号和密码信息
{
    esp_err_t err;
    nvs_handle_t nvs_handle;
    err = nvs_open("wificonfig", NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK)
    {
        ESP_LOGI(TAG, "Error (%s) opening NVS handle!\n", esp_err_to_name(err));
        return NVS_WIFI_INFO_ERROR;
    }
    uint8_t wifi_config = 0;
    ESP_ERROR_CHECK(nvs_get_u8(nvs_handle, "wifi_flag", &wifi_config)); // 获取WIFI配置标志位
    wifi_config_flag = wifi_config;
    if (wifi_config_flag == NVS_WIFI_INFO_SAVE) // 如果标志位为已经存储信息
    {
        ESP_LOGI(TAG, "has wifi config info");
        char ssid[32];
        char psw[64];
        size_t ssid_len = sizeof(ssid);
        size_t psw_len = sizeof(psw);
        ESP_ERROR_CHECK(nvs_get_str(nvs_handle, "ssid", ssid, &ssid_len));
        ESP_ERROR_CHECK(nvs_get_str(nvs_handle, "psw", psw, &psw_len));
        set_system_data_wifi_info(ssid, ssid_len, psw, psw_len); // 把信息填充给到全局变量g_system_data中
        print_system_data_wifi_info();                           // 打印信息
    }
    else
    {
        ESP_LOGI(TAG, "wifi config info null");
    }
    nvs_close(nvs_handle);
    return wifi_config_flag; // 返回配置标志位
}

void ds_nvs_init()
{
    // Initialize NVS
    esp_err_t err = nvs_flash_init(); // 默认 NVS 分区是在分区表中标记为“nvs”的分区。
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase()); // 擦除默认 NVS 分区。
        err = nvs_flash_init();             // 需要再次初始化分区才可以使用
    }
    ESP_ERROR_CHECK(err);
}
