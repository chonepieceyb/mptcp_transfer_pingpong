# -*- coding: UTF-8 -*-

import argparse 
import os
from subprocess import Popen
import time 
import signal
from common import *

#实验设置
flows = [10]

# 20 - 320K 
for i in range (1, 17): 
    flows.append(i * 20)

flows.extend([640, 1280, 2560, 5120, 10240, 20480, 40960, 409600, 4096000])

#flows = [10, 20 ,30 , 40]
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
    parser.add_argument("-c", "--count" , type=int, default= DEFAULT_REPEAT, help="repeat")
    parser.add_argument("-o","--output", type=str, default = DEFAULT_OUTPUT, help="output path")
    parser.add_argument("-i","--input", type=str, default = DEFAULT_PING_PONG_PATH, help="input client script")
    parser.add_argument("-a","--address", type=str, default = DEFAULT_ADDRESS, help="pear ip address")
    parser.add_argument("-p", "--port", type=int, default=DEFAULT_PORT, help = "server port")
    parser.add_argument("-H", "--high_cpu", action="store_true", help = "high cpu load")  #for mptcp

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

    #pingpong sudo client tcpflags -v -b send_buffer -a address -p port block
    pingpong_cmd = "sudo %s %s -v -b %d -a %s -p %d %d"
    send_buffer = DEFAULT_SEND_BUFFER

    #tcpdump cmd 
    tcpdump_normalexpr = "tcp port %d"
    tcpdump_bigexpr = "tcp port %d and (tcp[tcpflags] & (tcp-syn|tcp-fin) != 0)"

    for exp in exps: 
        flow = get_flow(exp)
        exp = os.path.join(exp_dir, exp)

        tcpdump_expr = tcpdump_normalexpr
        if (flow >= PKT_CAP_THRESDHOLD) : 
            tcpdump_expr = tcpdump_bigexpr

        if (args.high_cpu and flow >= PKT_BUFF_THRESDHOLD):
            send_buffer = HIGH_SEND_BUFFER
        else:
            send_buffer = DEFAULT_SEND_BUFFER

        tcp_dump_proc = Popen(args=["sudo", "tcpdump" , "-i", "any" , tcpdump_expr%args.port, "-w", exp], start_new_session=True, encoding='utf-8')   #start tcp dump
        time.sleep(3) 
        try :
            cmd = pingpong_cmd%(pingpong_client_path, tcp_flags, send_buffer, args.address, args.port, flow)
            print(cmd)
            res = os.system(cmd)
            if res : 
                raise RuntimeError("pingpong failed, exit : %d"%res)
            time.sleep(2) 
        except Exception as e:
            print(e)
        finally:
            os.killpg(tcp_dump_proc.pid, signal.SIGINT) 
