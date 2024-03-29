#!/usr/bin/env python
# -*- encoding: utf-8 -*-
"""
Description:    图像透视变换|demo
Version:        v1.0
Autor:          Leo
Date:           2022年05月31日 19:55:06
LastEditors:    Leo
LastEditTime:   2022年05月31日 19:55:19
"""

import base64
import cv2
import numpy as np
import string
import os
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
def matToBase64(img):
    img = cv2.cvtColor(img, cv2.COLOR_RGB2BGR)
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


capture = cv2.VideoCapture("../../res/samples/demo.mp4")  # 读取本地视频
capture.set(cv2.CAP_PROP_FRAME_WIDTH, 320)  # 设置视频分辨率:320*240
capture.set(cv2.CAP_PROP_FRAME_HEIGHT, 240)


# 检测视频是否接收到,加入exit()防止死循环
if not capture.isOpened():
    print("Cannot open camera")
    exit()
rate = int(capture.get(cv2.CAP_PROP_FPS))
width = int(capture.get(cv2.CAP_PROP_FRAME_WIDTH))
height = int(capture.get(cv2.CAP_PROP_FRAME_HEIGHT))
print("Camera Param: frame rate = ", rate, " width = ", width, " height = ", height)

windowName = "imageIpm"
cv2.namedWindow(windowName, cv2.WINDOW_NORMAL)  # 图像名称
cv2.resizeWindow(windowName, 320 * 2, 240)  # 分辨率

# 初始化智能汽车类
icar = intelligentCar_sou.IntelligentCar()

# 透视变换（IPM）参数初始化
icar.ipmInit()

# 相机矫正参数初始化
icar.imageCorrecteInit()

while True:
    # 读取视频帧
    isTrue, imageFrame = capture.read()

    # 图像矫正
    base64Frame = matToBase64(imageFrame)
    base64Corret = icar.imageCorrection(base64Frame)
    imageCorret = base64ToMat(base64Corret)
    if isTrue:
        cv2.waitKey(1)
    else:
        print("视频播放完毕")
        break

    # 透视变换
    base64Ipm = icar.ipmHomography(base64Corret)
    imageIpm = base64ToMat(base64Ipm)

    imageIpm = cv2.resize(imageIpm, (320, 240))
    cv2.putText(
        imageFrame, "[1] frame", (10, 30), cv2.FONT_HERSHEY_TRIPLEX, 1, (255, 0, 0), 1
    )
    cv2.putText(
        imageIpm, "[2] imgIpm", (10, 30), cv2.FONT_HERSHEY_TRIPLEX, 1, (255, 0, 0), 1
    )
    preprocess = np.hstack((imageFrame, imageIpm))
    cv2.imshow("imageIpm", preprocess)

    # 接上一段代码
    if isTrue:
        # cv2.imshow('My Video',imageBinary)
        if cv2.waitKey(20) & 0xFF == ord("d"):
            # 按下d退出
            break
    else:
        break
# 接上一段代码
capture.release()
cv2.destroyAllWindows()
