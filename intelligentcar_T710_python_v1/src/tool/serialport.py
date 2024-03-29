#!/usr/bin/env python
# -*- encoding: utf-8 -*-
"""
@file          :serialport.py
@Description   :串口通信/收发协议
@Date          :2023/02/06 12:10:50
@Autor         :Leo
@Version       :v1.0
"""

from ctypes import *
import time
import math
import struct
import serial


def byte2float(x):
    return struct.unpack("<f", struct.pack("4b", *x))[0]  #将字节型数据转化为浮点型


def float2byte(f):
    return [i for i in struct.pack("<f", f)]              #将浮点型小数转化为字节型数据


def uint162byte(u16):
    u16=int(round(u16,16))                                #保留16位数并且转为整型数据
    return [i for i in struct.pack("<i", u16)]            #将整型数据转化为字节型数据


def print_cmd(bytes):
    l = [hex(int(i)) for i in bytes]
    print("Send Cmd [{}]".format(" ".join(l)))


frameHead = bytes.fromhex("42")  # USB帧头
frameLenMin = bytes.fromhex("04")  # USB通信帧最短长度（字节）
frameLenMax = bytes.fromhex("04")  # USB通信帧最短长度（字节）

USB_ADDR_CARCONTROL = bytes.fromhex("01")  # 车辆运动控制
USB_ADDR_MOVEMENT = bytes.fromhex("02")  # 位移控制
USB_ADDR_RGBLIGHT = bytes.fromhex("03")  # RGB灯光控制
USB_ADDR_BUZZER = bytes.fromhex("04")  # 蜂鸣器音效
USB_ADDR_BATTERY = bytes.fromhex("05")  # 电池信息
USB_ADDR_RANGE = bytes.fromhex("06")  # 距离数据
USB_ADDR_TRACK = bytes.fromhex("07")  # 巡线使能
USB_ADDR_RESET = bytes.fromhex("08")  # 系统复位
PWMSERVOMAX = 1900  # 舵机PWM最大值（左）
PWMSERVOMID = 1500  # 舵机PWM中值
PWMSERVOMIN = 1100  # 舵机PWM最小值（右）


class Uart:
    def __init__(self):
        portx = "/dev/ttyUSB0"  # 串口号
        bps = 115200  # 波特率
        self.serial = serial.Serial(
            portx, int(bps), timeout=1, parity=serial.PARITY_NONE, stopbits=1
        )
        self.speed = 0.0

    def send_cmd(self, addr, uartStream):
        length = len(uartStream) + 4
        usbFrame = (
            frameHead  # 帧头
            + addr  # 地址
            + length.to_bytes(1, byteorder="little", signed=False)  # 帧长
            + uartStream  # 数据
        )
        check = int(0)
        for i in usbFrame:
            check += i
            check = int(check % 256)
        usbFrame = usbFrame + check.to_bytes(1, byteorder="little", signed=False)
        self.serial.write(usbFrame)

    """
    @brief:     智能车速度和方向控制
    @params:    speed: 速度m/s  |   steer:方向(PWM占空比)
    """

    def carControl(self,speed, steer):
        uartStream = float2byte(speed) + uint162byte(steer)  # 数据
        self.send_cmd(USB_ADDR_CARCONTROL, bytes(uartStream))

    def change_control_mode(self, open_close):
        if open_close:
            payload = bytes.fromhex("01 00 00 00 00 00 00 00")  # 闭环
        else:
            payload = bytes.fromhex("00 00 00 00 00 00 00 00")  # 开环
        # self.send_cmd(control_model_cmd, bytes(payload))


if __name__ == "__main__":
    uart = Uart()

    while True:
        uart.carControl(0.3, PWMSERVOMID)
        time.sleep(3)
        break

    uart.carControl(0.0, PWMSERVOMID)
