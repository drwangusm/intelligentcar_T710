/**
 * @file collection.cpp
 * @author Leo ()
 * @brief 遥控图像采集Demo
 * @version 0.1
 * @date 2022-03-13
 *
 * @copyright Copyright (c) 2022
 * @note 遥控采图步骤：
 *                  [01] 启动OpenCV摄像头图像捕获
 *                  [02] 创建遥控手柄多线程任务
 *                  [03] 智能车速度与方向控制
 *                  [04] 图像显示与存储
 */
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>
#include <string>
#include <sstream>
#include <unistd.h>
#include <signal.h>
#include "../include/uart.hpp"
#include "../include/common.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <cstdlib>
#include <sys/stat.h>
#include <pthread.h>

using namespace std;
using namespace cv;

#define JOYSTICK_MODE 3      // 遥控手柄模式（1/有线手柄  2/无线手柄A（单绿灯常亮）   3/无线手柄B（单红灯常亮））
#define JS_EVENT_BUTTON 0x01 /* button pressed/released */
#define JS_EVENT_AXIS 0x02   /* joystick moved */
#define JS_EVENT_INIT 0x80   /* initial state of device */

struct EventJoy
{
    unsigned int time;    // 手柄触发时间：ms
    short value;          // 操控值
    unsigned char type;   // 操控类型：按键/摇杆
    unsigned char number; // 编号
};

std::shared_ptr<Driver> driver = nullptr; // 初始化串口驱动
bool collectMoreEnable = false;           // 连续图像采样使能
bool collectOnceEnable = false;           // 单次图像采样使能
float speed = 0;                          // 车速：m/s
float steer = PWMSERVOMID;                // 打舵：PWM
bool dirFront = true;                     // 车辆速度方向:默认向前
bool uartSendEnable = false;              // 串口发送使能
pthread_t threadId[1];                    // 定义线程的Id
void callbackSignal(int signum);
void *threadJoystick(void *args);
struct stat buffer;
int main(int argc, char const *argv[])
{
    // USB转串口的设备名为/dev/ttyUSB0
    driver = std::make_shared<Driver>("/dev/ttyUSB0", BaudRate::BAUD_115200);
    if (driver == nullptr)
    {
        std::cout << "Create Uart-Driver Error!" << std::endl;
        return -1;
    }
    // 串口初始化，打开串口设备及配置串口数据格式
    int ret = driver->open();
    if (ret != 0)
    {
        std::cout << "Uart Open failed!" << std::endl;
        return -1;
    }

    // 摄像头初始化
    VideoCapture capture(0);
    if (!capture.isOpened())
    {
        std::cout << "can not open video device " << std::endl;
        return 1;
    }
    capture.set(cv::CAP_PROP_FRAME_WIDTH, COLSIMAGE);
    capture.set(cv::CAP_PROP_FRAME_HEIGHT, ROWSIMAGE);

    double rate = capture.get(CAP_PROP_FPS);
    double width = capture.get(CAP_PROP_FRAME_WIDTH);
    double height = capture.get(CAP_PROP_FRAME_HEIGHT);
    std::cout << "Camera Param: frame rate = " << rate << " width = " << width
              << " height = " << height << std::endl;

    signal(SIGINT, callbackSignal); // 程序退出信号

    // 创建遥控器多线程任务
    ret = pthread_create(&threadId[0], NULL, threadJoystick, NULL);
    if (ret != 0)
    {
        cout << "pthread_create error: error_code=" << ret << endl;
    }

    uint16_t counterHart = 0;
    while (1)
    {

        // 智能车控制|连续发送心跳信号
        counterHart++;
        if (uartSendEnable || counterHart > 10)
        {
            driver->carControl(speed, steer); // 智能车运动
            uartSendEnable = false;
            counterHart = 0;
        }

        // 读取图像
        Mat frame;
        if (!capture.read(frame))
        {
            std::cout << "no video frame" << std::endl;
            continue;
        }
        // 图像采集
        static int counter = 0;
        if (collectMoreEnable || collectOnceEnable)
        {
            // 保存到本地
            string name = ".jpg";
            counter++;
            string img_path = "../res/samples/train/";
            name = img_path + to_string(counter) + ".jpg";
            if (stat(img_path.c_str(), &buffer) != 0) // 判断文件夹是否存在
            {
                string command;
                command = "mkdir -p " + img_path;
                system(command.c_str()); // 利用os创建文件夹
            }
            imwrite(name, frame);
            char c = waitKey(10);

            std::cout << "Saved image: " << counter << ".jpg" << std::endl;
            collectOnceEnable = false;
        }

        putText(frame, to_string(counter), Point(10, 30), cv::FONT_HERSHEY_TRIPLEX, 1, cv::Scalar(0, 0, 254), 1, CV_AA); // 显示图片保存序号
        imshow("ingFrame", frame);
        waitKey(1);
    }
    pthread_exit(NULL);
    return 0;
}

/**
 * @brief 系统信号回调函数：系统退出
 *
 * @param signum 信号量
 */
void callbackSignal(int signum)
{
    driver->carControl(0, PWMSERVOMID); // 智能车停止运动
    cout << "====System Exit!!!  -->  CarStopping! " << signum << endl;
    int tid = *((int *)threadId[0]);
    pthread_cancel(tid);
    exit(signum);
}

/**
 * @brief 游戏手柄控制智能车-多线程任务
 *
 * @param threadId 线程ID
 * @return void*
 */
void *threadJoystick(void *args)
{
    cout << "Create thread for Joystick!" << endl;
    // 遥控手柄启动
    int fd = open("/dev/input/js0", O_RDONLY);
    struct EventJoy e;
    e.number = 0;

    while (1)
    {
        read(fd, &e, sizeof(e));
        int type = JS_EVENT_BUTTON | JS_EVENT_INIT;

        if (JOYSTICK_MODE == 1) // 有线手柄
        {
            if (e.type == JS_EVENT_AXIS) // 摇杆
            {
                switch (e.number)
                {
                case 0: // 方向控制
                    steer = PWMSERVOMID + e.value * (PWMSERVOMID - PWMSERVOMIN) / 32767;
                    uartSendEnable = true;
                    break;
                case 5: // 两档速度选择
                    if (e.value > 0)
                    {
                        speed = 0.4;
                        uartSendEnable = true;
                    }
                    break;
                }
            }
            else if (e.type == JS_EVENT_BUTTON) // 按键
            {
                switch (e.number)
                {
                case 3: // 开始连续采图
                    if (e.value == 1)
                    {
                        collectMoreEnable = true;
                    }
                    break;
                case 0: // 停止采图
                    if (e.value == 1)
                    {
                        collectMoreEnable = false;
                        collectOnceEnable = false;
                    }
                    break;
                case 2: // 开始单次采图
                    if (e.value == 1)
                    {
                        collectOnceEnable = true;
                    }
                    break;
                case 5: // 两档速度选择
                    if (e.value == 1)
                    {
                        speed = 0.7;
                        uartSendEnable = true;
                    }
                    break;

                default: // 任意键停止运动
                    speed = 0;
                    uartSendEnable = true;
                    break;
                }
            }
        }
        else if (JOYSTICK_MODE == 2) // 无线手柄A（单绿灯常亮）
        {
            if (e.type == JS_EVENT_AXIS) // 摇杆
            {
                // cout << "AXIS: " << to_string(e.number) << " | " << to_string(e.value) << endl;
                switch (e.number)
                {
                case 0: // 方向控制
                    steer = PWMSERVOMID + e.value * (PWMSERVOMID - PWMSERVOMIN) / 32767;
                    uartSendEnable = true;
                    break;
                case 1:              // 速度方向设置
                    if (e.value < 0) // 向前
                    {
                        dirFront = true;
                        if (speed < 0)
                            speed = -speed;
                    }
                    else if (e.value > 0)
                    {
                        dirFront = false;
                        if (speed > 0)
                            speed = -speed;
                    }
                    break;
                }
            }
            else if (e.type == JS_EVENT_BUTTON) // 按键
            {
                // cout << "BUTTON: " << to_string(e.number) << " | " << to_string(e.value) << endl;
                switch (e.number)
                {
                case 0: // 开始连续采图
                    if (e.value == 1)
                    {
                        collectMoreEnable = true;
                    }
                    break;
                case 2: // 停止采图
                    if (e.value == 1)
                    {
                        collectMoreEnable = false;
                        collectOnceEnable = false;
                    }
                    break;
                case 3: // 开始单次采图
                    if (e.value == 1)
                    {
                        collectOnceEnable = true;
                    }
                    break;
                case 5: // 两档速度选择: 高速档
                    if (e.value == 1)
                    {
                        if (dirFront)
                            speed = 0.7;
                        else
                            speed = -0.7;
                        uartSendEnable = true;
                    }
                    else
                    {
                        speed = 0.0;
                        uartSendEnable = true;
                    }
                    break;

                case 7: // 两档速度选择:慢速档
                    if (e.value >= 1)
                    {
                        if (dirFront)
                            speed = 0.4;
                        else
                            speed = -0.4;
                        uartSendEnable = true;
                    }
                    else
                    {
                        speed = 0.0;
                        uartSendEnable = true;
                    }
                    break;

                default: // 任意键停止运动
                    speed = 0;
                    uartSendEnable = true;
                    break;
                }
            }
        }

        else if (JOYSTICK_MODE == 3) // 无线手柄B（单红灯常亮）
        {
            if (e.type == JS_EVENT_AXIS) // 摇杆
            {
                // cout << "AXIS: " << to_string(e.number) << " | " << to_string(e.value) << endl;
                switch (e.number)
                {
                case 0: // 方向控制
                    steer = PWMSERVOMID + e.value * (PWMSERVOMID - PWMSERVOMIN) / 32767;
                    uartSendEnable = true;
                    break;
                case 5: // 两档速度选择:慢速档
                    if (e.value >= 1)
                    {
                        if (dirFront)
                            speed = 0.4;
                        else
                            speed = -0.4;
                        uartSendEnable = true;
                    }
                    else
                    {
                        speed = 0.0;
                        uartSendEnable = true;
                    }
                    break;
                case 1:
                    if (e.value < 0) // 向前
                    {
                        dirFront = true;
                        if (speed < 0)
                            speed = -speed;
                    }
                    else if (e.value > 0)
                    {
                        dirFront = false;
                        if (speed > 0)
                            speed = -speed;
                    }

                    break;
                }
            }
            else if (e.type == JS_EVENT_BUTTON) // 按键
            {
                // cout << "BUTTON: " << to_string(e.number) << " | " << to_string(e.value) << endl;
                switch (e.number)
                {
                case 2: // 开始单次采图
                    if (e.value == 1)
                    {
                        collectOnceEnable = true;
                    }
                    break;
                case 3: // 开始连续采图
                    if (e.value == 1)
                    {
                        collectMoreEnable = true;
                    }
                    break;
                case 0: // 停止采图
                    if (e.value == 1)
                    {
                        collectMoreEnable = false;
                        collectOnceEnable = false;
                    }
                    break;
                case 5: // 两档速度选择: 高速档
                    if (e.value == 1)
                    {
                        if (dirFront)
                        {
                            speed = 0.7;
                        }
                        else
                        {
                            speed = -0.7;
                        }
                        uartSendEnable = true;
                    }
                    else
                    {
                        speed = 0.0;
                        uartSendEnable = true;
                    }
                    break;

                default: // 任意键停止运动
                    speed = 0;
                    uartSendEnable = true;
                    break;
                }
            }
        }
    }
    close(fd);
    pthread_exit(NULL);
}