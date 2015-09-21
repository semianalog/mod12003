AVRGCC ?= /usr/bin/avr-gcc
AVRGPP ?= /usr/bin/avr-g++
AVRASM ?= /usr/bin/avr-as
AVROBJCOPY ?= /usr/bin/avr-objcopy
AVRSIZE ?= /usr/bin/avr-size
AVRDUDE ?= /usr/bin/avrdude

CC := ${AVRGCC}

AVRDUDEPROGRAMMER ?= avr109
AVRDUDEMCU=x32e5
AVRDUDEDEVICE=/dev/ttyUSB0 -b 115200 -e
MCU=atxmega32e5

# FLOAT_PRINTF ?= -Wl,-u,vfprintf -lprintf_flt
OPTFLAGS ?= -O1 -g
LANGFLAGS ?= -std=c99
CFLAGS ?= -mmcu=${MCU} ${OPTFLAGS} ${LANGFLAGS} -Wall -Wextra -Werror -flto -DF_CPU=32000000uLL -Iinclude
ASFLAGS ?= -mmcu=${MCU}
LDFLAGS ?= ${FLOAT_PRINTF} -lm -mmcu=${MCU} ${OPTFLAGS} ${LANGFLAGS} -flto -Iinclude

HFILES = $(shell find -type f -name '*.h')
CFILES = $(shell find -type f -name '*.c')
SFILES = $(shell find -type f -name '*.S')
C_OFILES = $(patsubst %.c,%.o,$(CFILES))
S_OFILES = $(patsubst %.s,%.o,$(SFILES))

.PHONY: all clean program

all: fw.elf
	${AVRSIZE} -C --mcu=${MCU} fw.elf

clean:
	rm -f *.o
	rm -f fw.elf

fw.elf: ${C_OFILES} ${S_OFILES} ${HFILES}
	${AVRGCC} -mmcu=${MCU} ${LDFLAGS} ${C_OFILES} ${S_OFILES} -o $@

program: fw.elf
	${AVRDUDE} -v -v -p ${AVRDUDEMCU} -c ${AVRDUDEPROGRAMMER} -P ${AVRDUDEDEVICE} \
		-U flash:w:$<
