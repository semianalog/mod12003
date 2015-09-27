#!/usr/bin/env python3

import crcmod
import serial
import sys

CRC16 = crcmod.predefined.mkCrcFun('crc-ccitt-false')

s = serial.Serial("/dev/ttyUSB0", 115200, timeout=1)

address = int(sys.argv[1], 0)
command = int(sys.argv[2], 0)
data = [int(i, 0) for i in sys.argv[3:]]

Taddress = bytes([address & 0xff])
Tcommand = bytes([command & 0xff, (command & 0xff00) >> 8])
Tdatalen = bytes([len(data) & 0xff, (len(data) & 0xff00) >> 8])
Tdata = bytes(data)

msg = Taddress + Tcommand + Tdatalen + Tdata
crc = CRC16(msg)

Tcrc = bytes([crc & 0xff, (crc & 0xff00) >> 8])
Tcrc = bytes([(crc & 0xff00) >> 8, crc & 0xff]) # BIG ENDIAN
msg += Tcrc

print("Transmit: %s" % list(hex(i) for i in msg))

s.write(msg)

response = s.read(1024)

print("Receive: %s" % list(hex(i) for i in response))

Raddress = response[0]
Rcommand = response[1] + (response[2] << 8)
Rdatalen = response[3] + (response[4] << 8)
Rdata = response[5:-2]
Rcrc = response[-2] + (response[-1] << 8)

print("Address: %d" % Raddress)
print("Command: %04x" % Rcommand)
print("Datalen: %d" % Rdatalen)
print("CRC: %d" % Rcrc)

if CRC16(response) == 0:
    print("CRC is valid")
else:
    print("CRC is invalid")

print()
print(list(hex(i) for i in Rdata))
