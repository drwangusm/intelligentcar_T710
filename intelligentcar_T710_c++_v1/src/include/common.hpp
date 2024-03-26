#pragma once

#include "json.hpp"
#include <fstream>
#include <iostream>
#include <opencv2/highgui.hpp> //OpenCV终端部署
#include <opencv2/opencv.hpp>  //OpenCV终端部署
#include "../src/perspective_mapping.cpp"

using namespace std;
using namespace cv;

#define COLSIMAGE 320                 // 图像的列数
#define ROWSIMAGE 240                 // 图像的行数
#define COLSIMAGEIPM 320              // IPM图像的列数
#define ROWSIMAGEIPM 400              // IPM图像的行数
#define PWMSERVOMAX 1900              // 舵机PWM最大值（左）1840
#define PWMSERVOMID 1500              // 舵机PWM中值 1520
#define PWMSERVOMIN 1100              // 舵机PWM最小值（右）1200
#define LABEL_BLUEONE "BlueOne"       // AI标签
#define LABEL_BLUETWO "BlueTwo"       // AI标签
#define LABEL_BUSYAREA "BusyArea"     // AI标签
#define LABEL_CONE "Cone"             // AI标签
#define LABEL_CROSSWALK "CrossWalk"   // AI标签
#define LABEL_FREEZONE "FreeZone"     // AI标签
#define LABEL_GASSTATION "GasStation" // AI标签
#define LABEL_SLOP "Slope"            // AI标签
#define LABEL_STOP "Stop"             // AI标签
#define LABEL_WRITEONE "WriteOne"     // AI标签
#define LABEL_WRITETWO "WriteTwo"     // AI标签

bool printAiEnable = false;
PerspectiveMapping ipm; // 逆透视变换公共类

struct POINT
{
    int x = 0;
    int y = 0;
    float slope = 0.0f;

    POINT(){};
    POINT(int x, int y) : x(x), y(y){};
};

/**
 * @brief 存储图像至本地
 *
 * @param image 需要存储的图像
 */
void savePicture(Mat &image)
{
    // 存图
    string name = ".png";
    static int counter = 0;
    counter++;
    string img_path = "../res/samples/train/";
    name = img_path + to_string(counter) + ".jpg";
    imwrite(name, image);
}

//--------------------------------------------------[公共方法]----------------------------------------------------
/**
 * @brief int集合平均值计算
 *
 * @param arr 输入数据集合
 * @return double
 */
double average(vector<int> vec)
{
    if (vec.size() < 1)
        return -1;

    double sum = 0;
    for (int i = 0; i < vec.size(); i++)
    {
        sum += vec[i];
    }

    return (double)sum / vec.size();
}

/**
 * @brief int集合数据方差计算
 *
 * @param vec Int集合
 * @return double
 */
double sigma(vector<int> vec)
{
    if (vec.size() < 1)
        return 0;

    double aver = average(vec); // 集合平均值
    double sigma = 0;
    for (int i = 0; i < vec.size(); i++)
    {
        sigma += (vec[i] - aver) * (vec[i] - aver);
    }
    sigma /= (double)vec.size();
    return sigma;
}

/**
 * @brief 赛道点集的方差计算
 *
 * @param vec
 * @return double
 */
double sigma(vector<POINT> vec)
{
    if (vec.size() < 1)
        return 0;

    double sum = 0;
    for (int i = 0; i < vec.size(); i++)
    {
        sum += vec[i].y;
    }
    double aver = (double)sum / vec.size(); // 集合平均值

    double sigma = 0;
    for (int i = 0; i < vec.size(); i++)
    {
        sigma += (vec[i].y - aver) * (vec[i].y - aver);
    }
    sigma /= (double)vec.size();
    return sigma;
}

/**
 * @brief 阶乘计算
 *
 * @param x
 * @return int
 */
int factorial(int x)
{
    int f = 1;
    for (int i = 1; i <= x; i++)
    {
        f *= i;
    }
    return f;
}

/**
 * @brief 贝塞尔曲线
 *
 * @param dt
 * @param input
 * @return vector<POINT>
 */
vector<POINT> Bezier(double dt, vector<POINT> input)
{
    vector<POINT> output;

    double t = 0;
    while (t <= 1)
    {
        POINT p;
        double x_sum = 0.0;
        double y_sum = 0.0;
        int i = 0;
        int n = input.size() - 1;
        while (i <= n)
        {
            double k =
                factorial(n) / (factorial(i) * factorial(n - i)) * pow(t, i) * pow(1 - t, n - i);
            x_sum += k * input[i].x;
            y_sum += k * input[i].y;
            i++;
        }
        p.x = x_sum;
        p.y = y_sum;
        output.push_back(p);
        t += dt;
    }
    return output;
}

auto formatDoble2String(double val, int fixed)
{
    auto str = std::to_string(val);
    return str.substr(0, str.find(".") + fixed + 1);
}

/**
 * @brief 点到直线的距离计算
 *
 * @param a 直线的起点
 * @param b 直线的终点
 * @param p 目标点
 * @return double
 */
double distanceForPoint2Line(POINT a, POINT b, POINT p)
{
    int d = 0; // 距离

    double ab_distance =
        sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
    double ap_distance =
        sqrt((a.x - p.x) * (a.x - p.x) + (a.y - p.y) * (a.y - p.y));
    double bp_distance =
        sqrt((p.x - b.x) * (p.x - b.x) + (p.y - b.y) * (p.y - b.y));

    double half = (ab_distance + ap_distance + bp_distance) / 2;
    double area = sqrt(half * (half - ab_distance) * (half - ap_distance) * (half - bp_distance));

    return (2 * area / ab_distance);
}
class IcarShow
{
private:
    bool enable = false; // 显示窗口使能
    int sizeWindow = 1;  // 窗口数量
    cv::Mat imgShow;     // 窗口图像
public:
    /**
     * @brief 显示窗口初始化
     *
     * @param size 窗口数量(1~7)
     */
    void init(int size)
    {
        if (size <= 0 || size > 7)
            return;

        cv::namedWindow("icar", WINDOW_NORMAL);                // 图像名称
        cv::resizeWindow("icar", COLSIMAGE * size, ROWSIMAGE); // 分辨率

        imgShow = cv::Mat::zeros(ROWSIMAGE, COLSIMAGE * size, CV_8UC3);
        enable = true;
        sizeWindow = size;
    }

    /**
     * @brief 设置新窗口属性
     *
     * @param index 窗口序号
     * @param img 显示图像
     * @param name 窗口名称
     */
    void setNewWindow(int index, Mat img, string name)
    {
        // 数据溢出保护
        if (!enable || index < 0 || index >= sizeWindow)
            return;
        if (img.cols <= 0 || img.rows <= 0)
            return;

        Mat imgDraw = img.clone();

        if (imgDraw.type() == CV_8UC1) // 非RGB类型的图像
            cvtColor(imgDraw, imgDraw, cv::COLOR_GRAY2BGR);

        // 图像缩放
        if (imgDraw.cols != COLSIMAGE || imgDraw.rows != ROWSIMAGE)
        {
            float fx = COLSIMAGE / imgDraw.cols;
            float fy = ROWSIMAGE / imgDraw.rows;
            if (fx <= fy)
                resize(imgDraw, imgDraw, Size(COLSIMAGE, ROWSIMAGE), fx, fx);
            else
                resize(imgDraw, imgDraw, Size(COLSIMAGE, ROWSIMAGE), fy, fy);
        }

        // 限制图片标题长度
        string text = "[" + to_string(index + 1) + "] ";
        if (name.length() > 15)
            text = text + name.substr(0, 15);
        else
            text = text + name;

        putText(imgDraw, text, Point(10, 20), cv::FONT_HERSHEY_TRIPLEX, 0.5, cv::Scalar(255, 0, 0), 0.5, CV_AA);

        Rect placeImg = cvRect(COLSIMAGE * index, 0, COLSIMAGE, ROWSIMAGE);
        imgDraw.copyTo(imgShow(placeImg));
    }

    /**
     * @brief 融合后的图像显示
     *
     */
    void display()
    {
        if (enable)
            imshow("icar", imgShow);
    }
};