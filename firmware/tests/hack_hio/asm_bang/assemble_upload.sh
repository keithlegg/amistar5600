#!/bin/bash

#ASSEMBLE AND UPLOAD
avr-gcc -Wall -Os -DF_CPU=16000000UL -mmcu=atmega328p  -o bitbangr.o ./asm/bitbang.S 2>&1
avr-objcopy -j .text -j .data -O ihex bitbangr.o bitbangr.hex
avrdude -V -c usbtiny -p atmega328p -b 19200 -U flash:w:bitbangr.hex

#CLEAN UP NOW
rm -f *.o
rm -f *.hex

