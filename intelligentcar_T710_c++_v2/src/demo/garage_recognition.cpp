/**
 * @file garage_recognition.cpp
 * @author Leo ()
 * @brief 车库入库与出库路径规划
 * @version 0.1
 * @date 2022-03-31
 * @copyright Copyright (c) 2022
 * @note 车库识别步骤:
 *                      [01] 出库识别与路径规划
 *                      [02] 出库完成及入库识别
 *                      [03] 入库路径规划及入库完成
 */
#include <fstream>
#include <iostream>
#include <cmath>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include "../src/image_preprocess.cpp"
#include "../src/recognition/garage_recognition.cpp"
#include "../include/detection.hpp" //百度Paddle框架移动端部署

using namespace cv;
using namespace std;

int main(int argc, char *argv[])
{
    // 图像显示窗口初始化
    std::string windowName = "icar";
    cv::namedWindow(windowName, WINDOW_NORMAL); // 图像名称

    IcarShow icarShow;                              // 图像显示窗口重绘
    ImagePreprocess imagePreprocess;                // 图像预处理类实例化
    TrackRecognition trackRecognition;              // 赛道识别
    GarageRecognition garageRecognition;            // 车库识别
    uint16_t circlesThis = 1;                       // 智能车当前运行的圈数
    uint16_t countercircles = 0;                    // 圈数计数器
    std::shared_ptr<Detection> detection = nullptr; // 初始化AI预测模型

    // Video输入源
    // detection = Detection::DetectionInstance("/dev/video0", "../res/model/yolov3_mobilenet_v1"); // 打开摄像头
    detection = Detection::DetectionInstance("../res/samples/garage.mp4", "../res/model/yolov3_mobilenet_v1"); // Video输入源
    printAiEnable = true;

    // 图像矫正参数初始化
    imagePreprocess.imageCorrecteInit();

    // 图像显示窗口初始化
    icarShow.init(4);
    garageRecognition.disGarageEntry = 0.35;
    while (1)
    {
        //[01] 视频源选择
        std::shared_ptr<DetectionResult> resultAI = detection->getLastFrame(); // 获取Paddle多线程模型预测数据
        Mat frame = resultAI->det_render_frame;
        icarShow.setNewWindow(0, frame, "imgFrame"); // 添加需要显示的图像

        //[02] 图像预处理
        Mat imageCorrect = imagePreprocess.imageCorrection(resultAI->rgb_frame); // 图像矫正
        Mat imageBinary = imagePreprocess.imageBinaryzation(imageCorrect);       // 图像二值化
        icarShow.setNewWindow(1, imageBinary, "imgBinary");                      // 添加需要显示的图像

        //[03] 赛道线识别
        trackRecognition.trackRecognition(imageBinary);   // 赛道线识别
        Mat imageTrack = imageCorrect.clone();            // 克隆RGB图像用作显示
        trackRecognition.drawImage(imageTrack);           // 图像显示赛道线识别结果
        icarShow.setNewWindow(2, imageTrack, "imgTrack"); // 添加需要显示的图像

        // [04] 车库识别与路径规划
        countercircles++;                                                 // 圈数计数
        if (garageRecognition.startingCheck(resultAI->predictor_results)) // 检测到起点
        {
            if (countercircles > 30)
            {
                circlesThis++;
            }
        }
        if (circlesThis >= 1 && countercircles > 50) // 入库使能：跑完N圈
            garageRecognition.entryEnable = true;

        garageRecognition.garageRecognition(trackRecognition, resultAI->predictor_results);
        if (garageRecognition.garageStep == garageRecognition.GarageEntryFinish) // 入库完成
        {
            cout << ">>>>>>>   入库结束 !!!!!" << endl;
            exit(0);
        }
        garageRecognition.drawImage(trackRecognition, imageCorrect);

        icarShow.setNewWindow(3, imageCorrect, "imgGarage"); // 添加需要显示的图像

        icarShow.display(); // 图像窗口显示

        char c = waitKey(5); // 等待刷新
    }

    return 0;
}
