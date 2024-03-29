# !/usr/bin/env python
# -*-coding:utf-8-*-
"""
Description:    图片合成视频|demo
Version:        v1.0
Autor:          Hjc
Date:           2023年02月21日 20:55:06
"""
import time
import cv2
import os


# 保存视频

video_dir = "../../res/samples/sample.mp4"  # 生成的视频路径和名称
width = 320  # 照片的大小
height = 240
fps = 30  # 视频的帧率
fourcc = cv2.VideoWriter_fourcc(*"mp4v")  # 将转化后的视频格式设置为mp4
videoWriter = cv2.VideoWriter(video_dir, fourcc, fps, (width, height))  # 将照片添加到视频中

img_dir = "../../res/samples/train/"

for i in range(5000):
    image = cv2.imread(img_dir + str(i + 1) + ".jpg")
    if image is not None:
        videoWriter.write(image)
        print("index = ", i + 1)

videoWriter.release()  # 资源释放

print("finished!")
