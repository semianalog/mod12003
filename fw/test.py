#!/usr/bin/env python3

import serial
import sys

s = serial.Serial("/dev/ttyUSB0", 115200, timeout=1)



address = int(sys.argv[1], 0)
command = int(sys.argv[2], 0)
data = [int(i, 0) for i in sys.argv[3:]]

transmission = bytes([address & 0xff, command & 0xff, (command & 0xff00) >> 8,
    len(data) & 0xff, (len(data) & 0xff00) >> 8]) + bytes(data) + bytes([0, 0])

print("Transmit: %s" % transmission)

s.write(transmission)

response = s.read(1024)

print("Receive: %s" % response)

Raddress = response[0]
Rcommand = response[1] + (response[2] << 8)
Rdatalen = response[3] + (response[4] << 8)
Rdata = response[5:-2]
Rcrc = response[-2] + (response[-1] << 8)

print("Address: %d" % Raddress)
print("Command: %04x" % Rcommand)
print("Datalen: %d" % Rdatalen)
print("CRC: %d" % Rcrc)
print()
print(Rdata)
