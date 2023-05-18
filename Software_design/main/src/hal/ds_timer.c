
#include <stdio.h>
#include "esp_types.h"
#include "ds_timer.h"
#include "freertos/FreeRTOS.h" //使用RTOS需要添加的头文件
#include "freertos/task.h"     //使用RTOS需要添加的头文件
#include "freertos/queue.h"
#include "driver/periph_ctrl.h"
#include "driver/timer.h"
#include "hal/timer_types.h"
#include "esp_log.h"

static const char *TAG = "TIMER APP";

#define TIMER_DIVIDER 16                                    //  Hardware timer clock divider
#define TIMER_SCALE (TIMER_BASE_CLK / TIMER_DIVIDER / 1000) // convert counter value to ms seconds
#define TIMER_INTERVAL0_SEC (10)                            // sample test interval for the first timer
#define TEST_WITH_RELOAD 1                                  // testing will be done with auto reload

/*
 * A sample structure to pass events
 * from the timer interrupt handler to the main program.
 */
typedef struct
{
    uint64_t timer_minute_count;
    uint64_t timer_second_count;
} timer_event_t;

timer_event_t g_timer_event;

xQueueHandle timer_queue;

static void example_tg0_timer_init(int timer_idx,
                                   bool auto_reload, double timer_interval_sec);
{
    /* Select and initialize basic parameters of the timer */
    timer_config_t config = {
        .divider = TIMER_DIVIDER,                  // 16分频 80 MHz APB_CLK 时钟
        .counter_dir = TIMER_COUNT_UP,             // 向上递增计时
        .counter_en = TIMER_PAUSE,                 // 暂停使能
        .alarm_en = TIMER_ALARM_EN,                // 报警使能
        .auto_reload = auto_reload,                // 自动重新加载
    };                                             // default clock source is APB
    timer_init(TIMER_GROUP_0, timer_idx, &config); // 配置好参数后初始化计时器组0的0号计时器

    /* Timer's counter will initially start from value below.
       Also, if auto_reload is set, this value will be automatically reload on alarm */
    timer_set_counter_value(TIMER_GROUP_0, timer_idx, 0x00000000ULL);

    /* Configure the alarm value and the interrupt on alarm. */
    timer_set_alarm_value(TIMER_GROUP_0, timer_idx, timer_interval_sec * TIMER_SCALE);
    timer_enable_intr(TIMER_GROUP_0, timer_idx); // 使能中断
    timer_isr_register(TIMER_GROUP_0, timer_idx, timer_group0_isr,
                       (void *)timer_idx, ESP_INTR_FLAG_IRAM, NULL);

    timer_start(TIMER_GROUP_0, timer_idx); // 此前初始化时TIMER_PAUSE没有使能，在这里配置完成后使能
}

void IRAM_ATTR timer_group0_isr(void *para)//中断服务函数
{
    timer_spinlock_take(TIMER_GROUP_0); // 由于使用了ESP_INTR_FLAG_IRAM，需要添加定时器自旋锁
    int timer_idx = (int)para;

    // /* Prepare basic event data
    //    that will be then sent back to the main program task */
    timer_event_t evt;

    timer_group_clr_intr_status_in_isr(TIMER_GROUP_0, TIMER_0);//清除标志位，仅用于ISR中

    /* After the alarm has been triggered
      we need enable it again, so it is triggered the next time */
    timer_group_enable_alarm_in_isr(TIMER_GROUP_0, timer_idx);//重新使能中断

    /* Now just send the event data back to the main program task */
    xQueueSendFromISR(timer_queue, &evt, NULL);//将消息发入消息队列中
    timer_spinlock_give(TIMER_GROUP_0);//解锁
}

static void timer_example_evt_task(void *arg)//任务函数
{
    while (1)
    {
        timer_event_t evt;
        xQueueReceive(timer_queue, &evt, portMAX_DELAY);//获取消息队列的队首消息
        //中断触发一次的时间是 10ms
        g_timer_event.timer_minute_count++;
        // 60s 计算一次 刷新时间
        if (g_timer_event.timer_minute_count >= 6000)
        {
            g_timer_event.timer_minute_count = 0;
        }
        g_timer_event.timer_second_count++;
        // 1s计算一次
        if (g_timer_event.timer_second_count >= 100)
        {
            g_timer_event.timer_second_count = 0;
            ESP_LOGI(TAG, "1s run ");
        }
    }
}

// 使用timer group0定时器组的硬件定时器htimer0 和 timer1
void ds_timer_init(void)
{
    g_timer_event.timer_minute_count = 0;
    g_timer_event.timer_second_count = 0;
    timer_queue = xQueueCreate(10, sizeof(timer_event_t));                      // 创建消息队列
    example_tg0_timer_init(TIMER_0, TEST_WITH_RELOAD, TIMER_INTERVAL0_SEC);     // 初始化定时器0
    xTaskCreate(timer_example_evt_task, "timer_evt_task", 2048, NULL, 5, NULL); // 创建任务，优先级为5
}