/**
 ********************************************************************************************************
 *                                               示例代码
 *                                             EXAMPLE  CODE
 *
 *                      (c) Copyright 2021; SaiShu.Lcc.; Leo; https://bjsstech.com
 *                                   版权所属[SASU-北京赛曙科技有限公司]
 *
 *            The code is for internal use only, not for commercial transactions(开源学习,请勿商用).
 *            The code ADAPTS the corresponding hardware circuit board(代码适配百度Edgeboard-FZ3B),
 *            The specific details consult the professional(欢迎联系我们,代码持续更正，敬请关注相关开源渠道).
 *********************************************************************************************************
 * @file main.cpp
 * @author Leo (liaotengjun@saishukeji.com)
 * @brief 智能汽车-顶层框架（TOP）
 * @version 0.1
 * @date 2022-10-10
 * @copyright Copyright (c) 2022
 *
 */

#include <unistd.h>
#include <iostream>
#include <signal.h>
#include <opencv2/highgui.hpp>                  //OpenCV终端部署
#include <opencv2/opencv.hpp>                   //OpenCV终端部署
#include "../include/uart.hpp"                  //串口通信驱动
#include "../include/detection.hpp"             //百度Paddle框架移动端部署
#include "../include/common.hpp"                //公共类方法文件
#include "image_preprocess.cpp"                 //图像预处理类
#include "recognition/track_recognition.cpp"    //赛道识别基础类
#include "controlcenter_cal.cpp"                //控制中心计算类
#include "motion_controller.cpp"                //智能车运动控制类
#include "recognition/ring_recognition.cpp"     //环岛道路识别与路径规划类
#include "recognition/cross_recognition.cpp"    //十字道路识别与路径规划类
#include "recognition/garage_recognition.cpp"   //车库及斑马线识别类
#include "recognition/freezone_recognition.cpp" //泛行区识别类
#include "detection/busy_detection.cpp"         //施工区AI检测与路径规划类
#include "detection/gasstation_detection.cpp"   //加油站AI检测与路径规划类
#include "detection/freezone_detection.cpp"     //泛行区AI检测与路径规划类
#include "detection/slope_detection.cpp"        //坡道AI检测与路径规划类

using namespace std;
using namespace cv;

void callbackSignal(int signum);
void displayWindowDetailInit(void);
void slowDownEnable(void);
std::shared_ptr<Driver> driver = nullptr; // 初始化串口驱动
bool slowDown = false;                    // 特殊区域减速标志
uint16_t counterSlowDown = 0;             // 减速计数器

enum RoadType
{
    BaseHandle = 0,   // 基础赛道处理
    RingHandle,       // 环岛赛道处理
    CrossHandle,      // 十字道路处理
    FreezoneHandle,   // 泛行区处理
    GarageHandle,     // 车库处理
    GasstationHandle, // 加油站处理
    BusyareaHandle,   // 施工区处理
    SlopeHandle       // 坡道处理
};

int main(int argc, char const *argv[])
{
    std::shared_ptr<Detection> detection = nullptr; // 初始化AI预测模型
    ImagePreprocess imagePreprocess;                // 图像预处理类
    TrackRecognition trackRecognition;              // 赛道识别
    ControlCenterCal controlCenterCal;              // 控制中心计算
    MotionController motionController;              // 运动控制
    RingRecognition ringRecognition;                // 环岛识别
    CrossroadRecognition crossroadRecognition;      // 十字道路处理
    GarageRecognition garageRecognition;            // 车库识别
    FreezoneRecognition freezoneRecognition;        // 泛型区识别类
    BusyareaDetection busyareaDetection;            // 施工区检测
    GasStationDetection gasStationDetection;        // 加油站检测
    FreezoneDetection freezoneDetection;            // 泛行区检测类
    SlopeDetection slopeDetection;                  // 坡道（桥）检测类
    uint16_t counterRunBegin = 1;                   // 智能车启动计数器：等待摄像头图像帧稳定
    RoadType roadType = RoadType::BaseHandle;       // 初始化赛道类型
    uint16_t counterOutTrackA = 0;                  // 车辆冲出赛道计数器A
    uint16_t counterOutTrackB = 0;                  // 车辆冲出赛道计数器B
    uint16_t circlesThis = 1;                       // 智能车当前运行的圈数
    uint16_t countercircles = 0;                    // 圈数计数器
    IcarShow icarShow;                              // 图像显示窗口重绘

    // USB转串口的设备名为 / dev/ttyUSB0
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

    ipm.init(Size(COLSIMAGE, ROWSIMAGE), Size(COLSIMAGEIPM, ROWSIMAGEIPM)); // IPM逆透视变换初始化

    signal(SIGINT, callbackSignal); // 程序退出信号

    motionController.loadParams(); // 读取配置文件
    trackRecognition.rowCutUp = motionController.params.rowCutUp;
    trackRecognition.rowCutBottom = motionController.params.rowCutBottom;
    garageRecognition.disGarageEntry = motionController.params.disGarageEntry;
    freezoneDetection.rateTurnFreezone = motionController.params.rateTurnFreezone;
    if (motionController.params.GarageEnable) // 出入库使能
        roadType = RoadType::GarageHandle;    // 初始赛道元素为出库

    imagePreprocess.imageCorrecteInit(); // 图像矫正参数初始化

    if (motionController.params.debug)
    {
        displayWindowDetailInit();                                                                                       // 显示窗口初始化                                                                                     //显示窗口初始化
        detection = Detection::DetectionInstance(motionController.params.pathVideo, "../res/model/yolov3_mobilenet_v1"); // Video输入源
        printAiEnable = true;                                                                                            // AI检测结果绘制
        icarShow.init(4);
    }
    else
    {
        cout << "等待发车!!!" << endl;
        detection = Detection::DetectionInstance("/dev/video0", "../res/model/yolov3_mobilenet_v1"); // Video输入源
        printAiEnable = false;                                                                       // AI检测结果绘制

        while (!driver->receiveStartSignal()) // 串口接收下位机-比赛开始信号
        {
            ;
        }
        cout << "--------- System start!!! -------" << endl;
        waitKey(100);
        for (int i = 0; i < 30; i++) // 3秒后发车
        {
            driver->carControl(0, PWMSERVOMID); // 智能车停止运动|建立下位机通信
            waitKey(100);
        }
    }

    while (1)
    {
        bool imshowRec = false; // 特殊赛道图像显示标志

        // 处理帧时长监测
        if (motionController.params.debug)
        {
            static auto preTime = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
            auto startTime = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
            cout << "run frame time : " << startTime - preTime << "ms" << endl;
            preTime = startTime;
        }

        //[01] 视频源选择
        std::shared_ptr<DetectionResult> resultAI = detection->getLastFrame(); // 获取Paddle多线程模型预测数据
        Mat frame = resultAI->rgb_frame;                                       // 获取原始摄像头图像
        if (motionController.params.debug)
        {
            savePicture(resultAI->det_render_frame);
            Mat AIframe = resultAI->det_render_frame;
            icarShow.setNewWindow(0, AIframe, "imgFrame"); // 添加需要显示的图像
        }
        else
        {
            if (motionController.params.saveImage) // 保存原始图像
                savePicture(frame);
        }

        //[02] 图像预处理
        Mat imageCorrect = imagePreprocess.imageCorrection(frame);         // RGB
        Mat imageBinary = imagePreprocess.imageBinaryzation(imageCorrect); // Gray

        //[03] 基础赛道识别
        trackRecognition.trackRecognition(imageBinary); // 赛道线识别
        if (motionController.params.debug)
        {
            Mat imageTrack = imageCorrect.clone();
            trackRecognition.drawImage(imageTrack);           // 图像显示赛道线识别结果
            icarShow.setNewWindow(1, imageTrack, "imgTrack"); // 添加需要显示的图像
            savePicture(imageTrack);
        }

        // [04] 出库和入库识别与路径规划
        if (motionController.params.GarageEnable) // 赛道元素是否使能
        {
            if (roadType == RoadType::GarageHandle || roadType == RoadType::BaseHandle)
            {
                countercircles++; // 圈数计数
                if (countercircles > 200)
                    countercircles = 200;
                if (garageRecognition.startingCheck(resultAI->predictor_results)) // 检测到起点
                {
                    ringRecognition.reset();     // 环岛识别初始化
                    busyareaDetection.reset();   // 施工区检测初始化
                    freezoneDetection.reset();   // 泛行区检测复位
                    gasStationDetection.reset(); // 加油站检测复位
                    freezoneRecognition.reset(); // 泛行区识别复位

                    if (countercircles > 60)
                    {
                        circlesThis++;
                        countercircles = 0;
                    }
                }

                if (circlesThis >= motionController.params.circles && countercircles > 100) // 入库使能：跑完N圈
                    garageRecognition.entryEnable = true;

                if (garageRecognition.garageRecognition(trackRecognition, resultAI->predictor_results))
                {
                    if (roadType == RoadType::BaseHandle) // 初次识别-蜂鸣器提醒
                        driver->buzzerSound(1);           // OK
                    roadType = RoadType::GarageHandle;
                    if (garageRecognition.garageStep == garageRecognition.GarageEntryFinish) // 入库完成
                    {
                        cout << ">>>>>>>   入库结束 !!!!!" << endl;
                        callbackSignal(0);
                    }
                    if (motionController.params.debug)
                    {
                        Mat imageGarage = Mat::zeros(Size(COLSIMAGE, ROWSIMAGE), CV_8UC3); // 初始化图像
                        garageRecognition.drawImage(trackRecognition, imageGarage);
                        icarShow.setNewWindow(2, imageGarage, "imgGarage"); // 添加需要显示的图像
                        imshowRec = true;
                        savePicture(imageGarage);
                    }
                }
                else
                    roadType = RoadType::BaseHandle;

                if (garageRecognition.slowDown) // 入库减速
                    slowDownEnable();
            }
        }

        //[05] 加油站检测
        if (motionController.params.GasStationEnable) // 赛道元素是否使能
        {
            if (roadType == RoadType::GasstationHandle || roadType == RoadType::BaseHandle)
            {
                if (gasStationDetection.gasStationDetection(trackRecognition, resultAI->predictor_results))
                {
                    if (roadType == RoadType::BaseHandle) // 初次识别-蜂鸣器提醒
                        driver->buzzerSound(1);           // OK
                    if (motionController.params.debug)
                    {
                        Mat imageGasStation = Mat::zeros(Size(COLSIMAGE, ROWSIMAGE), CV_8UC3); // 初始化图像
                        gasStationDetection.drawImage(trackRecognition, imageGasStation);
                        icarShow.setNewWindow(2, imageGasStation, "imgGasStation"); // 添加需要显示的图像
                        imshowRec = true;
                        savePicture(imageGasStation);

                        // Mat imageIpm;
                        // ipm.homography(imageGasStation, imageIpm); //图像的逆透视变换
                        // imshow("imageIpm", imageIpm);
                        // savePicture(imageIpm);
                    }
                    roadType = RoadType::GasstationHandle;
                }
                else
                    roadType = RoadType::BaseHandle;

                if (gasStationDetection.slowDown)
                    slowDownEnable();
            }
        }

        //[06] 施工区检测
        if (motionController.params.BusyAreaEnable) // 赛道元素是否使能
        {
            if (roadType == RoadType::BusyareaHandle || roadType == RoadType::BaseHandle)
            {
                if (busyareaDetection.busyareaDetection(trackRecognition, resultAI->predictor_results))
                {
                    if (roadType == RoadType::BaseHandle) // 初次识别-蜂鸣器提醒
                        driver->buzzerSound(1);           // OK
                    if (motionController.params.debug)
                    {
                        Mat imageBusyarea = Mat::zeros(Size(COLSIMAGE, ROWSIMAGE), CV_8UC3); // 初始化图像
                        busyareaDetection.drawImage(trackRecognition, imageBusyarea);
                        icarShow.setNewWindow(2, imageBusyarea, "imgBusyarea"); // 添加需要显示的图像
                        imshowRec = true;
                        savePicture(imageBusyarea);

                        // Mat imageIpm;
                        // ipm.homography(imageBusyarea, imageIpm); //图像的逆透视变换
                        // imshow("imageIpm", imageIpm);
                        // savePicture(imageIpm);
                    }
                    roadType = RoadType::BusyareaHandle;
                }
                else
                    roadType = RoadType::BaseHandle;

                if (busyareaDetection.slowDown)
                    slowDownEnable();
            }
        }

        // [07] 坡道（桥）检测与路径规划
        if (motionController.params.SlopEnable) // 赛道元素是否使能
        {
            if (roadType == RoadType::SlopeHandle || roadType == RoadType::BaseHandle)
            {
                if (slopeDetection.slopeDetection(trackRecognition, resultAI->predictor_results))
                {
                    if (roadType == RoadType::BaseHandle) // 初次识别-蜂鸣器提醒
                        driver->buzzerSound(1);           // OK
                    roadType = RoadType::SlopeHandle;
                    if (motionController.params.debug)
                    {
                        Mat imageSlope = Mat::zeros(Size(COLSIMAGE, ROWSIMAGE), CV_8UC3); // 初始化图像
                        slopeDetection.drawImage(trackRecognition, imageSlope);
                        icarShow.setNewWindow(2, imageSlope, "imgSlope"); // 添加需要显示的图像
                        imshowRec = true;
                        savePicture(imageSlope);
                    }
                }
                else
                    roadType = RoadType::BaseHandle;
            }
        }

        // [08] 泛行区检测与识别
        if (motionController.params.FreezoneEnable) // 赛道元素是否使能
        {
            if (roadType == RoadType::FreezoneHandle || roadType == RoadType::BaseHandle)
            {
                if (freezoneDetection.freezoneDetection(trackRecognition, resultAI->predictor_results))
                {
                    if (roadType == RoadType::BaseHandle) // 初次识别-蜂鸣器提醒
                        driver->buzzerSound(1);           // OK
                    roadType = RoadType::FreezoneHandle;
                    if (motionController.params.debug)
                    {
                        Mat imageFreezone = Mat::zeros(Size(COLSIMAGE, ROWSIMAGE), CV_8UC3); // 初始化图像
                        freezoneDetection.drawImage(trackRecognition, imageFreezone);
                        icarShow.setNewWindow(2, imageFreezone, "imgFreezone"); // 添加需要显示的图像
                        imshowRec = true;
                        savePicture(imageFreezone);
                    }
                    // if (freezoneDetection.slowDown)
                    //     slowDownEnable();
                }
                else
                    roadType = RoadType::BaseHandle;
            }
        }
        else
        {
            if (roadType == RoadType::FreezoneHandle || roadType == RoadType::BaseHandle)
            {
                if (freezoneRecognition.freezoneRecognition(trackRecognition, resultAI->predictor_results))
                {
                    if (roadType == RoadType::BaseHandle) // 初次识别-蜂鸣器提醒
                        driver->buzzerSound(1);           // OK
                    roadType = RoadType::FreezoneHandle;
                    if (motionController.params.debug)
                    {
                        Mat imageFreezone = Mat::zeros(Size(COLSIMAGE, ROWSIMAGE), CV_8UC3); // 初始化图像
                        freezoneRecognition.drawImage(trackRecognition, imageFreezone);
                        icarShow.setNewWindow(2, imageFreezone, "imgFreezone"); // 添加需要显示的图像
                        imshowRec = true;
                        savePicture(imageFreezone);
                    }
                }
                else
                    roadType = RoadType::BaseHandle;
            }
        }

        // [09] 环岛识别与处理
        if (motionController.params.RingEnable) // 赛道元素是否使能
        {
            if (roadType == RoadType::RingHandle || roadType == RoadType::BaseHandle)
            {
                if (ringRecognition.ringRecognition(trackRecognition, imageBinary))
                {
                    if (roadType == RoadType::BaseHandle) // 初次识别-蜂鸣器提醒
                        driver->buzzerSound(1);           // OK
                    roadType = RoadType::RingHandle;
                    if (motionController.params.debug)
                    {
                        Mat imageRing = Mat::zeros(Size(COLSIMAGE, ROWSIMAGE), CV_8UC3); // 初始化图像
                        ringRecognition.drawImage(trackRecognition, imageRing);
                        icarShow.setNewWindow(2, imageRing, "imgRing"); // 添加需要显示的图像
                        imshowRec = true;
                        savePicture(imageRing);
                    }
                }
                else
                    roadType = RoadType::BaseHandle;
            }
        }

        // [10] 十字道路处理
        if (motionController.params.CrossEnable) // 赛道元素是否使能
        {
            if (roadType == RoadType::CrossHandle || roadType == RoadType::BaseHandle)
            {
                if (crossroadRecognition.crossroadRecognition(trackRecognition, resultAI->predictor_results))
                {
                    roadType = RoadType::CrossHandle;

                    if (motionController.params.debug)
                    {
                        Mat imageCross = Mat::zeros(Size(COLSIMAGE, ROWSIMAGE), CV_8UC3); // 初始化图像
                        crossroadRecognition.drawImage(trackRecognition, imageCross);
                        icarShow.setNewWindow(2, imageCross, "imgCross"); // 添加需要显示的图像
                        imshowRec = true;
                        savePicture(imageCross);
                    }
                }
                else
                    roadType = RoadType::BaseHandle;
            }
        }

        // [11] 控制中心计算
        if (trackRecognition.pointsEdgeLeft.size() < 30 && trackRecognition.pointsEdgeRight.size() < 30 && roadType != RoadType::FreezoneHandle && roadType != RoadType::SlopeHandle) // 防止车辆冲出赛道
        {
            counterOutTrackA++;
            counterOutTrackB = 0;
            if (counterOutTrackA > 20)
                callbackSignal(0);
        }
        else
        {
            counterOutTrackB++;
            if (counterOutTrackB > 50)
            {
                counterOutTrackA = 0;
                counterOutTrackB = 50;
            }
        }
        controlCenterCal.controlCenterCal(trackRecognition); // 根据赛道边缘信息拟合运动控制中心

        // [12] 运动控制
        if (counterRunBegin > 30) ////智能车启动延时：前几场图像不稳定
        {
            // 智能汽车方向控制
            motionController.pdController(controlCenterCal.controlCenter); // PD控制器姿态控制

            // 智能汽车速度控制
            switch (roadType)
            {
            case RoadType::FreezoneHandle:                                               // 泛行区处理速度
                if (motionController.params.FreezoneEnable)                              // AI区域的速度
                    motionController.motorSpeed = motionController.params.speedFreezone; // 匀速控制
                else                                                                     // 非AI区域的速度
                    motionController.speedController(true, slowDown, controlCenterCal);  // 变加速控制
                break;
            case RoadType::GasstationHandle:                                        // 加油站速度
                motionController.motorSpeed = motionController.params.speedGasBusy; // 匀速控制
                break;
            case RoadType::BusyareaHandle:                                          // 施工区速度
                motionController.motorSpeed = motionController.params.speedGasBusy; // 匀速控制
                break;
            case RoadType::SlopeHandle:                                          // 坡道速度
                motionController.motorSpeed = motionController.params.speedSlop; // 匀速控制
                break;
            case RoadType::GarageHandle:                                           // 出入库速度
                motionController.motorSpeed = motionController.params.speedGarage; // 匀速控制
                break;
            default:                                                                // 基础巡线 | 十字 |环岛速度
                motionController.speedController(true, slowDown, controlCenterCal); // 变加速控制
                break;
            }

            if (!motionController.params.debug) // 调试模式下不控制车辆运动
            {
                driver->carControl(motionController.motorSpeed, motionController.servoPwm); // 串口通信，姿态与速度控制
            }

            // 减速缓冲
            if (slowDown)
            {
                counterSlowDown++;
                if (counterSlowDown > 50)
                {
                    slowDown = false;
                    counterSlowDown = 0;
                }
            }
        }
        else
            counterRunBegin++;

        // [13]调试模式下图像显示和存图
        if (motionController.params.debug)
        {
            controlCenterCal.drawImage(trackRecognition, imageCorrect);
            switch (roadType)
            {
            case RoadType::BaseHandle:                                                                                             // 基础赛道处理
                putText(imageCorrect, "[1] Track", Point(10, 20), cv::FONT_HERSHEY_TRIPLEX, 0.3, cv::Scalar(0, 0, 255), 1, CV_AA); // 显示赛道识别类型
                break;
            case RoadType::RingHandle:                                                                                            // 环岛赛道处理
                putText(imageCorrect, "[1] Ring", Point(10, 20), cv::FONT_HERSHEY_TRIPLEX, 0.3, cv::Scalar(0, 255, 0), 1, CV_AA); // 显示赛道识别类型
                break;
            case RoadType::CrossHandle:                                                                                            // 十字道路处理
                putText(imageCorrect, "[1] Cross", Point(10, 20), cv::FONT_HERSHEY_TRIPLEX, 0.3, cv::Scalar(0, 255, 0), 1, CV_AA); // 显示赛道识别类型
                break;
            case RoadType::FreezoneHandle:                                                                                            // 泛行区处理
                putText(imageCorrect, "[1] Freezone", Point(10, 20), cv::FONT_HERSHEY_TRIPLEX, 0.3, cv::Scalar(0, 255, 0), 1, CV_AA); // 显示赛道识别类型
                break;
            case RoadType::GarageHandle:                                                                                            // 车库处理
                putText(imageCorrect, "[1] Garage", Point(10, 20), cv::FONT_HERSHEY_TRIPLEX, 0.3, cv::Scalar(0, 255, 0), 1, CV_AA); // 显示赛道识别类型
                break;
            case RoadType::GasstationHandle:                                                                                            // 加油站处理
                putText(imageCorrect, "[1] Gasstation", Point(10, 20), cv::FONT_HERSHEY_TRIPLEX, 0.3, cv::Scalar(0, 255, 0), 1, CV_AA); // 显示赛道识别类型
                break;
            case RoadType::BusyareaHandle:                                                                                            // 施工区处理
                putText(imageCorrect, "[1] Busyarea", Point(10, 20), cv::FONT_HERSHEY_TRIPLEX, 0.3, cv::Scalar(0, 255, 0), 1, CV_AA); // 显示赛道识别类型
                break;
            case RoadType::SlopeHandle:                                                                                           // 坡道处理
                putText(imageCorrect, "[1] Slop", Point(10, 20), cv::FONT_HERSHEY_TRIPLEX, 0.3, cv::Scalar(0, 255, 0), 1, CV_AA); // 显示赛道识别类型
                break;
            }

            putText(imageCorrect, "v: " + formatDoble2String(motionController.motorSpeed, 2), Point(COLSIMAGE - 60, 80), FONT_HERSHEY_PLAIN, 1, Scalar(0, 0, 255), 1); // 车速

            string str = to_string(circlesThis) + "/" + to_string(motionController.params.circles);
            putText(imageCorrect, str, Point(COLSIMAGE - 50, ROWSIMAGE - 20), cv::FONT_HERSHEY_TRIPLEX, 0.5, cv::Scalar(0, 255, 0), 1, CV_AA); // 显示圈数
            if (!imshowRec)                                                                                                                    // 保持调试图像存储顺序和显示一致性
            {
                Mat imageNone = Mat::zeros(Size(COLSIMAGE, ROWSIMAGE), CV_8UC3); // 初始化图像
                icarShow.setNewWindow(2, imageNone, "imgNone");                  // 添加需要显示的图像
                savePicture(imageNone);
            }
            icarShow.setNewWindow(3, imageCorrect, "imgCorrect"); // 添加需要显示的图像
            savePicture(imageCorrect);
            icarShow.display(); // 图像窗口显示

            char c = waitKey(5);
        }
    }

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
    exit(signum);
}

/**
 * @brief OpenCV图像显示窗口初始化（详细参数/Debug模式）
 *
 */
void displayWindowDetailInit(void)
{
    string windowName = "icar";
    cv::namedWindow(windowName, WINDOW_NORMAL); // 图像名称
}

/**
 * @brief 车辆减速使能
 *
 */
void slowDownEnable(void)
{
    slowDown = true;
    counterSlowDown = 0;
}
