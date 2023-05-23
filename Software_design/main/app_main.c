/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h" //使用RTOS需要添加的头文件
#include "freertos/task.h"     //使用RTOS需要添加的头文件
#include "esp_system.h"
#include "esp_spi_flash.h"

#include "ds_timer.h"  //加入timer功能的头文件
#include "ds_spiffs.h" //加入spiffs功能的头文件
#include "ds_nvs.h" //加入NVS功能的头文件
#include "ds_system_data.h"
#include "ds_i2c.h"
#include "ds_ft6336.h"

#include "esp_log.h" //加入comonents中的关于日志的头文件

static const char *TAG = "MAIN APP";  // 定义文件在日志中的标识
static const char *TAG_task = "task"; // 定义任务在日志中的标识

void task1(void *pvParameters)
{
    while (1)
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        ESP_LOGI(TAG_task, "task1 run."); // 写入输出任务信息
    }
    // 注意，如果任务不能循环则需要调用删除函数vTaskDelete
}

void app_main(void)
{
    printf("Hello world!\n");

    /* Print chip information */
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info); // 获取芯片的信息，并在下面打印
    printf("This is %s chip with %d CPU core(s), WiFi%s%s, ",
           CONFIG_IDF_TARGET,
           chip_info.cores,
           (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
           (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

    printf("silicon revision %d, ", chip_info.revision);

    printf("%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
           (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    printf("Minimum free heap size: %d bytes\n", esp_get_minimum_free_heap_size());

    ds_timer_init(); // 添加了在ds_timer.c中自定义的定时器初始化函数

    ds_spiffs_init();   // 文件系统初始化
    ds_spiffs_test();   // 文件系统功能测试
    ds_spiffs_deinit(); // 文件系统的注销

    char *ssid = "leo";
    char *psw = "123456789";
    set_system_data_wifi_info(ssid, strlen(ssid), psw, strlen(psw));
    ds_nvs_init();
    ds_nvs_save_wifi_info();
    ds_nvs_read_wifi_info();

    xTaskCreate(task1, "task1", 2048, NULL, 10, NULL);
    // 调用创建任务函数，赋予2048字节的堆栈，无参数，优先级为10，创建的函数句柄为NULL

    TP_POSITION_T position;

    init_ft6336();

    while (1)
    {
        // printf("system run ...\n");
        get_ft6336_touch_sta(&position);
        vTaskDelay(10 / portTICK_PERIOD_MS);
        // 延迟 1000/1000=1 us，这里在task.h中实际应该有一个宏定义#define portTICK_PERIOD_MS 1000，但是本项目中没有
    }
}
