# -*- coding: UTF-8 -*-
import os 
import argparse
import time
from subprocess import Popen
from common import *
import signal

BATCH_SCRIPT_DIR = os.path.join(SCRIPT_DIR, "exp_batch.py")
OUTPUT_DIR = "/home/ubuntu/exp_resultv2"

stress_batch = [40, 38, 36, 32, 20, 10, 0]
#stress_batch = [0, 1]

high_stress_threshold = 30

if __name__ == '__main__' :
    parser = argparse.ArgumentParser(description="batch exp options")
    parser.add_argument("-c", "--count" , type=int, default= DEFAULT_REPEAT, help="repeat")
    parser.add_argument("-a","--address", type=str, default = DEFAULT_ADDRESS, help="pear ip address")
    parser.add_argument("-p", "--port", type=int, default=DEFAULT_PORT, help = "server port")
    parser.add_argument("-o", "--output", type=str, default=OUTPUT_DIR, help = "output_dir")

    parser.add_argument("-n", "--normal_mptcp", action="store_true", help = "run normal mptcp")
    parser.add_argument("-t", "--tcp", action="store_true", help = "run tcp")
    parser.add_argument("-x", "--mptcp_xdp", action="store_true", help = "run mptcp xdp")
    args = parser.parse_args()
    
    assert(args.normal_mptcp or args.tcp or args.mptcp_xdp)
    mode_flags = "-"
    if args.tcp : 
        mode_flags += 't'
    if args.normal_mptcp:
        mode_flags += 'n'
    if args.mptcp_xdp: 
        mode_flags += 'x'

    #sudo python batch_exp.py mode_flags -a address -p port -c count -o output_dir high_flg scene_name
    bacth_script_cmd_base = "sudo python %s %s -a %s -p %d -c %d -o %s"%(BATCH_SCRIPT_DIR, mode_flags, args.address, args.port, args.count, args.output)
    high_flag = ""

    clean_cmd = "sudo kill -9 $(pidof stress)"

    for stress in stress_batch:
        scene = "high_%d"%stress
        if stress >= high_stress_threshold:
            high_flag = '-H'
        else:
            high_flag = ""

        batch_exp_cmd = "%s %s %s"%(bacth_script_cmd_base, high_flag, scene)
        print("stress: %d"%stress)
        print(batch_exp_cmd)
        
        #先开stress 
        stress_proc = None
        if stress != 0: 
            stress_proc = Popen(args=["sudo", "stress", "-c", str(stress)],start_new_session=True, encoding='utf-8')
            time.sleep(5)
        
        #run batch exp script 
        os.system(batch_exp_cmd)
        time.sleep(3)
        
        if stress !=0 :
            os.system(clean_cmd)
        
        time.sleep(3)