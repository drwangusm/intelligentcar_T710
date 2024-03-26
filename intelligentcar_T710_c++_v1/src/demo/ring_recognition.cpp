/**
 * @file track_recognition.cpp
 * @author Leo ()
 * @brief 赛道识别|Demo
 * @version 0.1
 * @date 2022-03-18
 * @copyright Copyright (c) 2022
 * @note 赛道识别步骤:
 *                      [01] 图像源输入
 *                      [02] 图像预处理：矫正+二值化
 *                      [03] 赛道边缘提取：输出有效边缘信息、岔路信息、边缘斜率方差等
 */

#include <fstream>
#include <iostream>
#include <cmath>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include "../src/image_preprocess.cpp"
#include "../src/recognition/track_recognition.cpp"
#include "../src/recognition/ring_recognition.cpp"

using namespace cv;
using namespace std;

int main(int argc, char *argv[])
{
    // 图像显示窗口初始化
    std::string windowName = "icar";
    cv::namedWindow(windowName, WINDOW_NORMAL); // 图像名称

    IcarShow icarShow;                 // 图像显示窗口重绘
    ImagePreprocess imagePreprocess;   // 图像预处理类实例化
    TrackRecognition trackRecognition; // 赛道识别
    RingRecognition ringRecognition;   // 环岛识别

    // Video输入源
    // VideoCapture capture("/dev/video0");//打开摄像头
    VideoCapture capture("../res/samples/ring.mp4"); // 打开本地图像
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
        trackRecognition.trackRecognition(imageBinary);     // 赛道线识别
        Mat imageTrack = imgaeCorrect.clone();              // 克隆RGB图像用作显示
        trackRecognition.drawImage(imageTrack);             // 图像显示赛道线识别结果
        icarShow.setNewWindow(2, imageTrack, "imageTrack"); // 添加需要显示的图像

        //[04] 环岛识别
        ringRecognition.ringRecognition(trackRecognition, imageBinary);
        ringRecognition.drawImage(trackRecognition, imgaeCorrect);

        icarShow.setNewWindow(3, imgaeCorrect, "imgaeRing"); // 添加需要显示的图像

        icarShow.display(); // 图像窗口显示

        char c = waitKey(50); // 等待刷新
    }

    return 0;
}