#!/usr/bin/env python
# Panel mockup

DRIVER = "./raw.py"

CMD_COUNT = 0x10
CMD_QOUTPUT  = 0x90
CMD_QVOLTAGE = 0x91
CMD_QCURRENT = 0x92

import curses
import struct
import subprocess
import time

def cmd(addr, command, *data):
    return subprocess.check_output([DRIVER, "--raw", str(addr), str(command)] + [str(i) for i in data])

def count():
    return struct.unpack("B", cmd(0, CMD_COUNT, 0))[0]

def qvoltage(addr):
    return struct.unpack("<I", cmd(0, CMD_QVOLTAGE))[0] / 1000

def qcurrent(addr):
    return struct.unpack("<I", cmd(0, CMD_QCURRENT))[0] / 1000000

def draw_status(stdscr, mod_addr):
    num = mod_addr + 1

    volts = qvoltage(mod_addr)
    amps = qcurrent(mod_addr)

    stdscr.addstr(mod_addr, 0, '%d: %6.3f V   %5.2f A' % (num, volts, amps))

def main(stdscr):
    stdscr.clear()

    N_MODS = count()

    blip = False

    while True:

        if blip:
            stdscr.addstr(10, 0, "*")
        else:
            stdscr.addstr(10, 0, " ")
        blip = not blip

        for i in range(N_MODS):
            draw_status(stdscr, i)

        stdscr.refresh()
        #stdscr.getkey()
        time.sleep(0.5)

curses.wrapper(main)
