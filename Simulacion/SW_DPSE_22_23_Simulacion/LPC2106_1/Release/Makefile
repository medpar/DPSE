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
CCFLAGS=-gdwarf-2 -fomit-frame-pointer -Wall -fverbose-asm -MD -MP -fsigned-char -Os -mcpu=arm7tdmi -mthumb-interwork -I..
LDFLAGS=-mcpu=arm7tdmi -nostartfiles -mthumb-interwork

## Processing Tools
CC=arm-none-eabi-gcc.exe
LD=arm-none-eabi-gcc.exe
ASM=arm-none-eabi-gcc.exe
BIN2HEX=arm-none-eabi-objcopy.exe
#  C:/Users/Alonso/OneDrive/OneDrive - UVa/Desktop/CosasAlonso/2024ACTIVIDADES/clase40/2C/DPSE/SW_DPSE_22_23_Simulacion/LPC2106_1/Release/Release.hex: TOOL=BIN2HEX
#     C:/Users/Alonso/OneDrive/OneDrive - UVa/Desktop/CosasAlonso/2024ACTIVIDADES/clase40/2C/DPSE/SW_DPSE_22_23_Simulacion/LPC2106_1/Release/Release.elf: TOOL=LD
#        C:/Users/Alonso/OneDrive/OneDrive - UVa/Desktop/CosasAlonso/2024ACTIVIDADES/clase40/2C/DPSE/SW_DPSE_22_23_Simulacion/LPC2106_1/Release/crt.o: TOOL=ASM
#           C:/Users/Alonso/OneDrive/OneDrive - UVa/Desktop/CosasAlonso/2024ACTIVIDADES/clase40/2C/DPSE/SW_DPSE_22_23_Simulacion/LPC2106_1/crt.S
#        C:/Users/Alonso/OneDrive/OneDrive - UVa/Desktop/CosasAlonso/2024ACTIVIDADES/clase40/2C/DPSE/SW_DPSE_22_23_Simulacion/LPC2106_1/LPC2106_FLASH.ld
#        C:/Users/Alonso/OneDrive/OneDrive - UVa/Desktop/CosasAlonso/2024ACTIVIDADES/clase40/2C/DPSE/SW_DPSE_22_23_Simulacion/LPC2106_1/Release/main.o: TOOL=CC
#           C:/Users/Alonso/OneDrive/OneDrive - UVa/Desktop/CosasAlonso/2024ACTIVIDADES/clase40/2C/DPSE/SW_DPSE_22_23_Simulacion/LPC2106_1/main.c
#        C:/Users/Alonso/OneDrive/OneDrive - UVa/Desktop/CosasAlonso/2024ACTIVIDADES/clase40/2C/DPSE/SW_DPSE_22_23_Simulacion/LPC2106_1/Release/audio.o: TOOL=CC
#           C:/Users/Alonso/OneDrive/OneDrive - UVa/Desktop/CosasAlonso/2024ACTIVIDADES/clase40/2C/DPSE/SW_DPSE_22_23_Simulacion/LPC2106_1/audio.c
#        C:/Users/Alonso/OneDrive/OneDrive - UVa/Desktop/CosasAlonso/2024ACTIVIDADES/clase40/2C/DPSE/SW_DPSE_22_23_Simulacion/LPC2106_1/Release/big_font.o: TOOL=CC
#           C:/Users/Alonso/OneDrive/OneDrive - UVa/Desktop/CosasAlonso/2024ACTIVIDADES/clase40/2C/DPSE/SW_DPSE_22_23_Simulacion/LPC2106_1/big_font.c
#        C:/Users/Alonso/OneDrive/OneDrive - UVa/Desktop/CosasAlonso/2024ACTIVIDADES/clase40/2C/DPSE/SW_DPSE_22_23_Simulacion/LPC2106_1/Release/bloques.o: TOOL=CC
#           C:/Users/Alonso/OneDrive/OneDrive - UVa/Desktop/CosasAlonso/2024ACTIVIDADES/clase40/2C/DPSE/SW_DPSE_22_23_Simulacion/LPC2106_1/bloques.c
#        C:/Users/Alonso/OneDrive/OneDrive - UVa/Desktop/CosasAlonso/2024ACTIVIDADES/clase40/2C/DPSE/SW_DPSE_22_23_Simulacion/LPC2106_1/Release/CALIBR~1.o: TOOL=CC
#           C:/Users/Alonso/OneDrive/OneDrive - UVa/Desktop/CosasAlonso/2024ACTIVIDADES/clase40/2C/DPSE/SW_DPSE_22_23_Simulacion/LPC2106_1/calibrate.c
#        C:/Users/Alonso/OneDrive/OneDrive - UVa/Desktop/CosasAlonso/2024ACTIVIDADES/clase40/2C/DPSE/SW_DPSE_22_23_Simulacion/LPC2106_1/Release/NT35510.o: TOOL=CC
#           C:/Users/Alonso/OneDrive/OneDrive - UVa/Desktop/CosasAlonso/2024ACTIVIDADES/clase40/2C/DPSE/SW_DPSE_22_23_Simulacion/LPC2106_1/NT35510.c
#        C:/Users/Alonso/OneDrive/OneDrive - UVa/Desktop/CosasAlonso/2024ACTIVIDADES/clase40/2C/DPSE/SW_DPSE_22_23_Simulacion/LPC2106_1/Release/SMALL_~1.o: TOOL=CC
#           C:/Users/Alonso/OneDrive/OneDrive - UVa/Desktop/CosasAlonso/2024ACTIVIDADES/clase40/2C/DPSE/SW_DPSE_22_23_Simulacion/LPC2106_1/small_font.c
#        C:/Users/Alonso/OneDrive/OneDrive - UVa/Desktop/CosasAlonso/2024ACTIVIDADES/clase40/2C/DPSE/SW_DPSE_22_23_Simulacion/LPC2106_1/Release/vibrador.o: TOOL=CC
#           C:/Users/Alonso/OneDrive/OneDrive - UVa/Desktop/CosasAlonso/2024ACTIVIDADES/clase40/2C/DPSE/SW_DPSE_22_23_Simulacion/LPC2106_1/vibrador.c
#        C:/Users/Alonso/OneDrive/OneDrive - UVa/Desktop/CosasAlonso/2024ACTIVIDADES/clase40/2C/DPSE/SW_DPSE_22_23_Simulacion/LPC2106_1/Release/xpm.o: TOOL=CC
#           C:/Users/Alonso/OneDrive/OneDrive - UVa/Desktop/CosasAlonso/2024ACTIVIDADES/clase40/2C/DPSE/SW_DPSE_22_23_Simulacion/LPC2106_1/xpm.c
#        C:/Users/Alonso/OneDrive/OneDrive - UVa/Desktop/CosasAlonso/2024ACTIVIDADES/clase40/2C/DPSE/SW_DPSE_22_23_Simulacion/LPC2106_1/Release/xpt2046.o: TOOL=CC
#           C:/Users/Alonso/OneDrive/OneDrive - UVa/Desktop/CosasAlonso/2024ACTIVIDADES/clase40/2C/DPSE/SW_DPSE_22_23_Simulacion/LPC2106_1/xpt2046.c
#        C:/Users/Alonso/OneDrive/OneDrive - UVa/Desktop/CosasAlonso/2024ACTIVIDADES/clase40/2C/DPSE/SW_DPSE_22_23_Simulacion/LPC2106_1/Release/galaga.o: TOOL=CC
#           C:/Users/Alonso/OneDrive/OneDrive - UVa/Desktop/CosasAlonso/2024ACTIVIDADES/clase40/2C/DPSE/SW_DPSE_22_23_Simulacion/LPC2106_1/galaga.c
#        C:/Users/Alonso/OneDrive/OneDrive - UVa/Desktop/CosasAlonso/2024ACTIVIDADES/clase40/2C/DPSE/SW_DPSE_22_23_Simulacion/LPC2106_1/Release/PACMAN~1.o: TOOL=CC
#           C:/Users/Alonso/OneDrive/OneDrive - UVa/Desktop/CosasAlonso/2024ACTIVIDADES/clase40/2C/DPSE/SW_DPSE_22_23_Simulacion/LPC2106_1/pacmanXPM.c

# Build tree:
all:	Release

Release:	Release.hex
	cp "Release.hex" "C:/Users/Alonso/OneDrive/OneDrive - UVa/Desktop/CosasAlonso/2024ACTIVIDADES/clase40/2C/DPSE/SW_DPSE_22_23_Simulacion/LPC2106_1.hex"

Release.hex:	Release.elf 
	$(BIN2HEX) $(BIN2HEXFLAGS)  "Release.elf" "Release.hex"

Release.elf:	crt.o ../LPC2106_FLASH.ld main.o audio.o big_font.o bloques.o CALIBR~1.o NT35510.o SMALL_~1.o vibrador.o xpm.o xpt2046.o galaga.o PACMAN~1.o 
	$(LD) $(LDFLAGS)  -Wl,-Map,Release.map,--cref,--no-warn-mismatch,--start-group,--end-group,--gc-sections -o "Release.elf" "crt.o" -T "../LPC2106_FLASH.ld" "main.o" "audio.o" "big_font.o" "bloques.o" "CALIBR~1.o" "NT35510.o" "SMALL_~1.o" "vibrador.o" "xpm.o" "xpt2046.o" "galaga.o" "PACMAN~1.o" 

crt.o:	../crt.S 
	$(ASM) $(ASMFLAGS) -c  -o "crt.o" "../crt.S"

main.o:	../main.c 
	$(CC) -c $(CCFLAGS)   "../main.c" -o "main.o" 

audio.o:	../audio.c 
	$(CC) -c $(CCFLAGS)   "../audio.c" -o "audio.o" 

big_font.o:	../big_font.c 
	$(CC) -c $(CCFLAGS)   "../big_font.c" -o "big_font.o" 

bloques.o:	../bloques.c 
	$(CC) -c $(CCFLAGS)   "../bloques.c" -o "bloques.o" 

CALIBR~1.o:	../calibrate.c 
	$(CC) -c $(CCFLAGS)   "../calibrate.c" -o "CALIBR~1.o" 

NT35510.o:	../NT35510.c 
	$(CC) -c $(CCFLAGS)   "../NT35510.c" -o "NT35510.o" 

SMALL_~1.o:	../small_font.c 
	$(CC) -c $(CCFLAGS)   "../small_font.c" -o "SMALL_~1.o" 

vibrador.o:	../vibrador.c 
	$(CC) -c $(CCFLAGS)   "../vibrador.c" -o "vibrador.o" 

xpm.o:	../xpm.c 
	$(CC) -c $(CCFLAGS)   "../xpm.c" -o "xpm.o" 

xpt2046.o:	../xpt2046.c 
	$(CC) -c $(CCFLAGS)   "../xpt2046.c" -o "xpt2046.o" 

galaga.o:	../galaga.c 
	$(CC) -c $(CCFLAGS)   "../galaga.c" -o "galaga.o" 

PACMAN~1.o:	../pacmanXPM.c 
	$(CC) -c $(CCFLAGS)   "../pacmanXPM.c" -o "PACMAN~1.o" 

# tidy - delete all temporary files which are not involved in the target generation
tidy:
	rm -rf crt.hex
	rm -rf crt.err
	rm -rf crt.lst
	rm -rf ./main.d
	rm -rf ./audio.d
	rm -rf ./big_font.d
	rm -rf ./bloques.d
	rm -rf ./CALIBR~1.d
	rm -rf ./NT35510.d
	rm -rf ./SMALL_~1.d
	rm -rf ./vibrador.d
	rm -rf ./xpm.d
	rm -rf ./xpt2046.d
	rm -rf ./galaga.d
	rm -rf ./PACMAN~1.d

# cleanup - delete all generated files
clean:	tidy
	rm -rf Release.hex
	rm -rf Release.elf
	rm -rf crt.o
	rm -rf main.o
	rm -rf audio.o
	rm -rf big_font.o
	rm -rf bloques.o
	rm -rf CALIBR~1.o
	rm -rf NT35510.o
	rm -rf SMALL_~1.o
	rm -rf vibrador.o
	rm -rf xpm.o
	rm -rf xpt2046.o
	rm -rf galaga.o
	rm -rf PACMAN~1.o
