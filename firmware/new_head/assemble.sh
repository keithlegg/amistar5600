#!/bin/bash

#fuses - EXTERNAL NO DIV8 
#sudo avrdude -V -c usbtiny -p atmega324p -U lfuse:w:0xce:m -U hfuse:w:0x99:m -U efuse:w:0xff:m

#######

#324 WORKS!!
# avr-gcc -Wall -Os -DF_CPU=20000000UL -mmcu=atmega324p -o vga.o vga.S
# avr-objcopy -j .text -j .data -O ihex vga.o vga.hex
# avrdude -V -c usbtiny -p atmega324p -b 19200 -U flash:w:vga.hex


#164 WORKS!!!
# avr-gcc -Wall -Os -DF_CPU=20000000UL -mmcu=atmega164p -o vga.o vga.S
# avr-objcopy -j .text -j .data -O ihex vga.o vga.hex
# avrdude -V -c usbtiny -p atmega164p -b 19200 -U flash:w:vga.hex

#######

#THIS DID NOT WORK!
# avr-g++ -DF_CPU=20000000UL -mmcu=atmega324p -c vga.S -o vga.o
# avr-objcopy -O ihex -R .eeprom vga.o vga.hex
# avrdude -V -c usbtiny -p atmega324p -b 19200 -U flash:w:vga.hex


# if [ "$1" == "cat" ]; then
#     echo "Positional parameter 1 contains cat"
# else
#     echo "Positional parameter 1 is empty"
# fi


#ASSEMBLE AND UPLOAD
avr-gcc -Wall -Os -DF_CPU=20000000UL -mmcu=atmega2560 -o vga.o ./asm/CY7C109BN.S 2>&1

# avr-objcopy -j .text -j .data -O ihex vga.o vga.hex
# avrdude -V -c usbtiny -p atmega324p -b 19200 -U flash:w:vga.hex
# #CLEAN UP NOW
# rm -f *.o
# rm -f *.hex

