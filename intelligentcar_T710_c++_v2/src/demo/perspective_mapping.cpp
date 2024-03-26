/**
 * @file perspective_mapping.cpp
 * @author Leo ()
 * @brief 图像（逆）透视变换Demo
 * @version 0.1
 * @date 2022-04-08
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
#include "../src/perspective_mapping.cpp"

using namespace cv;
using namespace std;

int main(int argc, char *argv[])
{
    IcarShow icarShow;                     // 图像显示窗口重绘
    ImagePreprocess imagePreprocess;       // 图像预处理类实例化
    PerspectiveMapping perspectiveMapping; // 逆透视变换

    // 图像显示窗口初始化
    std::string windowName = "icar";
    cv::namedWindow(windowName, WINDOW_NORMAL); // 图像名称

    // 视频输入源
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

    // 图像矫正参数初始化
    imagePreprocess.imageCorrecteInit();

    // 图像显示窗口初始化
    icarShow.init(2);

    // IPM逆透视变换初始化
    perspectiveMapping.init(Size(COLSIMAGE, ROWSIMAGE), Size(COLSIMAGEIPM, ROWSIMAGEIPM));

    while (1)
    {
        //[01] 视频源选择
        Mat frame;
        if (!capture.read(frame))
        {
            std::cout << "no video frame!" << std::endl;
            exit(0); // 退出程序
            continue;
        }

        //[02] 图像矫正
        Mat imageCorrect = imagePreprocess.imageCorrection(frame); // 图像矫正

        //[03] 图像透视变换（逆）
        Mat imageIpm;
        perspectiveMapping.homography(imageCorrect, imageIpm); // 图像的逆透视变换                      // 俯视域的图像

        icarShow.setNewWindow(0, frame, "imgFrame");  // 添加需要显示的图像
        icarShow.setNewWindow(1, imageIpm, "imgIpm"); // 添加需要显示的图像

        icarShow.display(); // 图像窗口显示

        char c = waitKey(5); // 等待刷新
    }

    return 0;
}