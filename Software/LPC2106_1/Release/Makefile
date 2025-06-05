###############################################################################
# Makefile for project 
###############################################################################

## General options
PROJECT = 
MCU = LPC2106
COMPILER = "GCC for ARM"
TARGET = Release
SHELL = C:\WINDOWS\system32\cmd.exe

## Tools general options
ASMFLAGS=-x assembler-with-cpp -gdwarf-2 -mcpu=arm7tdmi -mthumb-interwork
BIN2HEXFLAGS=-O ihex
CCFLAGS=-Os -mcpu=arm7tdmi -mthumb-interwork -I.. -gdwarf-2 -fomit-frame-pointer -Wall -fverbose-asm -MD -MP -fsigned-char -I"C:\Users\Luis Fran\DPSE\Simulacion\SW_DPSE_22_23_Simulacion\LPC2106_1"
LDFLAGS=-mcpu=arm7tdmi -nostartfiles -mthumb-interwork

## Processing Tools
CC=arm-none-eabi-gcc.exe
LD=arm-none-eabi-gcc.exe
ASM=arm-none-eabi-gcc.exe
BIN2HEX=arm-none-eabi-objcopy.exe
#  C:/Users/Alonso/pulear/DPSE/Software/LPC2106_1/Release/Release.hex: TOOL=BIN2HEX
#     C:/Users/Alonso/pulear/DPSE/Software/LPC2106_1/Release/XPMPAC~1.elf: TOOL=LD
#        C:/Users/Alonso/pulear/DPSE/Software/LPC2106_1/Release/XPMPAC~1.o: TOOL=CC
#           C:/Users/Alonso/pulear/DPSE/Software/LPC2106_1/xpmPacman.c

# Build tree:
all:	Release

Release:	Release.hex
	cp "Release.hex" "C:/Users/Alonso/pulear/DPSE/Software/LPC2106_1.hex"

Release.hex:	XPMPAC~1.elf 
	$(BIN2HEX) $(BIN2HEXFLAGS)  "XPMPAC~1.elf" "Release.hex"

XPMPAC~1.elf:	XPMPAC~1.o 
	$(LD) $(LDFLAGS)  -Wl,-Map,XPMPAC~1.map,--cref,--no-warn-mismatch,--start-group,--end-group,--gc-sections -o "XPMPAC~1.elf" "XPMPAC~1.o" 

XPMPAC~1.o:	../xpmPacman.c 
	$(CC) -c $(CCFLAGS)   "../xpmPacman.c" -o "XPMPAC~1.o" 

# tidy - delete all temporary files which are not involved in the target generation
tidy:
	rm -rf ./XPMPAC~1.d

# cleanup - delete all generated files
clean:	tidy
	rm -rf Release.hex
	rm -rf XPMPAC~1.elf
	rm -rf XPMPAC~1.o
