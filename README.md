# 项目进展

## 需求分析

![项目整体架构需求框架](https://raw.githubusercontent.com/Franch-Toast/Desktop_Small_Screen_Project/hello_world/Picture_source/%E4%BA%A7%E5%93%81%E9%9C%80%E6%B1%82%E6%96%87%E6%A1%A3v1.png)



## 硬件设计

### 外设选型

电池管理：[单节锂离子电池恒流/恒压线性充电器TP4056](https://item.szlcsc.com/772193.html)、[三端输出低压差线性稳压器AMS1117](https://item.szlcsc.com/323882.html)







### 原理图设计





### PCB设计







## 软件设计

### 软件流程逻辑图

![桌面屏幕流程逻辑V1.0](https://raw.githubusercontent.com/Franch-Toast/Desktop_Small_Screen_Project/hello_world/Picture_source/%E6%A1%8C%E9%9D%A2%E5%B1%8F%E5%B9%95%E6%B5%81%E7%A8%8B%E9%80%BB%E8%BE%91V1.0.jpg)

### 开发环境

在windows平台上使用VScode IDE通过ssh远程链接Ubuntu虚拟机进行开发。

关于ESP32-IDF物联网开发框架的安装可以参考[乐鑫官网的ESP-IDF编程指南](https://docs.espressif.com/projects/esp-idf/zh_CN/release-v4.4/esp32/get-started/index.html)。

关于项目的构筑参考[构建系统（CMake 版）](https://docs.espressif.com/projects/esp-idf/zh_CN/release-v4.4/esp32/api-guides/build-system.html?highlight=cmake)，其中包含了整个项目的结构以及CMakeLists的编写方法。


### hello_world分支

从官方`examples`中导出的`hello_world`工程，工程中进行了对芯片信息的打印，并在进行倒数10s后重新启动。

其中`portTICK_PERIOD_MS`在`task.h`中没有进行宏定义，实际应该有宏定义`#define portTICK_PERIOD_MS 1000`。



### Log分支

学习在程序的运行过程中打印日志文件。

参考了ESP32-IDF开发指南中的[System_API--Logging](https://docs.espressif.com/projects/esp-idf/zh_CN/release-v4.4/esp32/api-reference/system/log.html)章节，在此前文件的基础上添加了官方logging部件中的头文件和宏定义`static const char *TAG = "MAIN APP";`标识日志信息，并在`app_main`中输出。



### FreeRTOS分支

学习在程序中添加多任务运行。

使用官方的FreeRTOS组件，简单地创建了一个任务，在任务中实现每秒打印日志。具体的API参考ESP32-IDF开发指南中的[System_API--FreeRTOS](https://docs.espressif.com/projects/esp-idf/zh_CN/release-v4.4/esp32/api-reference/system/freertos.html)。


### Timer分支

根据[Peripherals--Timer](https://docs.espressif.com/projects/esp-idf/zh_CN/release-v4.1/api-reference/peripherals/timer.html)以及[System_API--FreeRTOS](https://docs.espressif.com/projects/esp-idf/zh_CN/release-v4.4/esp32/api-reference/system/freertos.html)（消息队列相关内容在其中）添加定时器功能，并且创建消息队列，防止在中断中执行较复杂的任务形成终端堵塞。

这里的创建的任务也只是使用了基本的功能进行测试，没有实际的逻辑意义，如存入消息队列和取出队列中的消息是没有意义的，这里仅仅做了这个操作动作用来熟悉而已。

定时器的初始化工作：
1. 进行定时器的参数配置；
2. 中断函数的注册和定义；
3. 定时器的使能；

还定义了简单的消息队列，消息队列的发送在中断服务函数中进行，其取出并进行对应的后续操作在任务中实现。


### File_system分支

根据[Storage--SPIFFS](https://docs.espressif.com/projects/esp-idf/zh_CN/release-v4.4/esp32/api-reference/storage/spiffs.html)来编写文件系统，关于spiffs在ESP32的应用可以参考[知乎的一篇文章](https://zhuanlan.zhihu.com/p/115869248)。

该分支实现的功能：为原始分区表添加了`storage`分区，将 SPIFFS 注册并挂载到 VFS，调用`esp_err_t ds_spiffs_init(void);`进行相应的初始化，随后调用`void ds_spiffs_test(void);`测试使用C标准库的文件操作函数进行相应的一系列操作，最后调用`void ds_spiffs_deinit(void);`函数注销。


### NVS分支

根据[Storage--NVS](https://docs.espressif.com/projects/esp-idf/zh_CN/release-v4.4/esp32/api-reference/storage/nvs_flash.html#id10)来编写`ds_nvs.c`文件，在文件中实现初始化、写入、读取、提交等功能。

另外编写有`ds_system_data.c`文件，实现将通过API得到的信息赋值于全局结构体变量，用于后续的操作，该函数在`ds_nvs.c`中调用。

这里使用了`ESP_ERROR_CHECK`宏，功能和`assert`类似，不同之处在于：这个宏会检查 `esp_err_t` 的值，而非判断 bool 条件。如果传给 `ESP_ERROR_CHECK()` 的参数不等于 `ESP_OK` ，则会在控制台上打印错误消息，然后调用 `abort()` 函数。详情可参考[错误处理文档](https://docs.espressif.com/projects/esp-idf/zh_CN/release-v4.4/esp32/api-guides/error-handling.html?highlight=esp_error_check)。


## 更新日志

### 2023.5.9

1. 完成项目架构硬件部分的设计；

### 2023.5.12

1. 暂定项目的软件设计逻辑流程图；
2. 完成开发环境的配置；

### 2023.5.13

1. 修改README文件中图片不显示问题；（未成功解决）
2. 添加第一个分支`hello_world`作为初始学习ESP32的初始工程，工程内容来自乐鑫提供的官方示例文档；

### 2023.5.14

1. 学习为`hello_world`工程添加日志打印功能，该功能参考ESP32-IDF开发指南中的[System_API--Logging](https://docs.espressif.com/projects/esp-idf/zh_CN/release-v4.4/esp32/api-reference/system/log.html)章节；
2. 再次修改`README`文件中图片不显示问题；
3. 添加了`component`部件文件夹，提供文件支持；

### 2023.5.15

1. 在Log分支的基础上尝试使用官方`FreeRTOS`组件实现实时操作系统，添加了一个任务，任务中每一秒打印一条日志，具体的API参考ESP32-IDF开发指南中的[System_API--FreeRTOS](https://docs.espressif.com/projects/esp-idf/zh_CN/release-v4.4/esp32/api-reference/system/freertos.html)。
2. 添加了构筑系统的解释文档，简单地学习了CMakeLists的用法。

### 2023.5.16

1. 重新将此前的`hello_world.c`更改为`app_main.c`，并添加了定时器初始化函数；
2. 编写了`ds_timer.c`和对应的头文件`ds_timer.h`,在其中定义了一个测试用的新任务，测试了定时器功能和消息队列功能；
3. 更新了项目主组件中的`CMakeLists`文件。


### 2023.5.17
1. 添加了扁平的文件系统`SPIFFS`，添加了`ds_spiffs.c`及其头文件`ds_spiffs.h`；
2. 添加了分区表`partitions_example.csv`；
3. 更新了项目主组件中的`CMakeLists`文件。


### 2023.5.18

1. 用非易失性存储 (NVS) 库在 flash 中存储键值格式的数据，也即为WIFI的信息。添加了`ds_system_data.c`、`ds_nvs.c`及其对应的头文件，实现了存储及读取WIFI账号密码的功能。
2. 更新了项目主组件中的`CMakeLists`文件。