EESchema Schematic File Version 2
LIBS:Xilinx
LIBS:Vishay
LIBS:TexasInstruments
LIBS:symbol
LIBS:supertex
LIBS:st_ic
LIBS:ST
LIBS:skyworks
LIBS:silabs
LIBS:semi-trans-Vishay
LIBS:semi-trans-Toshiba
LIBS:semi-trans-TIP
LIBS:semi-trans-TI
LIBS:semi-trans-ST
LIBS:semi-trans-PZT
LIBS:semi-trans-OnSemi
LIBS:semi-trans-NXP
LIBS:semi-trans-MPS
LIBS:semi-trans-MMB
LIBS:semi-trans-misc
LIBS:semi-trans-IXYS
LIBS:semi-trans-IntRect
LIBS:semi-trans-Infineon
LIBS:semi-trans-Fairchild
LIBS:semi-trans-EPC
LIBS:semi-trans-DiodesInc
LIBS:semi-trans-BC
LIBS:semi-trans-AOS
LIBS:semi-trans-2N
LIBS:semi-thyristor-2N
LIBS:semi-opto-misc
LIBS:semi-diode-Vishay
LIBS:semi-diode-ST
LIBS:semi-diode-OnSemi
LIBS:semi-diode-NXP
LIBS:semi-diode-Murata
LIBS:semi-diode-MMB
LIBS:semi-diode-MCC
LIBS:semi-diode-DiodesInc
LIBS:semi-diode-BA
LIBS:semi-diode-1N
LIBS:_semi
LIBS:Recom
LIBS:power
LIBS:pcb
LIBS:pasv-xtal
LIBS:pasv-res
LIBS:pasv-ind
LIBS:pasv-cap
LIBS:pasv-Bourns
LIBS:pasv-BiTech
LIBS:passive
LIBS:onsemi
LIBS:NXP
LIBS:Micron
LIBS:Microchip
LIBS:mechanical
LIBS:maxim
LIBS:MACOM
LIBS:logic-7400
LIBS:logic-4000
LIBS:Littelfuse
LIBS:LinearTech
LIBS:_linear
LIBS:Lattice
LIBS:Intersil
LIBS:FTDI
LIBS:Fairchild
LIBS:_electromech
LIBS:display
LIBS:DiodesInc
LIBS:conn-test
LIBS:conn-te
LIBS:conn-tagconnect
LIBS:conn-molex
LIBS:conn-linx
LIBS:conn-fci
LIBS:conn-cui
LIBS:conn-100mil
LIBS:conn-2mm
LIBS:Atmel
LIBS:AnalogDevices
LIBS:Altera
LIBS:AOS
LIBS:mod12003-cache
EELAYER 25 0
EELAYER END
$Descr A 11000 8500
encoding utf-8
Sheet 1 6
Title "20V, 3A hybrid power supply module"
Date "2015-08-18"
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Sheet
S 3600 2200 1200 1100
U 55C7F4BB
F0 "Preregulator" 60
F1 "Preregulator.sch" 60
F2 "P_OUT" O R 4800 2400 50 
F3 "P_IN" I L 3600 2400 50 
F4 "SW" I L 3600 3100 50 
$EndSheet
$Sheet
S 5400 2200 1000 3200
U 55C7FF09
F0 "Linear" 60
F1 "Linear.sch" 60
F2 "VSET" I L 5400 4300 50 
F3 "ISET" I L 5400 4200 50 
F4 "VSENSE+" I R 6400 2500 50 
F5 "VSENSE-" I R 6400 3600 50 
F6 "ISENSE" I R 6400 3700 50 
F7 "P_IN" I L 5400 2400 50 
F8 "P_OUT" O R 6400 2400 50 
F9 "SCR_FIRE" O R 6400 3150 50 
F10 "OVP_THRESH" I L 5400 4100 50 
F11 "~EN" I L 5400 4000 50 
F12 "VSENSE_OUT" O L 5400 4700 50 
F13 "ISENSE_OUT" O L 5400 4800 50 
F14 "~VLIM" O L 5400 4900 50 
F15 "~ILIM" O L 5400 5000 50 
F16 "~SCR_TRIPPED" O L 5400 5100 50 
$EndSheet
$Comp
L CONN-100MIL-M-1x1 J4
U 1 1 55CB99FA
P 10150 2400
F 0 "J4" H 10269 2447 50  0000 L CNN
F 1 "OUT+" H 10269 2347 50  0000 L CNN
F 2 "~" H 10150 2400 50  0001 C CNN
F 3 "" H 10150 2400 50  0000 C CNN
	1    10150 2400
	1    0    0    -1  
$EndComp
$Comp
L CONN-100MIL-M-1x1 J5
U 1 1 55CB9ABB
P 10150 3700
F 0 "J5" H 10269 3747 50  0000 L CNN
F 1 "OUT-" H 10269 3647 50  0000 L CNN
F 2 "~" H 10150 3700 50  0001 C CNN
F 3 "" H 10150 3700 50  0000 C CNN
	1    10150 3700
	1    0    0    -1  
$EndComp
$Comp
L NET-TIE W2
U 1 1 55CB9B96
P 9650 2500
F 0 "W2" H 9650 2417 50  0000 C CNN
F 1 "NET-TIE" H 9650 2317 50  0000 C CNN
F 2 "conn-test:NET-TIE-0.3mm" H 9650 2400 50  0001 C CNN
F 3 "" H 9650 2500 60  0000 C CNN
	1    9650 2500
	1    0    0    -1  
$EndComp
$Comp
L NET-TIE W3
U 1 1 55CB9EAC
P 9650 3600
F 0 "W3" H 9650 3819 50  0000 C CNN
F 1 "NET-TIE" H 9650 3719 50  0000 C CNN
F 2 "conn-test:NET-TIE-0.3mm" H 9650 3500 50  0001 C CNN
F 3 "" H 9650 3600 60  0000 C CNN
	1    9650 3600
	1    0    0    -1  
$EndComp
$Comp
L C-ALUM-1030 C?
U 1 1 55CBF5C4
P 9050 3050
AR Path="/55C7FF09/55CBF5C4" Ref="C?"  Part="1" 
AR Path="/55CBF5C4" Ref="C4"  Part="1" 
F 0 "C4" H 9163 3100 50  0000 L CNN
F 1 "330u" H 9163 3000 50  0000 L CNN
F 2 "IPC7351-Nominal:CAPAE1030X1250" H 9050 3050 50  0001 C CNN
F 3 "" H 9040 3025 60  0000 C CNN
F 4 "DIST DIGIKEY 493-2206-1-ND" H 9050 3050 60  0001 C CNN "BOM"
	1    9050 3050
	1    0    0    -1  
$EndComp
$Comp
L FUSE F1
U 1 1 55CBFAA2
P 9350 2400
F 0 "F1" V 9165 2400 50  0000 C CNN
F 1 "4A" V 9265 2400 50  0000 C CNN
F 2 "IPC7351-Nominal:RESC3216X60" V 9265 2400 60  0001 C CNN
F 3 "" H 9350 2450 60  0000 C CNN
F 4 "DIST DIGIKEY 507-1193-1-ND" H 9350 2400 60  0001 C CNN "BOM"
	1    9350 2400
	0    1    1    0   
$EndComp
$Comp
L SCR Q2
U 1 1 55CC0302
P 7650 3050
F 0 "Q2" H 7730 3100 50  0000 L CNN
F 1 "12A" H 7730 3000 50  0000 L CNN
F 2 "smd-semi:TO-252" H 7650 3050 60  0001 C CNN
F 3 "" H 7650 3050 60  0000 C CNN
F 4 "DIST DIGIKEY 568-12231-1-ND" H 7650 3050 60  0001 C CNN "BOM"
	1    7650 3050
	1    0    0    -1  
$EndComp
$Comp
L SCHOTTKY D1
U 1 1 55CC11B2
P 7950 3050
F 0 "D1" H 7870 3000 50  0000 R CNN
F 1 "40V/5A" H 7870 3100 50  0000 R CNN
F 2 "smd-semi:TO-252" H 7950 3050 60  0001 C CNN
F 3 "" H 7950 3050 60  0000 C CNN
F 4 "DIST DIGIKEY FYD0504SATMCT-ND" H 7950 3050 60  0001 C CNN "BOM"
	1    7950 3050
	-1   0    0    1   
$EndComp
$Sheet
S 3000 3700 1200 1700
U 55CDE77D
F0 "Control" 50
F1 "Control.sch" 50
F2 "VSET" O R 4200 4300 50 
F3 "ISET" O R 4200 4200 50 
F4 "TX" O L 3000 5100 50 
F5 "RX" I L 3000 5200 50 
F6 "VSENSE" I R 4200 4700 50 
F7 "ISENSE" I R 4200 4800 50 
F8 "PREREG_SW" O R 4200 3800 50 
F9 "~ILIM" I R 4200 5000 50 
F10 "~VLIM" I R 4200 4900 50 
F11 "VPREREG" I R 4200 3900 50 
F12 "OVP_THRESH" O R 4200 4100 50 
F13 "~SCR_TRIPPED" I R 4200 5100 50 
F14 "~EN" O R 4200 4000 50 
$EndSheet
$Comp
L R-1206 R3
U 1 1 55CC9AB0
P 6800 3900
F 0 "R3" H 6863 3950 50  0000 L CNN
F 1 "R010" H 6863 3850 50  0000 L CNN
F 2 "IPC7351-Nominal:RESC3216X60" H 6800 3900 50  0001 C CNN
F 3 "" H 6800 3950 60  0000 C CNN
F 4 "DIST DIGIKEY 408-1570-1-ND" H 7550 4450 60  0001 C CNN "BOM"
F 5 "Value" H 6800 3900 60  0001 C CNN "Manuf"
	1    6800 3900
	1    0    0    -1  
$EndComp
$Comp
L CONN-PJ-037A J3
U 1 1 55CCBD03
P 1000 2400
F 0 "J3" H 1152 2394 50  0000 L CNN
F 1 "24V" H 1152 2294 50  0000 L CNN
F 2 "conn-cui:CONN-PJ-037A" H 1000 2400 50  0001 C CNN
F 3 "" H 1100 2400 60  0000 C CNN
F 4 "CONN CUI PJ-037AH" H 1500 3150 60  0001 C CNN "BOM"
	1    1000 2400
	-1   0    0    -1  
$EndComp
$Comp
L C-0805 C1
U 1 1 55CCF849
P 2300 2600
F 0 "C1" H 2413 2650 50  0000 L CNN
F 1 "1u" H 2413 2550 50  0000 L CNN
F 2 "IPC7351-Nominal:CAPC2012X70" H 2300 2600 50  0001 C CNN
F 3 "" H 2290 2575 60  0000 C CNN
F 4 "CAP MLCC 1u ≥X5R 25V 10% [0805]" H 3100 3150 60  0001 C CNN "BOM"
F 5 "Value" H 2300 2600 60  0001 C CNN "Manuf"
	1    2300 2600
	1    0    0    -1  
$EndComp
$Comp
L SCREW MP1
U 1 1 55CD1540
P 750 6650
F 0 "MP1" H 880 6670 50  0000 L CNN
F 1 "SCREW" H 880 6570 50  0000 L CNN
F 2 "mech:SCREW-4-SUPPORTED" H 750 6650 50  0001 C CNN
F 3 "" H 750 6650 60  0000 C CNN
F 4 "~" H 880 6520 50  0000 L CNN "Value2"
	1    750  6650
	1    0    0    -1  
$EndComp
$Comp
L SCREW MP2
U 1 1 55CD1961
P 750 7650
F 0 "MP2" H 880 7720 50  0000 L CNN
F 1 "SCREW" H 880 7620 50  0000 L CNN
F 2 "mech:SCREW-4-SUPPORTED" H 750 7650 50  0001 C CNN
F 3 "" H 750 7650 60  0000 C CNN
F 4 "NYLON" H 880 7520 50  0000 L CNN "Value2"
	1    750  7650
	1    0    0    -1  
$EndComp
$Comp
L V+ #PWR01
U 1 1 55CD3546
P 3400 1200
F 0 "#PWR01" H 3400 1250 30  0001 C CNN
F 1 "V+" H 3400 1313 50  0000 C CNN
F 2 "" H 3400 1200 60  0000 C CNN
F 3 "" H 3400 1200 60  0000 C CNN
	1    3400 1200
	1    0    0    -1  
$EndComp
$Sheet
S 3600 1200 1200 700 
U 55CEE1A2
F0 "LocalPwr" 50
F1 "LocalPwr.sch" 50
F2 "V_IN" I L 3600 1300 50 
F3 "3v6" O R 4800 1300 50 
F4 "3v3" O R 4800 1400 50 
F5 "-230mV" O R 4800 1500 50 
$EndSheet
$Comp
L JUMPER-SOLDER JP2
U 1 1 55CF2A68
P 9350 2100
F 0 "JP2" H 9350 2203 50  0000 C CNN
F 1 "JUMPER-SOLDER" H 9350 2203 50  0001 C CNN
F 2 "conn-test:JUMPER-SOLDER-3mm" H 9350 2100 60  0001 C CNN
F 3 "" H 9350 2100 60  0000 C CNN
	1    9350 2100
	1    0    0    -1  
$EndComp
$Comp
L JUMPER-SOLDER JP1
U 1 1 55CF2CB1
P 9350 1800
F 0 "JP1" H 9350 1903 50  0000 C CNN
F 1 "JUMPER-SOLDER" H 9350 1903 50  0001 C CNN
F 2 "conn-test:JUMPER-SOLDER-3mm" H 9350 1903 60  0001 C CNN
F 3 "" H 9350 1800 60  0000 C CNN
	1    9350 1800
	1    0    0    -1  
$EndComp
$Comp
L FUSE F3
U 1 1 55CF2EB9
P 9650 2100
F 0 "F3" V 9465 2100 50  0000 C CNN
F 1 "4A" V 9565 2100 50  0000 C CNN
F 2 "IPC7351-Nominal:RESC3216X60" V 9565 2100 60  0001 C CNN
F 3 "" H 9650 2150 60  0000 C CNN
F 4 "DIST DIGIKEY 507-1193-1-ND" H 9650 2100 60  0001 C CNN "BOM"
	1    9650 2100
	0    1    1    0   
$EndComp
$Comp
L FUSE F2
U 1 1 55CF2F92
P 9650 1800
F 0 "F2" V 9465 1800 50  0000 C CNN
F 1 "4A" V 9565 1800 50  0000 C CNN
F 2 "IPC7351-Nominal:RESC3216X60" V 9565 1800 60  0001 C CNN
F 3 "" H 9650 1850 60  0000 C CNN
F 4 "DIST DIGIKEY 507-1193-1-ND" H 9650 1800 60  0001 C CNN "BOM"
	1    9650 1800
	0    1    1    0   
$EndComp
$Comp
L AO4805 Q1
U 1 1 55CFEE56
P 1700 2450
F 0 "Q1" V 1997 2450 50  0000 C CNN
F 1 "AO4805" V 1897 2450 50  0000 C CNN
F 2 "IPC7351-Nominal:SOIC127P600X175-8" H 1900 2400 60  0001 C CNN
F 3 "" H 2000 2500 60  0000 C CNN
F 4 "PMOS AOS AO4805" H 2500 3000 60  0001 C CNN "BOM"
	1    1700 2450
	0    1    -1   0   
$EndComp
$Comp
L AO4805 Q1
U 2 1 55CFF672
P 2700 2450
F 0 "Q1" V 2997 2450 50  0000 C CNN
F 1 "AO4805" V 2897 2450 50  0000 C CNN
F 2 "IPC7351-Nominal:SOIC127P600X175-8" H 2900 2400 60  0001 C CNN
F 3 "" H 3000 2500 60  0000 C CNN
F 4 "PMOS AOS AO4805" H 3500 3000 60  0001 C CNN "BOM"
	2    2700 2450
	0    -1   -1   0   
$EndComp
Text Label 1300 2400 0    50   ~ 0
VIN
Text Label 2050 2400 0    50   ~ 0
VRP
$Comp
L REFGND #PWR02
U 1 1 55CBA4D5
P 6600 4550
F 0 "#PWR02" H 6600 4550 30  0001 C CNN
F 1 "REFGND" H 6600 4480 30  0001 C CNN
F 2 "" H 6600 4550 60  0000 C CNN
F 3 "" H 6600 4550 60  0000 C CNN
	1    6600 4550
	1    0    0    -1  
$EndComp
$Comp
L NET-TIE W1
U 1 1 55D017FE
P 6600 4400
F 0 "W1" V 6550 4494 50  0000 L CNN
F 1 "NET-TIE" V 6650 4494 50  0000 L CNN
F 2 "conn-test:NET-TIE-0.3mm" H 6600 4300 50  0001 C CNN
F 3 "" H 6600 4400 60  0000 C CNN
	1    6600 4400
	0    1    1    0   
$EndComp
$Sheet
S 1500 4600 900  1000
U 55D119A9
F0 "Iso" 50
F1 "Iso.sch" 50
F2 "ISO_5V" I L 1500 5500 50 
F3 "ISO_GND" I L 1500 4700 50 
F4 "ISO_IN" I L 1500 4800 50 
F5 "ISO_OUT" O L 1500 5300 50 
F6 "TO_ISO_OUT" I R 2400 5100 50 
F7 "FROM_ISO_IN" O R 2400 5200 50 
$EndSheet
Text Label 9550 2400 0    50   ~ 0
OUT+
Text Label 9750 3700 0    50   ~ 0
OUT-
$Comp
L GND #PWR03
U 1 1 55D38990
P 900 2650
F 0 "#PWR03" H 900 2650 30  0001 C CNN
F 1 "GND" H 900 2580 30  0001 C CNN
F 2 "" H 900 2650 60  0000 C CNN
F 3 "" H 900 2650 60  0000 C CNN
	1    900  2650
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR04
U 1 1 55D38C60
P 1750 3200
F 0 "#PWR04" H 1750 3200 30  0001 C CNN
F 1 "GND" H 1750 3130 30  0001 C CNN
F 2 "" H 1750 3200 60  0000 C CNN
F 3 "" H 1750 3200 60  0000 C CNN
	1    1750 3200
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR05
U 1 1 55D3A58B
P 7000 4150
F 0 "#PWR05" H 7000 4150 30  0001 C CNN
F 1 "GND" H 7000 4080 30  0001 C CNN
F 2 "" H 7000 4150 60  0000 C CNN
F 3 "" H 7000 4150 60  0000 C CNN
	1    7000 4150
	1    0    0    -1  
$EndComp
$Comp
L R-2512 R?
U 1 1 55D4BAB8
P 8400 3050
AR Path="/55C7FF09/55D4BAB8" Ref="R?"  Part="1" 
AR Path="/55D4BAB8" Ref="R5"  Part="1" 
F 0 "R5" H 8337 3000 50  0000 R CNN
F 1 "1k" H 8337 3100 50  0000 R CNN
F 2 "IPC7351-Nominal:RESC6432X70" H 8400 3050 50  0001 C CNN
F 3 "" H 8400 3100 60  0000 C CNN
F 4 "RES SMD 1k [2512]" H 9150 3600 60  0001 C CNN "BOM"
F 5 "Value" H 8400 3050 60  0001 C CNN "Manuf"
	1    8400 3050
	-1   0    0    1   
$EndComp
$Comp
L R-2512 R?
U 1 1 55D4BB6C
P 8650 3050
AR Path="/55C7FF09/55D4BB6C" Ref="R?"  Part="1" 
AR Path="/55D4BB6C" Ref="R6"  Part="1" 
F 0 "R6" H 8587 3000 50  0000 R CNN
F 1 "1k" H 8587 3100 50  0000 R CNN
F 2 "IPC7351-Nominal:RESC6432X70" H 8650 3050 50  0001 C CNN
F 3 "" H 8650 3100 60  0000 C CNN
F 4 "RES SMD 1k [2512]" H 9400 3600 60  0001 C CNN "BOM"
F 5 "Value" H 8650 3050 60  0001 C CNN "Manuf"
	1    8650 3050
	-1   0    0    1   
$EndComp
Text Notes 900  2250 0    50   ~ 0
24V
Text Notes 700  6350 0    50   ~ 0
LOOP INTERFACE\n\nThe mainboard and all modules must be joined in a loop,\noutput of one connected to the input of the next. The sequence\nwill determine the logical ordering of the modules as seen\nby the user interface.
Text Notes 6350 7300 0    150  ~ 30
MOD12003
$Comp
L +3.6 #PWR06
U 1 1 55D42D56
P 5000 1250
F 0 "#PWR06" H 5000 1300 30  0001 C CNN
F 1 "+3.6" H 5000 1363 50  0000 C CNN
F 2 "" H 5000 1250 60  0000 C CNN
F 3 "" H 5000 1250 60  0000 C CNN
	1    5000 1250
	1    0    0    -1  
$EndComp
$Comp
L +3.3 #PWR07
U 1 1 55D437B7
P 5200 1250
F 0 "#PWR07" H 5200 1300 30  0001 C CNN
F 1 "+3.3" H 5200 1363 50  0000 C CNN
F 2 "" H 5200 1250 60  0000 C CNN
F 3 "" H 5200 1250 60  0000 C CNN
	1    5200 1250
	1    0    0    -1  
$EndComp
$Comp
L V- #PWR08
U 1 1 55D43A48
P 5000 1600
F 0 "#PWR08" H 5000 1550 30  0001 C CNN
F 1 "V-" H 5000 1487 50  0000 C CNN
F 2 "" H 5000 1600 60  0000 C CNN
F 3 "" H 5000 1600 60  0000 C CNN
	1    5000 1600
	1    0    0    -1  
$EndComp
$Comp
L CONN-100MIL-M-1x4-SHROUD J1
U 1 1 55D54DF0
P 950 4700
F 0 "J1" H 1069 4597 50  0000 L CNN
F 1 "IN" H 1069 4497 50  0000 L CNN
F 2 "conn-100mil:CONN-100MIL-M-1x4-SHROUD" H 950 4700 50  0001 C CNN
F 3 "" H 950 4700 50  0000 C CNN
F 4 "DIST DIGIKEY A19431-ND" H 950 4700 60  0001 C CNN "BOM"
	1    950  4700
	-1   0    0    -1  
$EndComp
$Comp
L CONN-100MIL-M-1x4-SHROUD J2
U 1 1 55D555A0
P 950 5200
F 0 "J2" H 1069 5097 50  0000 L CNN
F 1 "OUT" H 1069 4997 50  0000 L CNN
F 2 "conn-100mil:CONN-100MIL-M-1x4-SHROUD" H 950 5200 50  0001 C CNN
F 3 "" H 950 5200 50  0000 C CNN
F 4 "DIST DIGIKEY A19431-ND" H 950 5200 60  0001 C CNN "BOM"
	1    950  5200
	-1   0    0    -1  
$EndComp
$Comp
L CONN-100MIL-M-1x3 J8
U 1 1 55DA18EF
P 950 4100
F 0 "J8" H 1068 4047 50  0000 L CNN
F 1 "DEVEL" H 1068 3947 50  0000 L CNN
F 2 "conn-100mil:CONN-100MIL-M-1x3" H 950 4100 50  0001 C CNN
F 3 "" H 950 4100 50  0000 C CNN
F 4 "(3) PIN STRIP 100mil" H 950 4100 60  0001 C CNN "BOM"
	1    950  4100
	-1   0    0    -1  
$EndComp
$Comp
L GND #PWR09
U 1 1 55DA2653
P 1050 4100
F 0 "#PWR09" H 1050 4100 30  0001 C CNN
F 1 "GND" H 1050 4030 30  0001 C CNN
F 2 "" H 1050 4100 60  0000 C CNN
F 3 "" H 1050 4100 60  0000 C CNN
	1    1050 4100
	0    -1   -1   0   
$EndComp
$Comp
L JUMPER JP3
U 1 1 55DA31AE
P 2700 5200
F 0 "JP3" H 2700 5350 50  0000 C CNN
F 1 "JUMPER" H 2700 5442 50  0001 C CNN
F 2 "conn-100mil:CONN-100MIL-M-1x2" H 2700 5200 60  0001 C CNN
F 3 "" H 2700 5200 60  0000 C CNN
F 4 "(2) PIN STRIP 100mil; JUMPER 100mil" H 2700 5200 60  0001 C CNN "BOM"
	1    2700 5200
	1    0    0    1   
$EndComp
Text Notes 700  6500 0    50   ~ 0
Mating connector: A30980-ND
Text Notes 7950 4050 0    50   ~ 10
MOUNTING/HEAT SINKING:
Text Notes 9100 4700 1    50   ~ 0
@R6
Text Notes 9225 4700 1    50   ~ 0
@R5
Text Notes 8780 5290 1    50   ~ 0
@Q10
Text Notes 8000 4250 0    30   ~ 0
SCREW
Text Notes 9550 4250 0    30   ~ 0
SCREW
Text Notes 9350 5630 0    30   ~ 0
METAL\nSCREW
Text Notes 8900 4925 0    30   ~ 0
THERMAL ELASTOMER
Text Notes 8450 5475 0    30   ~ 0
THERMAL ELASTOMER
Text Notes 7950 6950 0    50   ~ 0
Thermal elastomer pads can be cut from one of:\nt-Global H48-6-21-15-13-1A  (1168-1700-ND)\nt-Global H48-6-25-25-9  (1168-1703-ND)\nor similar compressible materials.\n\nPlace an insulating plastic sheet between the PCB\nand the heat sink, with minimal cutouts for\npassthrough.\n\nTop two screws must be non-conductive.
Wire Wire Line
	10000 2400 9950 2500
Wire Wire Line
	9950 2500 9750 2500
Connection ~ 10000 2400
Wire Wire Line
	9750 3600 9950 3600
Wire Wire Line
	9950 3600 10000 3700
Connection ~ 10000 3700
Wire Wire Line
	6800 3800 6800 3750
Wire Wire Line
	6800 3750 6850 3700
Wire Wire Line
	6850 3700 10100 3700
Wire Wire Line
	6800 3750 6750 3700
Wire Wire Line
	6750 3700 6400 3700
Connection ~ 6800 3750
Connection ~ 6800 4050
Wire Wire Line
	4800 2400 5400 2400
Wire Wire Line
	9050 3150 9050 3700
Connection ~ 9050 3700
Wire Wire Line
	9050 2400 9050 2950
Connection ~ 9050 2400
Wire Wire Line
	9450 2400 10100 2400
Wire Wire Line
	6400 2400 9250 2400
Wire Wire Line
	7650 2950 7650 2400
Connection ~ 7950 2400
Wire Wire Line
	7650 3150 7650 3700
Connection ~ 7950 3700
Connection ~ 7650 3700
Connection ~ 7650 2400
Wire Wire Line
	7950 3700 7950 3150
Wire Wire Line
	7950 2950 7950 2400
Wire Wire Line
	6400 3150 7600 3150
Wire Wire Line
	7300 3300 7300 3150
Connection ~ 7300 3150
Wire Wire Line
	7300 3500 7300 3700
Connection ~ 7300 3700
Wire Wire Line
	6950 3150 6950 3300
Connection ~ 6950 3150
Wire Wire Line
	6950 3500 6950 3700
Connection ~ 6950 3700
Wire Wire Line
	1200 2400 1600 2400
Wire Wire Line
	900  2600 900  2650
Wire Wire Line
	2800 2400 3600 2400
Wire Wire Line
	1800 2400 2600 2400
Wire Wire Line
	1750 2800 2850 2800
Wire Wire Line
	2650 2800 2650 2550
Wire Wire Line
	1950 2500 1950 2400
Connection ~ 1950 2400
Wire Wire Line
	1950 2700 1950 2800
Connection ~ 1950 2800
Connection ~ 1750 2800
Connection ~ 2650 2800
Wire Wire Line
	3050 2800 3250 2800
Wire Wire Line
	3250 2800 3250 2400
Connection ~ 3250 2400
Wire Wire Line
	2300 2500 2300 2400
Connection ~ 2300 2400
Wire Wire Line
	2300 2700 2300 2800
Connection ~ 2300 2800
Wire Wire Line
	3400 1200 3400 2400
Connection ~ 3400 2400
Wire Wire Line
	4200 4200 5400 4200
Wire Wire Line
	5400 4300 4200 4300
Wire Wire Line
	9250 1800 9150 1800
Wire Wire Line
	9150 1800 9150 2400
Connection ~ 9150 2400
Wire Wire Line
	9150 2100 9250 2100
Connection ~ 9150 2100
Wire Wire Line
	9450 2100 9550 2100
Wire Wire Line
	9550 1800 9450 1800
Wire Wire Line
	9750 1800 9850 1800
Wire Wire Line
	9850 1800 9850 2400
Connection ~ 9850 2400
Wire Wire Line
	9750 2100 9850 2100
Connection ~ 9850 2100
Wire Wire Line
	1600 2350 1550 2350
Wire Wire Line
	1550 2350 1550 2400
Connection ~ 1550 2400
Wire Wire Line
	1750 2550 1750 2900
Wire Wire Line
	2800 2350 2850 2350
Wire Wire Line
	2850 2350 2850 2400
Connection ~ 2850 2400
Wire Wire Line
	6800 4050 6600 4100
Wire Wire Line
	6600 4100 6600 4300
Wire Wire Line
	6800 4050 7000 4100
Wire Wire Line
	7000 4100 7000 4150
Wire Wire Line
	3600 3100 3300 3100
Wire Wire Line
	3300 3100 3300 3500
Wire Wire Line
	3300 3500 4400 3500
Wire Wire Line
	4400 3500 4400 3800
Wire Wire Line
	4400 3800 4200 3800
Wire Wire Line
	4200 3900 5000 3900
Wire Wire Line
	5000 3900 5000 2400
Connection ~ 5000 2400
Wire Wire Line
	4200 4100 5400 4100
Wire Wire Line
	1750 3200 1750 3100
Wire Wire Line
	6800 4000 6800 4050
Wire Wire Line
	4200 4000 5400 4000
Wire Wire Line
	8400 2950 8400 2400
Connection ~ 8400 2400
Wire Wire Line
	8400 3150 8400 3700
Connection ~ 8400 3700
Wire Wire Line
	6400 3600 9550 3600
Wire Wire Line
	6400 2500 9550 2500
Wire Wire Line
	8650 2400 8650 2950
Connection ~ 8650 2400
Wire Wire Line
	8650 3150 8650 3700
Connection ~ 8650 3700
Wire Wire Line
	4200 4700 5400 4700
Wire Wire Line
	4200 4800 5400 4800
Wire Wire Line
	4200 4900 5400 4900
Wire Wire Line
	4200 5000 5400 5000
Wire Wire Line
	4200 5100 5400 5100
Wire Wire Line
	750  6800 750  7000
Wire Wire Line
	3600 1300 3400 1300
Connection ~ 3400 1300
Wire Wire Line
	5000 1250 5000 1300
Wire Wire Line
	5000 1300 4800 1300
Wire Wire Line
	5200 1250 5200 1400
Wire Wire Line
	5200 1400 4800 1400
Wire Wire Line
	5000 1600 5000 1500
Wire Wire Line
	5000 1500 4800 1500
Wire Wire Line
	1000 4700 1500 4700
Wire Wire Line
	1000 5500 1500 5500
Wire Wire Line
	1000 5000 1100 5000
Wire Wire Line
	1100 5000 1100 5500
Connection ~ 1100 5500
Wire Wire Line
	1000 5200 1200 5200
Wire Wire Line
	1200 5200 1200 4700
Connection ~ 1200 4700
Wire Wire Line
	1000 5300 1500 5300
Wire Wire Line
	1000 4800 1500 4800
Wire Wire Line
	1000 4200 2850 4200
Wire Wire Line
	2850 4200 2850 5200
Connection ~ 2850 5200
Wire Wire Line
	1000 4300 2750 4300
Wire Wire Line
	2750 4300 2750 5100
Connection ~ 2750 5100
Wire Wire Line
	1050 4100 1000 4100
Wire Wire Line
	2400 5100 3000 5100
Wire Wire Line
	2800 5200 3000 5200
Wire Wire Line
	2600 5200 2400 5200
Wire Wire Line
	1000 4900 1050 4900
Wire Wire Line
	1050 4900 1050 4700
Connection ~ 1050 4700
Wire Wire Line
	1000 5400 1050 5400
Wire Wire Line
	1050 5400 1050 5200
Connection ~ 1050 5200
Wire Notes Line
	7500 2850 7500 3250
Wire Notes Line
	7500 3250 8850 3250
Wire Notes Line
	8850 3250 8850 2850
Wire Notes Line
	8850 2850 7500 2850
Wire Bus Line
	7950 4100 9750 4100
Wire Bus Line
	9750 4100 9750 5900
Wire Bus Line
	9750 5900 7950 5900
Wire Bus Line
	7950 5900 7950 4100
Wire Wire Line
	8900 4400 9350 4400
Wire Wire Line
	8900 4400 8900 4850
Wire Wire Line
	8900 4850 9350 4850
Wire Wire Line
	9350 4850 9350 4400
Wire Notes Line
	9000 4500 9250 4500
Wire Notes Line
	9125 4500 9125 4750
Wire Notes Line
	9000 4750 9250 4750
Wire Notes Line
	9000 4750 9000 4500
Wire Notes Line
	9250 4750 9250 4500
Wire Wire Line
	8450 4950 8450 5400
Wire Wire Line
	8450 5400 8900 5400
Wire Wire Line
	8900 5400 8900 4950
Wire Wire Line
	8900 4950 8450 4950
Wire Notes Line
	8000 4150 8150 4150
Wire Notes Line
	8150 4150 8150 4300
Wire Notes Line
	8150 4300 8000 4300
Wire Notes Line
	8000 4300 8000 4150
Wire Notes Line
	9700 4150 9550 4150
Wire Notes Line
	9550 4150 9550 4300
Wire Notes Line
	9550 4300 9700 4300
Wire Notes Line
	9700 4300 9700 4150
Wire Notes Line
	9350 5500 9500 5500
Wire Notes Line
	9500 5500 9500 5650
Wire Notes Line
	9500 5650 9350 5650
Wire Notes Line
	9350 5650 9350 5500
Wire Notes Line
	8950 4350 8950 4300
Wire Notes Line
	8950 4300 9150 4300
Wire Notes Line
	9150 4300 9150 4350
Wire Notes Line
	9150 4350 8950 4350
Wire Notes Line
	8950 4325 8750 4325
Wire Notes Line
	8750 4325 8750 5000
Wire Bus Line
	9800 4150 9850 4100
Wire Bus Line
	9850 4100 9900 4150
Wire Bus Line
	9800 4100 9900 4100
Wire Bus Line
	9800 5900 9900 5900
Wire Bus Line
	9850 5900 9800 5850
Wire Bus Line
	9850 5900 9900 5850
Wire Bus Line
	9850 4100 9850 5900
Wire Bus Line
	8000 6050 7950 6000
Wire Bus Line
	7950 6000 8000 5950
Wire Bus Line
	7950 6050 7950 5950
Wire Bus Line
	9750 6050 9750 5950
Wire Bus Line
	9750 6000 9700 6050
Wire Bus Line
	9750 6000 9700 5950
Wire Bus Line
	7950 6000 9750 6000
Text Notes 9950 5100 1    50   ~ 0
90mm
Text Notes 8800 6100 0    50   ~ 0
90mm
Text Notes 8000 5750 0    40   ~ 0
@Q10 and @U6 mount to heat sink,\nnot PCB, through a thin insulator.\neg. Laird A15037-003  (926-1489-ND)
Text Notes 8610 5320 1    50   ~ 0
@U6
Wire Bus Line
	8500 5350 8850 5350
Wire Bus Line
	8500 5100 8500 5350
Wire Bus Line
	8500 5100 8650 5100
Wire Bus Line
	8650 5350 8650 5000
Wire Bus Line
	8650 5000 8850 5000
Wire Bus Line
	8850 5000 8850 5350
$Comp
L SCREW MP3
U 1 1 55DFD130
P 1250 7650
F 0 "MP3" H 1380 7720 50  0000 L CNN
F 1 "SCREW" H 1380 7620 50  0000 L CNN
F 2 "mech:SCREW-4-SUPPORTED" H 1250 7650 50  0001 C CNN
F 3 "" H 1250 7650 60  0000 C CNN
F 4 "NYLON" H 1380 7520 50  0000 L CNN "Value2"
	1    1250 7650
	1    0    0    -1  
$EndComp
Wire Wire Line
	750  7800 750  7950
Wire Wire Line
	750  7950 1150 7950
Text Label 1150 7950 2    50   ~ 0
PAD_MP2
Wire Wire Line
	1250 7800 1250 7950
Wire Wire Line
	1250 7950 1650 7950
Text Label 1650 7950 2    50   ~ 0
PAD_MP3
$Comp
L C-1206 C58
U 1 1 55DFEA7B
P 750 7100
F 0 "C58" H 863 7150 50  0000 L CNN
F 1 "1n/1kV" H 863 7050 50  0000 L CNN
F 2 "IPC7351-Nominal:CAPC3216X70" H 750 7100 50  0001 C CNN
F 3 "" H 740 7075 60  0000 C CNN
F 4 "CAP MLCC 1n 1kV 20% [1206]" H 1550 7650 60  0001 C CNN "BOM"
F 5 "Value" H 750 7100 60  0001 C CNN "Manuf"
	1    750  7100
	1    0    0    -1  
$EndComp
Wire Wire Line
	750  6850 1200 6850
Wire Wire Line
	1200 6850 1200 6900
Connection ~ 750  6850
Wire Wire Line
	1200 7150 1200 7100
Wire Wire Line
	1200 7350 1200 7400
Wire Wire Line
	1200 7400 750  7400
Wire Wire Line
	750  7200 750  7450
$Comp
L GND #PWR010
U 1 1 55E00FFA
P 750 7450
F 0 "#PWR010" H 750 7450 30  0001 C CNN
F 1 "GND" H 750 7380 30  0001 C CNN
F 2 "" H 750 7450 60  0000 C CNN
F 3 "" H 750 7450 60  0000 C CNN
	1    750  7450
	1    0    0    -1  
$EndComp
Connection ~ 750  7400
Text Label 1200 6850 2    50   ~ 0
PAD_MP1
Wire Wire Line
	6600 4500 6600 4550
$Comp
L R-0402 R?
U 1 1 55FDF19A
P 7300 3400
AR Path="/55CDE77D/55FDF19A" Ref="R?"  Part="1" 
AR Path="/55C7FF09/55FDF19A" Ref="R?"  Part="1" 
AR Path="/55FDF19A" Ref="R4"  Part="1" 
F 0 "R4" H 7237 3350 50  0000 R CNN
F 1 "348" H 7237 3450 50  0000 R CNN
F 2 "IPC7351-Nominal:RESC1005X40" H 7300 3400 50  0001 C CNN
F 3 "" H 7300 3450 60  0000 C CNN
F 4 "RES SMD 348 1% [0402]" H 8050 3950 60  0001 C CNN "BOM"
	1    7300 3400
	-1   0    0    1   
$EndComp
$Comp
L C-0402 C?
U 1 1 55FE111A
P 6950 3400
AR Path="/55CDE77D/55FE111A" Ref="C?"  Part="1" 
AR Path="/55C7FF09/55FE111A" Ref="C?"  Part="1" 
AR Path="/55FE111A" Ref="C3"  Part="1" 
F 0 "C3" H 7063 3450 50  0000 L CNN
F 1 "100n" H 7063 3350 50  0000 L CNN
F 2 "IPC7351-Nominal:CAPC1005X55" H 6950 3400 50  0001 C CNN
F 3 "" H 6940 3375 60  0000 C CNN
F 4 "CAP MLCC 100n ≥X5R 10V 20% [0402]" H 7750 3950 60  0001 C CNN "BOM"
	1    6950 3400
	1    0    0    -1  
$EndComp
$Comp
L R-0402 R?
U 1 1 55FEF887
P 1200 7250
AR Path="/55C7FF09/55FEF887" Ref="R?"  Part="1" 
AR Path="/55FEF887" Ref="R53"  Part="1" 
F 0 "R53" H 1137 7200 50  0000 R CNN
F 1 "470k" H 1137 7300 50  0000 R CNN
F 2 "IPC7351-Nominal:RESC1005X40" H 1200 7250 50  0001 C CNN
F 3 "" H 1200 7300 60  0000 C CNN
F 4 "RES SMD 470k 1% [0402]" H 1950 7800 60  0001 C CNN "BOM"
	1    1200 7250
	-1   0    0    1   
$EndComp
$Comp
L R-0402 R?
U 1 1 55FF06D0
P 1200 7000
AR Path="/55C7FF09/55FF06D0" Ref="R?"  Part="1" 
AR Path="/55FF06D0" Ref="R52"  Part="1" 
F 0 "R52" H 1137 6950 50  0000 R CNN
F 1 "470k" H 1137 7050 50  0000 R CNN
F 2 "IPC7351-Nominal:RESC1005X40" H 1200 7000 50  0001 C CNN
F 3 "" H 1200 7050 60  0000 C CNN
F 4 "RES SMD 470k 1% [0402]" H 1950 7550 60  0001 C CNN "BOM"
	1    1200 7000
	-1   0    0    1   
$EndComp
$Comp
L R-0402 R?
U 1 1 56006D2F
P 1750 3000
AR Path="/55C7FF09/56006D2F" Ref="R?"  Part="1" 
AR Path="/56006D2F" Ref="R1"  Part="1" 
F 0 "R1" H 1687 2950 50  0000 R CNN
F 1 "470k" H 1687 3050 50  0000 R CNN
F 2 "IPC7351-Nominal:RESC1005X40" H 1750 3000 50  0001 C CNN
F 3 "" H 1750 3050 60  0000 C CNN
F 4 "RES SMD 470k 1% [0402]" H 2500 3550 60  0001 C CNN "BOM"
	1    1750 3000
	-1   0    0    1   
$EndComp
$Comp
L R-0402 R?
U 1 1 56007AD8
P 1950 2600
AR Path="/55C7FF09/56007AD8" Ref="R?"  Part="1" 
AR Path="/56007AD8" Ref="R2"  Part="1" 
F 0 "R2" H 1887 2550 50  0000 R CNN
F 1 "470k" H 1887 2650 50  0000 R CNN
F 2 "IPC7351-Nominal:RESC1005X40" H 1950 2600 50  0001 C CNN
F 3 "" H 1950 2650 60  0000 C CNN
F 4 "RES SMD 470k 1% [0402]" H 2700 3150 60  0001 C CNN "BOM"
	1    1950 2600
	-1   0    0    1   
$EndComp
$Comp
L C-0402 C?
U 1 1 5600B2D2
P 2950 2800
AR Path="/55C7FF09/5600B2D2" Ref="C?"  Part="1" 
AR Path="/5600B2D2" Ref="C2"  Part="1" 
F 0 "C2" V 2712 2800 50  0000 C CNN
F 1 "22n" V 2812 2800 50  0000 C CNN
F 2 "IPC7351-Nominal:CAPC1005X55" H 2950 2800 50  0001 C CNN
F 3 "" H 2940 2775 60  0000 C CNN
F 4 "CAP MLCC 22n ≥X5R 50V 10% [0402]" H 3750 3350 60  0001 C CNN "BOM"
	1    2950 2800
	0    1    1    0   
$EndComp
Text Notes 3150 6750 0    50   ~ 0
Additional parts:\nJ1, J2 mating connector: A30980-ND\nElastomer: 1168-1700-ND\nHeatsink insulator: 926-1489-ND
$EndSCHEMATC
