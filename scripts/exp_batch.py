# -*- coding: UTF-8 -*-
import argparse 
import os
from subprocess import Popen, check_call
import time 
import signal
from common import *

#批量执行 tcp mptcp mptcp_withxdp

DEFAULT_XDP_SCRIPT = os.path.join(XDP_PROJECT_DIR, "src_py", "mptcp_xdp_loader.py")
BATCH_DEFAULT_OUTPUT = DEFAULT_OUTPUT
EXP_SCRIPT = os.path.join(SCRIPT_DIR, "exp_client_to_server.py")  #之后直接改这个值就行

if __name__ == '__main__': 
    parser = argparse.ArgumentParser(description="batch exp options")
    parser.add_argument("-c", "--count" , type=int, default= DEFAULT_REPEAT, help="repeat")
    parser.add_argument("-o","--output", type=str, default = BATCH_DEFAULT_OUTPUT, help="output path")
    parser.add_argument("--xdp", type=str, default = DEFAULT_XDP_SCRIPT, help="input client script")
    parser.add_argument("-a","--address", type=str, default = DEFAULT_ADDRESS, help="pear ip address")
    parser.add_argument("-p", "--port", type=int, default=DEFAULT_PORT, help = "server port")
    parser.add_argument("-H", "--high_cpu", action="store_true", help = "high cpu load")

    parser.add_argument("-n", "--normal_mptcp", action="store_true", help = "run normal mptcp")
    parser.add_argument("-t", "--tcp", action="store_true", help = "run tcp")
    parser.add_argument("-x", "--mptcp_xdp", action="store_true", help = "run mptcp xdp")

    parser.add_argument(metavar="scene", dest="scene", type=str, help="scene name")

    args = parser.parse_args()
    

    high_cpu_flags = ""
    if (args.high_cpu):
        high_cpu_flags = "-H"

    #sudo python3 exp.py high_cpu_flag -a addr -p port -o output expname
    exp_mptcp_cmd_base = "sudo python3 %s %s -c %d -a %s -p %d -o %s"%(EXP_SCRIPT, high_cpu_flags, args.count, args.address, args.port, args.output)
    exp_tcp_cmd_base = "sudo python3 %s %s -t -c %d -a %s -p %d -o %s"%(EXP_SCRIPT, high_cpu_flags, args.count, args.address, args.port, args.output)

    # sudo python xdp_scripts.py -d 
    move_xdp_cmd = "sudo python %s -d --all"%(args.xdp)

    #move xdp first 
    res = os.system(move_xdp_cmd)
    if (res) : 
        print("move xdp failed %d"%res)
        exit(-1)
 
    #test tcp 
    if args.tcp: 
        exp_tcp_cmd = "%s %s-tcp"%(exp_tcp_cmd_base, args.scene)
        res = os.system(exp_tcp_cmd)
        if (res):
            print("tcp exp failed %d"%res)
            exit(-1)
    
    #normal mptcp
    if args.normal_mptcp:
        exp_mptcp_normal_cmd = "%s %s-mptcp_normal"%(exp_mptcp_cmd_base, args.scene)
        res = os.system(exp_mptcp_normal_cmd)
        if (res):
            print("mptcp normal exp failed %d"%res)
            exit(-1)
    
    #attach xdp
    if args.mptcp_xdp:
        xdp_proc = Popen(args=["sudo", "python", args.xdp, "-a", "--all"], start_new_session=True, encoding='utf-8')   #start tcp dump
        time.sleep(20) 

        try :
            exp_mptcp_xdp_cmd = "%s %s-mptcp_xdp"%(exp_mptcp_cmd_base, args.scene)
            res = os.system(exp_mptcp_xdp_cmd)
            if (res):
                print("mptcp xdp exp failed %d"%res)
                raise RuntimeError("mptcp xdp exp failed %d"%res)
            time.sleep(2)       
        except Exception as e:
            print(e)
        finally:
            os.killpg(xdp_proc.pid, signal.SIGKILL)

    # sudo python xdp_scripts.py -d 
    os.system(move_xdp_cmd)