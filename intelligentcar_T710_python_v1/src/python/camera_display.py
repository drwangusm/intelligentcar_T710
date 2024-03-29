#!/usr/bin/env python
# -*- encoding: utf-8 -*-
"""
Description:    摄像头采图显示（矫正机械结构）|demo
Version:        v1.0
Autor:          Leo
Date:           2022年05月31日 19:55:06
LastEditors:    Leo
LastEditTime:   2022年05月31日 19:55:19
"""

import cv2

# 接收视频数据
cap = cv2.VideoCapture(0)
# 设定视频分辨率
cap.set(cv2.CAP_PROP_FRAME_WIDTH, 320)
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 240)

# 检测视频是否接收到,加入exit()防止死循环
if not cap.isOpened():
    print("Cannot open camera")
    exit()
# 注意最后放exit()，没有可能会卡死
rate = int(cap.get(cv2.CAP_PROP_FPS))
width = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
height = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
print("Camera Param: frame rate = ", rate, " width = ", width, " height = ", height)

while 1:
    ret, frame = cap.read()

    if not ret:
        print("Cannot open camera")
        exit()
    k = cv2.waitKey(1)
    # 设定退出信号
    if k == ord("q"):
        break

    # 绘制田字格：基准线
    rows = int(240 / 30)
    cols = int(320 / 32)

    for i in range(1, rows):
        ptStart = (0, 30 * i)
        ptEnd = (width - 1, 30 * i)
        cv2.line(frame, ptStart, ptEnd, (211, 211, 211), 1)
    for i in range(1, cols):
        if i == cols / 2:
            ptStart = (32 * i, 0)
            ptEnd = (32 * i, height - 1)
            cv2.line(frame, ptStart, ptEnd, (0, 0, 255), 1)
        else:
            ptStart = (32 * i, 0)
            ptEnd = (32 * i, height - 1)
            cv2.line(frame, ptStart, ptEnd, (211, 211, 211), 1)
    # 视频窗口名称设定与显示
    cv2.imshow("frame", frame)
    cv2.waitKey(10)

cap.release()
