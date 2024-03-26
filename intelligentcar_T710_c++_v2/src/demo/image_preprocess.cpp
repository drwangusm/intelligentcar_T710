/**
 * @file image_preprocess.cpp
 * @author Leo
 * @brief 图像预处理Demo
 * @version 0.1
 * @date 2022-02-18
 * @copyright Copyright (c) 2022
 * @note 图像预处理步骤:
 *                      [01] 图像源输入
 *                      [02] 图像预处理：得到矫正后的二值化图像
 *                      [03] 图像二值化：大津法动态自适应阈值
 *
 */

#include <fstream>
#include <iostream>
#include <cmath>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include "../src/image_preprocess.cpp"

using namespace cv;
using namespace std;

int main(int argc, char *argv[])
{
    // 图像显示窗口初始化
    std::string windowName = "icar";
    cv::namedWindow(windowName, WINDOW_NORMAL); // 图像名称

    IcarShow icarShow;               // 图像显示窗口重绘
    ImagePreprocess imagePreprocess; // 图像预处理类实例化

    // Video输入源
    // VideoCapture capture("/dev/video0"); // 打开摄像头
    VideoCapture capture("../res/samples/demo.mp4"); // 打开本地图像
    if (!capture.isOpened())
    {
        std::cout << "can not open video device " << std::endl;
        return 1;
    }

    // 图像矫正参数初始化
    imagePreprocess.imageCorrecteInit();

    // 图像显示窗口初始化
    icarShow.init(3);

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

        //[02] 图像矫正
        Mat imageCorrect = imagePreprocess.imageCorrection(frame); // 图像矫正

        //[03] 图像二值化
        Mat imageBinary = imagePreprocess.imageBinaryzation(imageCorrect); // 图像二值化

        icarShow.setNewWindow(0, frame, "imgFrame");          // 添加需要显示的图像
        icarShow.setNewWindow(1, imageCorrect, "imgCorrect"); // 添加需要显示的图像
        icarShow.setNewWindow(2, imageBinary, "imgBinary");   // 添加需要显示的图像

        icarShow.display(); // 图像窗口显示

        char c = waitKey(5); // 等待刷新
    }

    return 0;
}