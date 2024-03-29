#!/usr/bin/env python
# -*- encoding: utf-8 -*-
"""
Description:    图像预处理|demo
Version:        v1.0
Autor:          Leo
Date:           2022年05月31日 19:55:06
LastEditors:    Leo
LastEditTime:   2022年05月31日 19:55:19
"""


import base64
import cv2
import numpy as np
import sys

sys.path.append("../lib")  # 引用C++库
import intelligentCar_sou  # 引用图像预处理库

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


capture = cv2.VideoCapture("../../res/samples/demo.mp4")  # 选择本地视频
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

windowName = "preprocess"
cv2.namedWindow(windowName, cv2.WINDOW_NORMAL)  # 图像名称
cv2.resizeWindow(windowName, 320 * 3, 240)  # 分辨率

# 初始化智能汽车类
icar = intelligentCar_sou.IntelligentCar()

# 相机矫正参数初始化
icar.imageCorrecteInit()

while True:
    # 读取视频帧
    isTrue, imageFrame = capture.read()
    if isTrue:
        cv2.waitKey(1)
    else:
        print("视频播放完毕")
        break
    # 图像矫正
    base64Frame = matToBase64(imageFrame)
    base64Corret = icar.imageCorrection(base64Frame)
    imageCorret = base64ToMat(base64Corret)

    # RGB图像转换成灰度图像
    imageGray = cv2.cvtColor(imageCorret, cv2.COLOR_BGR2GRAY)

    # 图像二值化
    rel, imageBinary = cv2.threshold(imageGray, 0, 255, cv2.THRESH_OTSU)
    imageBinary = np.expand_dims(imageBinary, axis=2)  # 将单通道灰度图转换成RGB
    imageBinary = np.concatenate((imageBinary, imageBinary, imageBinary), axis=-1)
    cv2.putText(
        imageFrame, "[1] frame", (10, 30), cv2.FONT_HERSHEY_TRIPLEX, 1, (255, 0, 0), 1
    )
    cv2.putText(
        imageCorret,
        "[2] imgCorret",
        (10, 30),
        cv2.FONT_HERSHEY_TRIPLEX,
        1,
        (255, 0, 0),
        1,
    )
    cv2.putText(
        imageBinary,
        "[3] imgBinary",
        (10, 30),
        cv2.FONT_HERSHEY_TRIPLEX,
        1,
        (255, 0, 0),
        1,
    )
    preprocess = np.hstack((imageFrame, imageCorret))
    preprocess = np.hstack((preprocess, imageBinary))

    cv2.imshow("preprocess", preprocess)

    # 接上一段代码
    if isTrue:
        if cv2.waitKey(20) & 0xFF == ord("d"):
            # 按下d退出
            break
    else:
        break
# 接上一段代码
capture.release()
cv2.destroyAllWindows()
