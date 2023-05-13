# 项目进展

## 需求分析

![项目整体架构需求框架](Picture_source\产品需求文档V1.png)



## 硬件设计

### 外设选型

电池管理：[单节锂离子电池恒流/恒压线性充电器TP4056](https://item.szlcsc.com/772193.html)、[三端输出低压差线性稳压器AMS1117](https://item.szlcsc.com/323882.html)







### 原理图设计





### PCB设计







## 软件设计

### 软件流程逻辑图

![桌面屏幕流程逻辑V1.0](Picture_source\桌面屏幕流程逻辑V1.0.jpg)

### 开发环境

在windows平台上使用VScode IDE通过ssh远程链接Ubuntu虚拟机进行开发。

关于ESP32-IDF物联网开发框架的安装可以参考[乐鑫官网的ESP-IDF编程指南](https://docs.espressif.com/projects/esp-idf/zh_CN/release-v4.4/esp32/get-started/index.html)。



## 更新日志

### 2023.5.9

1. 完成项目架构硬件部分的设计；

### 2023.5.12

1. 暂定项目的软件设计逻辑流程图；
2. 完成开发环境的配置；

### 2023.5.13

1. 修改README文件中图片不显示问题；
2. 添加第一个分支`hello_world`作为初始学习ESP32的初始工程并完成编译，工程内容来自乐鑫提供的官方示例文档；