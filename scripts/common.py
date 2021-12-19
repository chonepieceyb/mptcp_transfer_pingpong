# -*- coding: UTF-8 -*-
import os

REPEAT = 10   #每组数据重复做5次
SCRIPT_DIR  = os.path.dirname(os.path.abspath(__file__))
PROJECT_DIR = os.path.dirname(SCRIPT_DIR)
BIN_DIR = os.path.join(PROJECT_DIR, "bin")

DEFAULT_OUTPUT = os.path.join(SCRIPT_DIR, "result")
DEFAULT_PING_PONG_PATH = os.path.join(BIN_DIR, "client")
DEFAULT_ADDRESS = "223.3.71.76"
DEFAULT_PORT = 60000
PKT_THRESDHOLD = 10240  #10M 
