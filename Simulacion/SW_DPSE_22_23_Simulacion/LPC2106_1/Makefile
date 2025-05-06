# Makefile 
# Ingenier a de Sistemas Electr nicos
# Desarrollo Pr ctico de Sistemas Electr nicos
# Jes s M. Hern ndez
# 2023/03/21

SHELL = $(COMSPEC) /c
SRC = crt.S main.c \
	NT35510.c \
	calibrate.c xpt2046.c  \
	bloques.c xpm.c \
	vibrador.c big_font.c small_font.c audio.c \
	galaga.c pacmanXPM.c xpmschulos.c
	
INC = nt35510.h calibrate.h xpt2046.h system.h audio.h 

DEP = $(SRC) $(INC) Makefile LPC2106_FLASH.ld
USB_COM =$(shell .\detect_USB_COM.exe)
#USB_COM = COM3
MODEL=2023

TOOL = arm-none-eabi
BT2	 = ../bootloader2103/bt2.exe -c -s 115200 -sb 115200
ISP  = ../bootloader2103/lpc21isp_197.exe
CC   = $(TOOL)-gcc
LD   = $(TOOL)-ld -v
AR   = $(TOOL)-ar
AS   = $(TOOL)-as
CP   = $(TOOL)-objcopy
OD	 = $(TOOL)-objdump
SZ   = $(TOOL)-size

CFLAGS  = -I./ -fno-common -O2 -g -nostartfiles -static -mcpu=arm7tdmi -mthumb-interwork -MD -MP -fomit-frame-pointer
LF   = LPC2106_FLASH.ld

all: burn

ram: code_ram.bin 
	$(BT2) -l code_ram.bin
%.bin: %.elf
	$(CP) -O binary $< $@
%.hex: %.elf
	$(CP) -O ihex $< $@
code_ram.elf: $(DEP)
	$(CC) $(CFLAGS) -Wl,-Tlinker_ram.ld -o $@ $(SRC)
code_flash.elf: $(DEP) 
	$(CC) $(CFLAGS) -DREAL_SYSTEM -DMODEL=$(MODEL) -Wl,-T$(LF) -Wl,-Map=a.map,--cref,--no-warn-mismatch,--start-group,--end-group,--gc-sections -o $@ $(SRC)
burn: code_flash.hex
	$(SZ) code_flash.elf
	$(OD) -d -S code_flash.elf > code_flash.S
#	$(ISP) -control -term code_flash.hex //./$(USB_COM) 115200 14746	
#	$(ISP) -control code_flash.hex //./$(USB_COM) 115200 14746	
	$(ISP) -control code_flash.hex //./$(USB_COM) 230400 14746	
	$(ISP) -termonly code_flash.hex //./$(USB_COM) 115200 14746
burn2: code_flash.hex 
	$(SZ) code_flash.elf	
	$(OD) -d -S code_flash.elf > code_flash.S
	$(ISP) -controlswap -control code_flash.hex //./$(USB_COM) 230400 14746	
	$(ISP) -controlswap -termonly code_flash.hex //./$(USB_COM) 115200 14746

dis:
	$(OD) -d -S code_flash.elf > code_flash.S
clean:
	del *.lst
	del	*.bin 
	del *.o 
	del *.hex 
	del *.elf
terminal:
	$(ISP) -termonly code_flash.hex //./$(USB_COM) 115200 14746
reset:
	$(ISP) -termonly code_flash.hex //./$(USB_COM) 115200 14746
reset2:
	$(ISP) -controlswap -termonly code_flash.hex //./$(USB_COM) 115200 14746
	