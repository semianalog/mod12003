#!/usr/bin/env python3

import crcmod
import serial
import sys
import struct

packed_args_structs = {
    "u8":   "B",
    "u16":  "H",
    "u32":  "I",
    "i8":   "b",
    "i16":  "h",
    "i32":  "i",
}

CRC16 = crcmod.predefined.mkCrcFun('crc-ccitt-false')

s = serial.Serial("/dev/ttyUSB0", 115200, timeout=1)

if sys.argv[1] == "--raw":
    RAW = True
    sys.argv.pop(0)
else:
    RAW = False

address = int(sys.argv[1], 0)
command = int(sys.argv[2], 0)

data = []
args = sys.argv[3:][::-1]
while args:
    arg = args.pop()
    if arg in packed_args_structs:
        value_arg = args.pop()
        data.extend(struct.pack(packed_args_structs[arg], int(value_arg, 0)))
    else:
        data.append(int(arg, 0))


Taddress = bytes([address & 0xff])
Tcommand = bytes([command & 0xff])
Tdatalen = bytes([len(data) & 0xff, (len(data) & 0xff00) >> 8])
Tdata = bytes(data)

msg = b"\x00" + Tcommand + Tdatalen + Tdata
crc = CRC16(msg)

Tcrc = bytes([crc & 0xff, (crc & 0xff00) >> 8])
Tcrc = bytes([(crc & 0xff00) >> 8, crc & 0xff]) # BIG ENDIAN

msg = Taddress + Tcommand + Tdatalen + Tdata + Tcrc

if not RAW:
    print("Transmit: %s" % list(hex(i) for i in msg))

s.write(msg)

header = s.read(4)

Raddress = header[0]
Rcommand = header[1]
Rdatalen = header[2] + (header[3] << 8)

response = header + s.read(Rdatalen + 2)

Rdata = response[4:-2]
Rcrc = response[-2] + (response[-1] << 8)

if RAW:
    if CRC16(response) == 0:
        sys.stdout.buffer.write(Rdata)
    else:
        sys.exit(1)
else:
    print("Receive: %s" % list(hex(i) for i in response))

    print("Address: %d" % Raddress)
    print("Command: %02x" % Rcommand)
    print("Datalen: %d" % Rdatalen)
    print("CRC: %d" % Rcrc)

    if CRC16(response) == 0:
        print("CRC is valid")
    else:
        print("CRC is invalid")

    print()
    print(list(hex(i) for i in Rdata))
    print(Rdata)
