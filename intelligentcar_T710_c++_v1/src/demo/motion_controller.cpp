/**
 * @file motion_controller.cpp
 * @author Leo ()
 * @brief 智能汽车车辆控制
 * @version 0.1
 * @date 2022-04-19
 * @copyright Copyright (c) 2022
 * @note 车辆控制步骤：
 *                  [01] 根据赛道识别与检测，及路径规划结果，计算车辆的控制中心
 *                  [02] 三阶PD控制器实现车辆姿态控制
 */
#include <fstream>
#include <iostream>
#include <cmath>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include "../src/image_preprocess.cpp"
#include "../src/recognition/track_recognition.cpp"
#include "../include/uart.hpp"
#include "../src/controlcenter_cal.cpp"
#include "../src/motion_controller.cpp"

using namespace cv;
using namespace std;
void savePicture(Mat &image);

int main(int argc, char *argv[])
{
    // 图像显示窗口初始化
    std::string windowName = "icar";
    cv::namedWindow(windowName, WINDOW_NORMAL); // 图像名称

    IcarShow icarShow;                 // 图像显示窗口重绘
    ImagePreprocess imagePreprocess;   // 图像预处理类实例化
    TrackRecognition trackRecognition; // 赛道识别
    ControlCenterCal controlCenterCal; // 控制中心计算
    MotionController motionController; // 运动控制

    // 串口驱动
    std::shared_ptr<Driver> driver = nullptr; // 初始化串口驱动
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

    // Video输入源
    // VideoCapture capture("/dev/video0");//打开摄像头
    VideoCapture capture("../res/samples/demo.mp4"); // 打开本地图像
    if (!capture.isOpened())
    {
        std::cout << "can not open video device " << std::endl;
        return 1;
    }
    capture.set(cv::CAP_PROP_FRAME_WIDTH, 320); // 设置OpenCV分辨率
    capture.set(cv::CAP_PROP_FRAME_HEIGHT, 240);

    // 获取图像基本参数
    double rate = capture.get(CAP_PROP_FPS);            // 帧率
    double width = capture.get(CAP_PROP_FRAME_WIDTH);   // 宽度：列
    double height = capture.get(CAP_PROP_FRAME_HEIGHT); // 高度：行
    cout << "Camera Param: frame rate = " << rate << " width = " << width << " height = " << height << endl;

    motionController.loadParams(); // 读取配置文件

    // 图像矫正参数初始化
    imagePreprocess.imageCorrecteInit();

    // 图像显示窗口初始化
    icarShow.init(4);

    while (1)
    {
        //[01] 视频源选择
        Mat frame;
        if (!capture.read(frame))
        {
            std::cout << "no video frame" << std::endl;
            exit(0); // 退出程序
            continue;
        }

        //[02] 图像预处理
        Mat imgaeCorrect = imagePreprocess.imageCorrection(frame);         // 图像矫正
        Mat imageBinary = imagePreprocess.imageBinaryzation(imgaeCorrect); // 图像二值化
        icarShow.setNewWindow(0, frame, "frame");                          // 添加需要显示的图像
        icarShow.setNewWindow(1, imageBinary, "imgBinary");                // 添加需要显示的图像

        //[03] 赛道线识别
        trackRecognition.trackRecognition(imageBinary); // 赛道线识别
        Mat imageTrack = imgaeCorrect.clone();          // 克隆RGB图像用作显示
        trackRecognition.drawImage(imageTrack);         // 图像显示赛道线识别结果

        icarShow.setNewWindow(2, imageTrack, "imageTrack"); // 添加需要显示的图像

        // [04] 控制中心计算
        controlCenterCal.controlCenterCal(trackRecognition);
        controlCenterCal.drawImage(trackRecognition, imgaeCorrect);

        icarShow.setNewWindow(3, imgaeCorrect, "imgaeCorrect"); // 添加需要显示的图
        icarShow.display();                                     // 图像窗口显示

        // [05] 运动控制
        motionController.pdController(controlCenterCal.controlCenter);
        driver->carControl(motionController.motorSpeed, motionController.servoPwm); // 智能车姿态与速度控制

        char c = waitKey(50); // 等待刷新
    }

    return 0;
}
