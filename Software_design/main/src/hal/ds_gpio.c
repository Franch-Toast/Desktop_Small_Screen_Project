
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

#include "ds_gpio.h"
#include "ds_system_data.h"

/**
 * GPIO status:
 * GPIO5: output
 * GPIO34:  input, pulled up, interrupt from rising edge 输入、上升、上升沿中断
 */

#define GPIO_OUTPUT_IO_0 5 // TP复位引脚
#define GPIO_OUTPUT_PIN_SEL ((1ULL << GPIO_OUTPUT_IO_0))
#define GPIO_INPUT_IO_0 4 // TP中断引脚
#define GPIO_INPUT_PIN_SEL ((1ULL << GPIO_INPUT_IO_0))
#define ESP_INTR_FLAG_DEFAULT 0

// 屏幕片选 0-有效
#define SCREEN_GPIO_OUTPUT_CS 27
#define SCREEN_GPIO_OUTPUT_CS_SEL ((1ULL << SCREEN_GPIO_OUTPUT_CS))
// 屏幕数据/指令选择 1-data 0-cmd
#define SCREEN_GPIO_OUTPUT_DC 14
#define SCREEN_GPIO_OUTPUT_DC_SEL ((1ULL << SCREEN_GPIO_OUTPUT_DC))
// 屏幕复位 0-reset
#define SCREEN_GPIO_OUTPUT_RES 12
#define SCREEN_GPIO_OUTPUT_RES_SEL ((1ULL << SCREEN_GPIO_OUTPUT_RES))
// 屏幕状态 1-busy
#define SCREEN_GPIO_INTPUT_BUSY 13
#define SCREEN_GPIO_INTPUT_BUSY_SEL ((1ULL << SCREEN_GPIO_INTPUT_BUSY))

static xQueueHandle gpio_evt_queue = NULL;

static void IRAM_ATTR gpio_isr_handler(void *arg)
{
    uint32_t gpio_num = (uint32_t)arg;                  // 获取中断的GPIO端口号
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL); // 向GPIO消息队列中发送端口号
}

static void gpio_task_example(void *arg)
{
    uint32_t io_num;
    for (;;)
    {
        if (xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY)) // 获取消息队列中中断触发的GPIO端口号
        {
            printf("GPIO[%d] intr, val: %d\n", io_num, gpio_get_level(io_num)); // 打印该端口的信息
        }
    }
}

void ds_touch_gpio_init() // 触摸屏GPIO初始化
{
    gpio_config_t io_conf; // GPIO配置结构体
    // disable interrupt
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE; // 失能中断
    // set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    // bit mask of the pins that you want to set,present:GPIO5
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    // disable pull-down mode，取消下拉和上拉
    io_conf.pull_down_en = 0;
    // disable pull-up mode
    io_conf.pull_up_en = 0;
    // configure GPIO with the given settings
    gpio_config(&io_conf);

    // 接下来配置GPIO34
    //  GPIO interrupt type : both rising and falling edge
    io_conf.intr_type = GPIO_INTR_ANYEDGE;
    // bit mask of the pins, use GPIO4/5 here
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    // set as input mode
    io_conf.mode = GPIO_MODE_INPUT;
    // enable pull-up mode
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);

    // change gpio intrrupt type for one pin
    //  gpio_set_intr_type(GPIO_INPUT_IO_0, GPIO_INTR_NEGEDGE);

    // create a queue to handle gpio event from isr
    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    // start gpio task
    xTaskCreate(gpio_task_example, "gpio_task_example", 2048, NULL, 10, NULL);

    // install gpio isr service
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT); // 安装驱动程序的 GPIO ISR 处理程序服务，该服务允许每引脚 GPIO 中断处理程序。
    // hook isr handler for specific gpio pin
    gpio_isr_handler_add(GPIO_INPUT_IO_0, gpio_isr_handler, (void *)GPIO_INPUT_IO_0); // 为相应的 GPIO 引脚添加 ISR 处理程序。
}

void ds_screen_gpio_init() // 显示屏GPIO初始化
{
    gpio_config_t io_conf;
    // disable interrupt
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    // set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    // bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = SCREEN_GPIO_OUTPUT_CS_SEL;
    // disable pull-down mode
    io_conf.pull_down_en = 0;
    // disable pull-up mode
    io_conf.pull_up_en = 0;
    // configure GPIO with the given settings
    gpio_config(&io_conf);

    // bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = SCREEN_GPIO_OUTPUT_DC_SEL;
    // configure GPIO with the given settings
    gpio_config(&io_conf);

    // bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = SCREEN_GPIO_OUTPUT_RES_SEL;
    // configure GPIO with the given settings
    gpio_config(&io_conf);

    io_conf.intr_type = GPIO_INTR_NEGEDGE; // 下降沿触发
    // bit mask of the pins, use GPIO4/5 here
    io_conf.pin_bit_mask = SCREEN_GPIO_INTPUT_BUSY_SEL;
    // set as input mode
    io_conf.mode = GPIO_MODE_INPUT;
    // enable pull-up mode
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);
    // change gpio intrrupt type for one pin
    //  gpio_set_intr_type(SCREEN_GPIO_INTPUT_BUSY, GPIO_INTR_NEGEDGE);

    // install gpio isr service
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    // hook isr handler for specific gpio pin
    gpio_isr_handler_add(SCREEN_GPIO_INTPUT_BUSY, gpio_isr_handler, (void *)SCREEN_GPIO_INTPUT_BUSY);
}

void ds_gpio_init()
{
    ds_touch_gpio_init(); // 触摸屏GPIO初始化
}

void ds_gpio_set_screen_cs(uint32_t level)
{
    gpio_set_level(SCREEN_GPIO_OUTPUT_CS, level); // GPIO将屏幕片选设置高低电平，即是否选择屏幕
}

void ds_gpio_set_screen_dc(uint32_t level)
{
    gpio_set_level(SCREEN_GPIO_OUTPUT_DC, level); // GPIO将屏幕数据/指令操作设置高低电平，即是数据还是指令
}

void ds_gpio_set_screen_rst(uint32_t level)
{
    gpio_set_level(SCREEN_GPIO_OUTPUT_RES, level); // 复位
}

int ds_gpio_get_screen_busy()
{
    return gpio_get_level(SCREEN_GPIO_INTPUT_BUSY); // 繁忙
}

void ds_gpio_set_touch_rst(uint32_t level)
{
    gpio_set_level(GPIO_OUTPUT_IO_0, level); // 触摸屏复位
}
