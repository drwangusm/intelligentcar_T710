#!/usr/bin/env python
# -*- encoding: utf-8 -*-
"""
Description:    AI目标检测（慢行区检测）|demo
Version:        v1.0
Autor:          Hjc
Date:           2023年02月17日 20:55:06
"""

import base64
import cv2
import numpy as np
import sys

sys.path.append("../lib")  # 引用C++库
import intelligentCar_sou

# C++,Mat类到Python,numpy类的转换（解码）
def base64ToMat(base64_data):
    img = base64.b64decode(base64_data)
    nparr = np.fromstring(img, np.uint8)
    img_np = cv2.imdecode(nparr, 1)
    img_np = cv2.cvtColor(img_np, cv2.COLOR_BGR2RGB)
    return img_np


# Python,numpy类到C++,Mat类的转换（编码）
def matToBase64(rgb, img):
    if rgb:
        img = cv2.cvtColor(img, cv2.COLOR_RGB2BGR)  # RGB转BGR
    x = cv2.imencode(".jpg", img)[1].tobytes()
    base64_data = base64.b64encode(x)
    return base64_data


counter = 0
# 存储图片到本地
def savPicture(img):
    name = ".png"
    global counter
    counter += 1
    img_path = "../../res/samples/train/"
    name = img_path + str(counter) + ".png"
    cv2.imwrite(name, img)


capture = cv2.VideoCapture("../../res/samples/slowzone.mp4")  # 选择本地视频
capture.set(cv2.CAP_PROP_FRAME_WIDTH, 320)  # 设置视频分辨率
capture.set(cv2.CAP_PROP_FRAME_HEIGHT, 240)

# 检测视频是否接收到,加入exit()防止死循环
if not capture.isOpened():
    print("Cannot open camera")
    exit()
rate = int(capture.get(cv2.CAP_PROP_FPS))
width = int(capture.get(cv2.CAP_PROP_FRAME_WIDTH))
height = int(capture.get(cv2.CAP_PROP_FRAME_HEIGHT))
print("Camera Param: frame rate = ", rate, " width = ", width, " height = ", height)

# 初始化智能汽车类
icar = intelligentCar_sou.IntelligentCar()

# 相机矫正参数初始化
icar.imageCorrecteInit()

# AI-SSD模型初始化
icar.paddleModelInit()

while True:
    # 读取视频帧
    isTrue, imageFrame = capture.read()
    if isTrue:
        cv2.waitKey(1)
    else:
        print("视频播放完毕")
        break

    # 图像矫正
    base64Frame = matToBase64(False, imageFrame)
    base64Corret = icar.imageCorrection(base64Frame)
    imageCorret = base64ToMat(base64Corret)

    # RGB图像转换成灰度图像
    imageGray = cv2.cvtColor(imageCorret, cv2.COLOR_BGR2GRAY)

    # 图像二值化
    rel, imageBinary = cv2.threshold(imageGray, 0, 255, cv2.THRESH_OTSU)

    # 赛道识别
    base64Binary = matToBase64(False, imageBinary)
    icar.trackRecognition(base64Binary)
    icar.trackDrawImage(base64Corret)  # 可视化显示识别结果

    # 启动AI推理
    icar.AiDetection(base64Frame)

    # 慢行区检测与路径规划
    icar.slowzoneDetection()
    icar.slowzoneDrawImage(base64Corret)
    base64Slowzone = matToBase64(True, imageCorret)
    icar.slowzoneDrawImage(base64Slowzone)  # 可视化显示识别结果

    # 接上一段代码
    if isTrue:
        if cv2.waitKey(1) & 0xFF == ord("d"):
            # 按下d退出
            break
    else:
        break
# 接上一段代码
capture.release()
cv2.destroyAllWindows()
