/**
 * @file freezone_recognition.cpp
 * @author Leo ()
 * @brief 泛行区识别与路径规划Demo
 * @version 0.1
 * @date 2022-03-30
 * @copyright Copyright (c) 2022
 * @note 泛行区处理步骤：
 *                      [1] 识别泛行区入口标志（等边三角形检测）
 *                      [2] 赛道补偿点搜索&路径处理
 *                      [3] 入泛行区完成
 *                      [4] 识别泛行区出口标志（等边三角形检测）
 *                      [5] 赛道补偿点搜索&路径处理
 *                      [6] 出泛行区完成
 */

#include <fstream>
#include <iostream>
#include <cmath>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include "../src/image_preprocess.cpp"
#include "../src/recognition/freezone_recognition.cpp"

using namespace cv;
using namespace std;

int main(int argc, char *argv[])
{
    IcarShow icarShow;                       // 图像显示窗口重绘
    ImagePreprocess imagePreprocess;         // 图像预处理类实例化
    TrackRecognition trackRecognition;       // 赛道识别
    FreezoneRecognition freezoneRecognition; // 泛型区识别

    // Video输入源
    // VideoCapture capture("/dev/video0");//打开摄像头
    VideoCapture capture("../res/samples/freezone.mp4"); // 打开本地图像
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

    // IPM逆透视变换初始化
    ipm.init(Size(COLSIMAGE, ROWSIMAGE), Size(COLSIMAGEIPM, ROWSIMAGEIPM)); // IPM逆透视变换初始化

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
        Mat imageCorrect = imagePreprocess.imageCorrection(frame);         // 图像矫正
        Mat imageBinary = imagePreprocess.imageBinaryzation(imageCorrect); // 图像二值化
        icarShow.setNewWindow(0, frame, "imgFrame");                       // 添加需要显示的图像

        //[03] 赛道线识别
        trackRecognition.trackRecognition(imageBinary);   // 赛道线识别
        Mat imageTrack = imageCorrect.clone();            // 克隆RGB图像用作显示
        trackRecognition.drawImage(imageTrack);           // 图像显示赛道线识别结果
        icarShow.setNewWindow(1, imageTrack, "imgTrack"); // 添加需要显示的图像

        // [04] 泛行区识别与路径规划
        vector<PredictResult> predict;
        freezoneRecognition.freezoneRecognition(trackRecognition);
        freezoneRecognition.drawImage(trackRecognition, imageCorrect);
        icarShow.setNewWindow(2, imageCorrect, "imgFreezone"); // 添加需要显示的图像

        Mat imageIpm;
        ipm.homography(imageCorrect, imageIpm);       // 图像的逆透视变换
        icarShow.setNewWindow(3, imageIpm, "imgIpm"); // 添加需要显示的图像

        icarShow.display();  // 图像窗口显示
        char c = waitKey(5); // 等待刷新
    }

    return 0;
}