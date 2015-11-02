AVRGCC ?= /usr/bin/avr-gcc
AVRGXX ?= /usr/bin/avr-g++
AVRASM ?= /usr/bin/avr-as
AVROBJCOPY ?= /usr/bin/avr-objcopy
AVRSIZE ?= /usr/bin/avr-size
AVRDUDE ?= /usr/bin/avrdude

CC := ${AVRGCC}

# AVRDUDEPROGRAMMER ?= avr109
# AVRDUDEMCU=x32e5
# AVRDUDEDEVICE=/dev/ttyUSB0 -b 115200 -e
# MCU=atxmega32e5
# F_CPU=32000000

AVRDUDEPROGRAMMER=avrispmkII
AVRDUDEMCU=x16e5
AVRDUDEDEVICE=usb
MCU=atxmega16e5
F_CPU=32000000

INCROOT=afw/include
SRCROOT=afw/src

OPTFLAGS ?= -flto -Os -mcall-prologues
CFLAGS 	 ?= -mmcu=${MCU} -DF_CPU=${F_CPU}uLL -I${INCROOT} -iquote. -std=gnu11 ${OPTFLAGS} -g -Wall -Wextra
LDFLAGS  ?= -mmcu=${MCU} -std=gnu11 ${OPTFLAGS} -g -Wall -Wextra

HFILES = $(shell find ${INCROOT} -type f -name '*.h') $(wildcard *.h)
CFILES = $(shell find ${SRCROOT} -type f -name '*.c') $(wildcard *.c)
C_OFILES = $(patsubst %.c,%.o,$(CFILES))

.PHONY: all clean program doc

all: fw.elf
	${AVRSIZE} -C --mcu=${MCU} fw.elf

clean:
	rm -f ${C_OFILES}
	rm -f fw.elf

fw.elf: ${C_OFILES} ${S_OFILES} ${HFILES}
	${AVRGCC} -mmcu=${MCU} ${LDFLAGS} ${C_OFILES} ${S_OFILES} -o $@

program: fw.elf
	${AVRDUDE} -v -v -p ${AVRDUDEMCU} -c ${AVRDUDEPROGRAMMER} -P ${AVRDUDEDEVICE} \
		-U flash:w:$<

doc: Doxyfile
	doxygen Doxyfile
