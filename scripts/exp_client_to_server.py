# -*- coding: UTF-8 -*-

import argparse 
import os
import subprocess 
from subprocess import Popen
import time 
import signal
#实验设置
'''
flows = []   # 从 10k 到 320M 10 20 40 80 160 320 ....
for i in range(15) :
    flows.append(10 * pow(2, i)) 
'''

flows = [60]
REPEAT = 10   #每组数据重复做5次
FILE_DIR = os.path.abspath(os.path.dirname(os.path.abspath(__file__)))
DEFAULT_OUTPUT = os.path.join(FILE_DIR, "result")
DEFAULT_PING_PONG_PATH = "/home/chonepieceyb/CODING/WorkSpace/mptcp_transfer_pingpong/bin/client"

ADDRESS = "223.3.71.76"
PORT = 60000

def gen_exps(flows_list, repeat, base_path) : 
    exps = []
    for flow in flows_list: 
        for index in range(1, repeat + 1): 
            exp_file_name = "%d-%d.pcap"%(flow, index)
            exps.append(os.path.join(base_path, exp_file_name))
    return exps

if __name__ == '__main__': 
    #exp [-t,--tcp] [-c] [-o] expname 
    parser = argparse.ArgumentParser(description="exp options")
    parser.add_argument("-t", "--tcp", action="store_true", help="use tcp")
    parser.add_argument("-c", "--count" , type=int, default= REPEAT, help="repeat")
    parser.add_argument("-o","--output", type=str, default = DEFAULT_OUTPUT, help="output path")
    parser.add_argument("-i","--input", type=str, default = DEFAULT_PING_PONG_PATH, help="input client script")
    parser.add_argument("-a","--address", type=str, default = ADDRESS, help="pear ip address")
    parser.add_argument("-p", "--port", type=int, default=PORT, help = "server port")

    parser.add_argument(metavar="exp_name", dest="exp_name", type=str, help="exp name")
    
    args = parser.parse_args()

    output_dir = os.path.abspath(args.output)
    if not os.path.exists(output_dir) :
        os.mkdir(output_dir)
    
    exp_dir = os.path.join(output_dir, args.exp_name)
    if os.path.exists(exp_dir) : 
        print("exp %s exitst!"%exp_dir)
        exit(-1)

    os.mkdir(exp_dir)
    exps = gen_exps(flows, args.count, exp_dir)
    print(exps)

    pingpong_client_path = args.input
    tcp_flags = ""
    if args.tcp : 
        tcp_flags = "-t"

    #pingpong sudo client tcpflags -a address -p port block
    pingpong_cmd = "sudo %s %s -v -a %s -p %d %d"
    #tcpdump cmd 
    tcpdump_cmd = "sudo tcpdump tcp port %d -w %s"

    for i in range(len(flows)): 
        flow = flows[i]
        exp = exps[i]
        #tcp_dump_proc = Popen(args=tcpdump_cmd %(args.port, exp), shell=True,encoding='utf-8')   #start tcp dump
        tcp_dump_proc = Popen(args=["sudo", "tcpdump", "tcp", "port", str(args.port), "-w", exp],start_new_session=True,encoding='utf-8')   #start tcp dump
        time.sleep(3) 
        try :
            res = os.system(pingpong_cmd%(pingpong_client_path, tcp_flags, args.address, args.port, flow))
            if res : 
                raise RuntimeError("pingpong failed, exit : %d"%res)
            time.sleep(3) 
            os.killpg(tcp_dump_proc.pid, signal.SIGINT) 
        except Exception as e:
            print(e)
            os.killpg(tcp_dump_proc.pid, signal.SIGINT) 