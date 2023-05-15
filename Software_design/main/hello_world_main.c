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

    xTaskCreate(task1, "task1", 2048, NULL, 10, NULL);
    // 调用创建任务函数，赋予2048字节的堆栈，无参数，优先级为10，创建的函数句柄为NULL

    for (int i = 10; i >= 0; i--)
    {
        printf("Restarting in %d seconds...\n", i);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        // 延迟 1000/1000=1 us，这里在task.h中实际应该有一个宏定义#define portTICK_PERIOD_MS 1000，但是本项目中没有
    }

    ESP_LOGI(TAG, "system init V1.1"); // 写入输出日志信息

    printf("Restarting now.\n");
    fflush(stdout);
    esp_restart();
}
