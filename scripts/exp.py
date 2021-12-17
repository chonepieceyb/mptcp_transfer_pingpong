# -*- coding: UTF-8 -*-

import argparse
import sys 
import os 

#实验设置
data_size = ()   # 从 1M 到 8G 
for i in range(14) :
    data_size.append(1024 * pow(2, i)) 

repeat = 5   #每组数据重复做5次

if __name__ == '__main__': 
    #参数 -m/-t 使用 tcp 或者是mptcp 
    #title 实验标题: title_tcp/ title_mptcp 
    