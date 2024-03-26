#pragma once

#include "capture.hpp"
#include "predictor.hpp"
#include <condition_variable>
#include <mutex>
#include <thread>
#include <unistd.h>

/**
 * @brief AI目标检测结果
 *
 */
struct DetectionResult
{
  cv::Mat det_render_frame;                     // AI推理绘制图像
  cv::Mat rgb_frame;                            // 相机原始图像
  std::vector<PredictResult> predictor_results; // AI推理数据
};

class Detection
{

public:
  Detection(bool logEn = false) : _log_en(logEn) {}
  ~Detection() {}

  /**
   * @brief 多线程初始化
   *
   * @param videoPath 视频地址
   * @param modelPath AI模型地址
   * @return int
   */
  int init(std::string videoPath, std::string modelPath)
  {
    _isFile = true;
    _videoPath = videoPath;
    _capture = std::make_shared<Capture>();
    if (_capture == nullptr)
    {
      std::cout << "Capture create failed." << std::endl;
      return -1;
    }
    int ret = _capture->open(videoPath);
    if (ret != 0)
    {
      std::cout << "Capture open failed." << std::endl;
      return -1;
    }
    return _init(modelPath);
  };

  /**
   * @brief 多线程初始化
   *
   * @param cameraIndex 摄像头编号
   * @param modelPath AI模型地址
   * @return int
   */
  int init(int cameraIndex, std::string modelPath)
  {
    _capture = std::make_shared<Capture>();
    if (_capture == nullptr)
    {
      std::cout << "Capture create failed." << std::endl;
      return -1;
    }
    int ret = _capture->open(cameraIndex);
    if (ret != 0)
    {
      std::cout << "Capture open failed." << std::endl;
      return -1;
    }
    return _init(modelPath);
  };

  /**
   * @brief 启动多线程
   *
   */
  void startThread()
  {
    _thread = std::make_unique<std::thread>([this]()
                                            {
      while (1) 
      {
        std::shared_ptr<DetectionResult> result = std::make_shared<DetectionResult>();

        result->rgb_frame = _capture->read();

        // 重新开打视频文件（循环播放）
        if (result->rgb_frame.empty() && _isFile) 
        {
          _capture->close();
          int ret = _capture->open(_videoPath);
          if (ret != 0) {
            std::cout << "Capture open failed." << std::endl;
            return -1;
          }
          continue;
        }

        if (result->rgb_frame.empty()) 
        {
          std::cout << "Error: Capture Get Empty Error Frame." << std::endl;
          exit(-1);
        }

        //启动AI推理
        auto feeds = _predictor->preprocess(result->rgb_frame, {320, 320});
        _predictor->run(*feeds);
        // get result
        _predictor->render();
        // 绘制AI识别结果(图像)
        if (printAiEnable) 
        {
          result->det_render_frame = result->rgb_frame.clone();
          _predictor->drawBox(result->det_render_frame);
        }

        result->predictor_results = _predictor->results;
        // 多线程共享数据传递
        std::unique_lock<std::mutex> lock(_mutex);
        _lastResult = result;
        cond_.notify_all();
        if (printAiEnable) // 调试模式下降低帧率
        {
          //waitKey(60);
          sleep(0.05);
        }
      } });
  }
  int stop() { return 0; }
  int deinit() { return 0; }

  std::shared_ptr<DetectionResult> getLastFrame()
  {
    std::shared_ptr<DetectionResult> ret = nullptr;
    {
      std::unique_lock<std::mutex> lock(_mutex);

      while (_lastResult == nullptr)
      {
        cond_.wait(lock);
      }
      ret = _lastResult;
      _lastResult = nullptr;
    }
    return ret;
  }

public:
  static std::shared_ptr<Detection> DetectionInstance(std::string file_path,
                                                      std::string model_path)
  {
    static std::shared_ptr<Detection> detectioner = nullptr;
    if (detectioner == nullptr)
    {
      detectioner = std::make_shared<Detection>();
      int ret = detectioner->init(file_path, model_path);
      if (ret != 0)
      {
        std::cout << "Detection init error :" << model_path << std::endl;
        exit(-1);
      }
      detectioner->startThread();
    }
    return detectioner;
  }

  static std::shared_ptr<Detection> DetectionInstance()
  {
    static std::shared_ptr<Detection> detectioner = nullptr;
    if (detectioner == nullptr)
    {
      detectioner = std::make_shared<Detection>();
      std::string model_path = "../res/model/yolov3_mobilenet_v1";
      int ret = detectioner->init(0, model_path);
      if (ret != 0)
      {
        std::cout << "Detection init error :" << model_path << std::endl;
        exit(-1);
      }
      detectioner->startThread();
    }
    return detectioner;
  }

private:
  int _init(std::string model_config_path)
  {
    _predictor = std::make_shared<Predictor>();

    if (!_predictor->init("../res/model/yolov3_mobilenet_v1")) // AI推理初始化
    {
      std::cout << "Predictor init failed." << std::endl;
      return -1;
    }
    return 0;
  }

private:
  bool _isFile = false;
  std::string _videoPath;
  bool _log_en;
  std::shared_ptr<DetectionResult> _lastResult;

  std::mutex _mutex;
  std::condition_variable cond_;

  std::unique_ptr<std::thread> _thread;

  std::shared_ptr<Capture> _capture;
  std::shared_ptr<Predictor> _predictor;
};
