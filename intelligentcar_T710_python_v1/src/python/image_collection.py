#!/usr/bin/env python
# -*- encoding: utf-8 -*-
"""
@file          :image_collection.py
@Description   :
@Date          :2023/02/20 17:20:31
@Autor         :Hjc
@Version       :v1.0
"""

import threading
import cv2
import time
import os
import struct
import array
from fcntl import ioctl
import sys

sys.path.append("../tool")
from serialport import Uart

PWMSERVOMAX = 1900  # 舵机PWM最大值（左）
PWMSERVOMID = 1500  # 舵机PWM中值
PWMSERVOMIN = 1100  # 舵机PWM最小值（右）
JOYSTICK_MODE = 3  #  2/无线手柄A（单绿灯常亮）   3/无线手柄B（单红灯常亮））


class JoyStick:
    def __init__(self):
        print("avaliable devices")
        for fn in os.listdir("/dev/input"):
            if fn.startswith("js"):
                print("/dev/input/%s" % fn)

        self.fn = "/dev/input/js0"
        self.x_axis = 0

    def open(self):
        self.jsdev = open(self.fn, "rb")

    def read(self):
        self.evbuf = self.jsdev.read(8)
        return struct.unpack("IhBB", self.evbuf)

    def type(self, type_):
        if type_ & 0x01:
            return "button"
        if type_ & 0x02:
            return "axis"

    def button_state(self):
        return 1

    def get_x_axis(self):
        time, value, type_, number = struct.unpack("IhBB", self.evbuf)
        if number == 1:
            fvalue = value / 32767
            return fvalue


js = JoyStick()
inital = False  # 遥控手柄初始化屏蔽
uart = Uart()


class Collecte:
    def __init__(self):
        self.camera = cv2.VideoCapture(0)
        self.speed = 0.0  # 车速：m/s
        self.forward = True  # 车辆行驶方向
        self.steer = PWMSERVOMID
        self.appExit = False  # 程序退出标志
        self.cotMore = False  # 连续图像采样使能
        self.cotOnce = False  # 单次图像采样使能
        self.index = 0
        self.counterHeart = 0  # 串口发送心跳计数器
        self.saveDir = "../../res/samples/train"
        if not os.path.exists(self.saveDir):
            os.makedirs(self.saveDir)
        if not self.camera.isOpened():
            print("camera open faild!")
            self.exit()

        codec = cv2.VideoWriter_fourcc("M", "J", "P", "G")
        self.camera.set(cv2.CAP_PROP_FOURCC, codec)
        self.camera.set(cv2.CAP_PROP_FPS, 30)
        self.camera.set(cv2.CAP_PROP_FRAME_WIDTH, 320)
        self.camera.set(cv2.CAP_PROP_FRAME_HEIGHT, 240)

    # 高速运行
    def speedHigh(self):
        if self.forward:
            self.speed = 0.7
        else:
            self.speed = -0.7
        uart.carControl(self.speed, self.steer)

    # 低速运行
    def speedLow(self):
        if self.forward:
            self.speed = 0.3
        else:
            self.speed = -0.3
        uart.carControl(self.speed, self.steer)

    # 连续采图
    def collecteMore(self):
        self.cotMore = True
        self.cotOnce = False

    # 单帧采图
    def collecteOnce(self):
        self.cotMore = False
        self.cotOnce = True

    # 停止采图
    def collecteStop(self):
        self.cotMore = False
        self.cotOnce = False

    # 向前运动
    def frontRun(self):
        self.forward = True
        if self.speed < 0:
            self.speed = -self.speed

    # 向后运动
    def backRun(self):
        self.forward = False
        if self.speed > 0:
            self.speed = -self.speed

    # 左右转
    def turn(self, turn):
        self.steer = PWMSERVOMID + turn * (PWMSERVOMID - PWMSERVOMIN) / 32767
        uart.carControl(self.speed, self.steer)

    # 图像采集
    def sample(self):
        if self.counterHeart > 5:
            uart.carControl(self.speed, self.steer)
            self.counterHeart = 0
        else:
            self.counterHeart += 1

        return_value, image = self.camera.read()
        if self.cotMore or self.cotOnce:
            path = "{}/{}.jpg".format(self.saveDir, self.index)
            cv2.imwrite(path, image)
            print("saveimage: {}.jpg".format(self.index))
            self.index += 1

            if self.cotOnce:
                self.cotOnce = False

        cv2.putText(
            image,
            str(self.index),
            (10, 30),
            cv2.FONT_HERSHEY_SIMPLEX,
            1,
            (0, 0, 254),
            1,
            cv2.LINE_AA,
        )
        cv2.imshow("frame", image)  # UI显示
        cv2.waitKey(10)

    # 停车
    def carStop(self):
        self.speed = 0.0
        uart.carControl(self.speed, steer=1500)

    # 程序状态
    def state(self):
        return self.appExit

    # 线程退出
    def exit(self):
        self.appExit = True
        self.carStop()


collecte = Collecte()

# 遥控手柄线程


def joystick_thread():
    js.open()
    while not collecte.state():
        time, value, type_, number = js.read()
        if inital and JOYSTICK_MODE == 2:
            if js.type(type_) == "button":
                # print("button:{} state: {}".format(number, value))

                if number == 0 and value > 0:  # 连续采图
                    collecte.collecteMore()
                elif number == 3 and value > 0:  # 单帧采图
                    collecte.collecteOnce()
                elif number == 2 and value > 0:  # 停止采图
                    collecte.collecteStop()
                elif number == 5:  # 高速档
                    if value == 1:
                        collecte.speedHigh()
                    elif value == 0:
                        collecte.carStop()
                elif number == 7:  # 低速档
                    if value == 1:
                        collecte.speedLow()
                    elif value == 0:
                        collecte.carStop()
                else:
                    collecte.carStop()

            if js.type(type_) == "axis":
                # print("axis:{} state: {}".format(number, value))

                if number == 1:  # 车辆行驶方向
                    if value < 0:
                        collecte.frontRun()
                    elif value > 0:
                        collecte.backRun()
                elif number == 0:  # 左右转向移动
                    collecte.turn(value)
        if inital and JOYSTICK_MODE == 3:
            if js.type(type_) == "button":
                # print("button:{} state: {}".format(number, value))

                if number == 3 and value > 0:  # 连续采图
                    collecte.collecteMore()
                elif number == 2 and value > 0:  # 单帧采图
                    collecte.collecteOnce()
                elif number == 0 and value > 0:  # 停止采图
                    collecte.collecteStop()
                elif number == 5:  # 高速档
                    if value == 1:
                        collecte.speedHigh()
                    elif value == 0:
                        collecte.carStop()
                else:
                    collecte.carStop()

            if js.type(type_) == "axis":
                # print("axis:{} state: {}".format(number, value))

                if number == 1:  # 车辆行驶方向
                    if value < 0:
                        collecte.frontRun()
                    elif value > 0:
                        collecte.backRun()
                elif number == 0:  # 左右转向移动
                    collecte.turn(value)
                elif number == 5:  # 低速档
                    if value >= 1:
                        collecte.speedLow()
                    elif value <= 0:
                        collecte.carStop()

    print("JoyStick's thread out!")


if __name__ == "__main__":
    # 开启遥控手柄多线程
    threadJok = threading.Thread(target=joystick_thread, args=())
    threadJok.start()

    time.sleep(1)
    inital = True

    while not collecte.state():
        collecte.sample()

    collecte.exit()
    threadJok.join()
    uart.carControl(0, PWMSERVOMID)
