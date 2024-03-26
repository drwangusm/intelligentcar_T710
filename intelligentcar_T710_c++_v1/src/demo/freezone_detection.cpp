/**
 * @file freezone_detection.cpp
 * @author Leo ()
 * @brief 泛行区检测与路径规划Demo
 * @version 0.1
 * @date 2022-03-30
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <fstream>
#include <iostream>
#include <cmath>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include "../src/image_preprocess.cpp"
#include "../src/detection/freezone_detection.cpp" //泛行区AI检测与路径规划类
#include "../include/detection.hpp"                //百度Paddle框架移动端部署

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
    FreezoneDetection freezoneDetection;            // 泛行区检测类
    std::shared_ptr<Detection> detection = nullptr; // 初始化AI预测模型

    // Video输入源
    // detection = Detection::DetectionInstance("/dev/video0", "../res/model/yolov3_mobilenet_v1"); // 打开摄像头
    detection = Detection::DetectionInstance("../res/samples/freezoneDet.mp4", "../res/model/yolov3_mobilenet_v1"); // Video输入源
    printAiEnable = true;                                                                                           // 开启AI绘制

    // 图像矫正参数初始化
    imagePreprocess.imageCorrecteInit();

    // IPM逆透视变换初始化
    ipm.init(Size(COLSIMAGE, ROWSIMAGE), Size(COLSIMAGEIPM, ROWSIMAGEIPM)); // IPM逆透视变换初始化

    // 图像显示窗口初始化
    icarShow.init(4);

    while (1)
    {
        //[01] 视频源选择
        std::shared_ptr<DetectionResult> resultAI = detection->getLastFrame(); // 获取Paddle多线程模型预测数据
        Mat frame = resultAI->det_render_frame;                                // 克隆AI预测图像用作显示
        icarShow.setNewWindow(0, frame, "imgFrame");                           // 添加需要显示的图像

        //[02] 图像预处理
        Mat imageCorrect = imagePreprocess.imageCorrection(resultAI->rgb_frame); // 图像矫正
        Mat imageBinary = imagePreprocess.imageBinaryzation(imageCorrect);       // 图像二值化
        icarShow.setNewWindow(1, imageCorrect, "imgCorrect");

        //[03] 赛道线识别
        trackRecognition.trackRecognition(imageBinary);   // 赛道线识别
        Mat imageTrack = imageCorrect.clone();            // 克隆RGB图像用作显示
        trackRecognition.drawImage(imageTrack);           // 图像显示赛道线识别结果
        icarShow.setNewWindow(2, imageTrack, "imgTrack"); // 添加需要显示的图像

        // [04] 泛行区检测与路径规划
        freezoneDetection.freezoneDetection(trackRecognition, resultAI->predictor_results);
        freezoneDetection.drawImage(trackRecognition, imageCorrect);

        icarShow.setNewWindow(3, imageCorrect, "imgFreezone"); // 添加需要显示的图像

        icarShow.display(); // 图像窗口显示

        char c = waitKey(50); // 等待刷新
    }

    return 0;
}