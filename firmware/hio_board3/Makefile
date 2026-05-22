



SOURCES = main.c src/ST7735.c

 




###################################
PROJECT=lcd2560
CC=avr-gcc
OBJCOPY=avr-objcopy
MMCU=atmega2560


PROG_MMCU=atmega2560

PROGRAMMER=usbtiny
#PROGRAMMER=stk500

#PORT=usb:bus:device #/dev/cuaU0 #usb:bus:device


#defaults for atmega2560
LOW_FUSE=0x42
HIGH_FUSE=0x99
EXT_FUSE=0xff

###############################

#-std=gnu99

# OPTIONS -  https://gcc.gnu.org/onlinedocs/gcc-3.0/gcc_32.html

#-W              = Print extra warning messages for these events
#-Os             = Optimize for size
#-funsigned-char = Let the type char be unsigned 
#-M              = Instead of outputting the result of preprocessing, output a rule for make describing the dependencies
#-MP             = This option instructs CPP to add a phony target for each dependency other than the main file
#-MT target      = By default CPP uses the main file name, this will allow you to specify a target  overriding the default one. 
#-Idir           = Add the directory dir to the head of the list of directories to be searched for header files.
#-I-             = any `-I' options before the `-I-'option are searched only for the case of #include "file" not #include <file> 

#-Wall -Os  -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -MD -MP -MT 
CFLAGS=-mmcu=$(MMCU) -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums 

###############################


### avr-objcopy -j .text -j .data
###
###  your problem is that your hex file doesn't contain
### the string literal's initialization values (they are located in
### section ".data", and are copied over into SRAM by the startup code).
### 
### Btw., conventionally, the suffix .o is used for *relocatable* object
### files, while the result from the linker either has no suffix at all
### (Unix convention), or one of the suffices .out (generic usage) or .elf
### (this seems to be the de-facto standard for the AVR-GCC community).


$(PROJECT).hex: $(PROJECT).out
	@echo "Creating hex file..."
	#$(OBJCOPY) -j .text -O ihex $(PROJECT).out $(PROJECT).hex
	$(OBJCOPY) -j .text -j .data -O ihex $(PROJECT).out $(PROJECT).hex

	rm -f ./*.d
	rm -f ./*.out
	@echo


$(PROJECT).out: $(SOURCES)
	@echo
	@echo "Compiling..."
	$(CC) $(CFLAGS) -I./ -o $(PROJECT).out $(SOURCES) 
	avr-size $(PROJECT).out
	@echo


program_fuses:
	@echo
	@echo "Writing fuses..."
	avrdude -P $(PORT) -v -p $(PROG_MMCU) -c $(PROGRAMMER) -e \
	-U lfuse:w:$(LOW_FUSE):m -U hfuse:w:$(HIGH_FUSE):m -U efuse:w:$(EXT_FUSE):m
	@echo


program: $(PROJECT).hex
	@echo
	@echo "Downloading..."
	avrdude -v -p $(PROG_MMCU) -c $(PROGRAMMER) -e \
	-U flash:w:$(PROJECT).hex
	@echo


clean:
	@echo
	@echo "Cleaning..."
	rm -f ./*.out
	rm -f ./*.hex
	rm -f ./*.d
	rm -f ./*.map
	rm -f ./*.o
	@echo

