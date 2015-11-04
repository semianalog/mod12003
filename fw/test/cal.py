#!/usr/bin/env python3

import subprocess
import struct
import sys
import time
import atexit

DRIVER="./raw.py"

CMD_CAL_COUNT = 0xf0
CMD_CAL_SELECT = 0xf1
CMD_CAL_NAME = 0xf2
CMD_CAL_STATUS = 0xf3
CMD_CAL_USER = 0xf4
CMD_CAL_RUN = 0xf5
CMD_CAL_SAVE = 0xf6
CMD_CAL_ABORT = 0xf7
CMD_CAL_READ = 0xf8
CMD_CAL_WRITE = 0xf9

CAL_STATE_IDLE  = 0
CAL_STATE_SEL   = 1
CAL_STATE_STOP  = 2
CAL_STATE_RUN   = 3
CAL_STATE_PAUSE = 4
CAL_STATE_FINI  = 5

def cal_count(addr):
    d = subprocess.check_output([DRIVER, "--raw", str(addr), str(CMD_CAL_COUNT)])
    return struct.unpack('H', d)[0]

def cal_select(addr, n):
    d = subprocess.check_output([DRIVER, "--raw", str(addr), str(CMD_CAL_SELECT), "u16", str(n)])

def cal_name(addr):
    d = subprocess.check_output([DRIVER, "--raw", str(addr), str(CMD_CAL_NAME)])
    return d.decode('ascii')

def cal_status(addr):
    d = subprocess.check_output([DRIVER, "--raw", str(addr), str(CMD_CAL_STATUS)])
    status = d[0]
    data = d[1:]
    return (status, data)

def cal_read(addr):
    d = subprocess.check_output([DRIVER, "--raw", str(addr), str(CMD_CAL_READ)])
    return d

def cal_save(addr):
    d = subprocess.check_output([DRIVER, "--raw", str(addr), str(CMD_CAL_SAVE)])

def cal_run(addr):
    d = subprocess.check_output([DRIVER, "--raw", str(addr), str(CMD_CAL_RUN)])

def cal_abort(addr):
    d = subprocess.check_output([DRIVER, "--raw", str(addr), str(CMD_CAL_ABORT)])

def cal_user(addr, n):
    d = subprocess.check_output([DRIVER, "--raw", str(addr), str(CMD_CAL_USER), "i32", str(n)])

def cal_full_abort(addr):
    cal_abort(addr)
    while True:
        status, data = cal_status(addr)
        if status == CAL_STATE_IDLE:
            break

if len(sys.argv) > 1:
    addr = int(sys.argv[1], 0)
else:
    addr = 0

atexit.register(lambda: cal_abort(addr))

count = cal_count(addr)
print("Routines:")
for i in range(count):
    cal_select(addr, i)
    name = cal_name(addr)
    print(" % 3d - %s" % (i, name))
    cal_abort(addr)
print()
selection = int(input("Choose a cal routine: "), 0)
cal_select(addr, selection)

while True:
    status, data = cal_status(addr)
    if status == CAL_STATE_IDLE:
        print("Waiting for cal to be selected...")
        time.sleep(0.5)
    elif status == CAL_STATE_SEL:
        break
    else:
        print("Invalid state %d!" % status)
        sys.exit(1)

def cal_cycle(addr):
    cal_run(addr)
    while True:
        status, data = cal_status(addr)
        if status == CAL_STATE_IDLE:
            break
        elif status == CAL_STATE_SEL:
            print("Waiting for cal to start...")
        elif status == CAL_STATE_STOP:
            print("Cal stopped with error: " + data.decode('ascii'))
            sys.exit(1)
        elif status == CAL_STATE_RUN:
            print("Cal running...")
        elif status == CAL_STATE_PAUSE:
            print("Cal requesting user data.")
            message, unit = data[0:16], data[16:20]
            message = message.rstrip(b"\x00").decode('ascii')
            unit = unit.rstrip(b"\x00").decode('ascii')
            userdata = int(input("%s (%s): " % (message, unit)), 0)
            cal_user(addr, userdata)
        elif status == CAL_STATE_FINI:
            print("Cal finished.")
            return
        else:
            print("Invalid state %d" % status)
            sys.exit(1)
        time.sleep(0.1)

while True:
    action = input("Action (Run/Query/Write/Save)? ")[0].lower()
    if action == "r":
        cal_cycle(addr)
    elif action == "q":
        print(repr(cal_read(addr)))
    elif action == "w":
        print ("Not implemented yet!")
    elif action == "s":
        cal_save(addr)

