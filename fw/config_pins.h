//PORT  PIN NAME            DIR     DEFVAL  CONFIG

X(A,    0,  P_VREF,         D_IN,   0,      0)
X(A,    1,  P_REFGND,       D_IN,   0,      0)
X(A,    2,  P_ISET,         D_OUT,  0,      0)
X(A,    3,  P_ISENSE,       D_IN,   0,      0)
X(A,    4,  P_VSENSE,       D_IN,   0,      0)
X(A,    5,  PA5,            D_IN,   0,      0)
X(A,    6,  PA6,            D_IN,   0,      0)
X(A,    7,  P_TEMP,         D_IN,   0,      0)

X(C,    0,  PC0,            D_IN,   0,      0)
X(C,    1,  P_OVPTH_PWM,    D_OUT,  1,      0)
X(C,    2,  P_PREREG_SW,    D_OUT,  1,      0)
X(C,    3,  P_SCR_TRIP,     D_IN,   0,      PORT_INVEN_bm)
X(C,    4,  P_ILIM,         D_IN,   0,      PORT_INVEN_bm)
X(C,    5,  P_VLIM,         D_IN,   0,      PORT_INVEN_bm)
X(C,    6,  P_LINREG_EN,    D_OUT,  0,      PORT_INVEN_bm)
X(C,    7,  PC7_GND,        D_IN,   0,      0)

X(D,    0,  P_SDA,          D_OUT,  1,      PORT_OPC_WIREDANDPULL_gc)
X(D,    1,  P_SCL,          D_OUT,  1,      PORT_OPC_WIREDANDPULL_gc)
X(D,    2,  P_LEDCC,        D_OUT,  1,      0)
X(D,    3,  P_LEDCV,        D_OUT,  1,      0)
X(D,    4,  P_VPREREG,      D_IN,   0,      0)
X(D,    5,  P_BTN2,         D_IN,   0,      PORT_INVEN_bm | PORT_OPC_PULLUP_gc)
X(D,    6,  P_RX,           D_IN,   1,      0)
X(D,    7,  P_TX,           D_OUT,  1,      0)
XREMAP(D, USART0)

X(R,    0,  P_BTN1,         D_IN,   0,      PORT_INVEN_bm | PORT_OPC_PULLUP_gc)
X(R,    1,  PR1,            D_IN,   0,      0)
