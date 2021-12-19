# -*- coding: UTF-8 -*-

import argparse 
import os
import subprocess 
from subprocess import Popen
import time 
import signal
from common import *

#实验设置
flows = []   # 从 10k 到 5G 10 20 40 80 160 320 ....
for i in range(20) :
    flows.append(10 * pow(2, i)) 

def gen_exps(flows_list, repeat) : 
    exps = []
    for flow in flows_list: 
        for index in range(1, repeat + 1): 
            exp_file_name = "%d-%d.pcap"%(flow, index)
            exps.append(exp_file_name)
    return exps

def get_flow(exp) :
    flow = exp.split('-')[0]
    return int(flow)
    
if __name__ == '__main__': 
    #exp [-t,--tcp] [-c] [-o] expname 
    parser = argparse.ArgumentParser(description="exp options")
    parser.add_argument("-t", "--tcp", action="store_true", help="use tcp")
    parser.add_argument("-c", "--count" , type=int, default= REPEAT, help="repeat")
    parser.add_argument("-o","--output", type=str, default = DEFAULT_OUTPUT, help="output path")
    parser.add_argument("-i","--input", type=str, default = DEFAULT_PING_PONG_PATH, help="input client script")
    parser.add_argument("-a","--address", type=str, default = DEFAULT_ADDRESS, help="pear ip address")
    parser.add_argument("-p", "--port", type=int, default=DEFAULT_PORT, help = "server port")

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
    exps = gen_exps(flows, args.count)
    print(exps)

    pingpong_client_path = args.input
    tcp_flags = ""
    if args.tcp : 
        tcp_flags = "-t"

    #pingpong sudo client tcpflags -a address -p port block
    pingpong_cmd = "sudo %s %s -v -a %s -p %d %d"
    #tcpdump cmd 
    tcpdump_normalexpr = "tcp port %d"
    tcpdump_bigexpr = "tcp port %d and (tcp[tcpflags] & (tcp-syn|tcp-fin) != 0)"

    for exp in exps: 
        flow = get_flow(exp)
        exp = os.path.join(exp_dir, exp)

        tcpdump_expr = tcpdump_normalexpr
        if (flow >= PKT_THRESDHOLD) : 
            tcpdump_expr = tcpdump_bigexpr
        tcp_dump_proc = Popen(args=["sudo", "tcpdump" , tcpdump_expr%args.port, "-w", exp],start_new_session=True,encoding='utf-8')   #start tcp dump
        time.sleep(1) 
        try :
            res = os.system(pingpong_cmd%(pingpong_client_path, tcp_flags, args.address, args.port, flow))
            if res : 
                raise RuntimeError("pingpong failed, exit : %d"%res)
            time.sleep(1) 
            os.killpg(tcp_dump_proc.pid, signal.SIGINT) 
        except Exception as e:
            print(e)
            os.killpg(tcp_dump_proc.pid, signal.SIGINT) 