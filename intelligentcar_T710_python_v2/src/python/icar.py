#!/usr/bin/env python
# -*- encoding: utf-8 -*-
"""
Description:    智能汽车完整运行|demo
Version:        v1.0
Autor:          Hjc
Date:           2023年02月17日 20:55:06
"""

import sys

sys.path.append("../lib")  # 引用C++库
import carrun


icar = carrun.carrun()    # 实例化

if __name__ == "__main__":

    while True:
        icar.CarRuning()  # 运行整车

