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