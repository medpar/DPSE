/*****************************************************************************
 *  Solomon Systech. NT35510 LCD controller driver
 *****************************************************************************/

#ifndef _NT35510_H
#define _NT35510_H

#define CMD_NOP                     0x00
#define CMD_SOFT_RESET              0x01
#define CMD_GET_PWR_MODE            0x0A
#define CMD_GET_ADDR_MODE           0x0B
#define CMD_GET_DISPLAY_MODE        0x0D
#define CMD_GET_TEAR_EFFECT_STATUS  0x0E
#define CMD_ENT_SLEEP               0x10
#define CMD_EXIT_SLEEP              0x11
#define CMD_ENT_PARTIAL_MODE        0x12
#define CMD_ENT_NORMAL_MODE         0x13
#define CMD_EXIT_INVERT_MODE        0x20
#define CMD_ENT_INVERT_MODE         0x21
#define CMD_SET_GAMMA               0x26
#define CMD_BLANK_DISPLAY           0x28
#define CMD_ON_DISPLAY              0x29
#define CMD_SET_COLUMN              0x2A00 // NT35510 OK
#define CMD_SET_PAGE                0x2B00 // NT35510 OK

#define CMD_WR_MEMSTART             0x2C00 // NT35510 OK
#define CMD_RD_MEMSTART             0x2E00 // NT35510 OK

#define CMD_SET_PARTIAL_AREA        0x30
#define CMD_SET_SCROLL_AREA         0x33
#define CMD_SET_TEAR_OFF            0x34
#define CMD_SET_TEAR_ON             0x35
#define CMD_SET_ADDR_MODE           0x36
#define CMD_SET_SCROLL_START        0x37
#define CMD_EXIT_IDLE_MODE          0x38
#define CMD_ENT_IDLE_MODE           0x39

#define CMD_SET_PIXEL_FORMAT        0x3A

#define CMD_WR_MEM_AUTO             0x3C
#define CMD_RD_MEM_AUTO             0x3E
#define CMD_SET_TEAR_SCANLINE       0x44
#define CMD_GET_SCANLINE            0x45
#define CMD_RD_DDB_START            0xA1
#define CMD_SET_PANEL_MODE          0xB0
#define CMD_GET_PANEL_MODE          0xB1
#define CMD_SET_HOR_PERIOD          0xB4
#define CMD_GET_HOR_PERIOD          0xB5
#define CMD_SET_VER_PERIOD          0xB6
#define CMD_GET_VER_PERIOD          0xB7
#define CMD_SET_GPIO_CONF           0xB8
#define CMD_GET_GPIO_CONF           0xB9
#define CMD_SET_GPIO_VAL            0xBA
#define CMD_GET_GPIO_STATUS         0xBB
#define CMD_SET_POST_PROC           0xBC
#define CMD_GET_POST_PROC           0xBD
#define CMD_SET_PWM_CONF            0xBE
#define CMD_GET_PWM_CONF            0xBF
#define CMD_SET_LCD_GEN0            0xC0
#define CMD_GET_LCD_GEN0            0xC1
#define CMD_SET_LCD_GEN1            0xC2
#define CMD_GET_LCD_GEN1            0xC3
#define CMD_SET_LCD_GEN2            0xC4
#define CMD_GET_LCD_GEN2            0xC5
#define CMD_SET_LCD_GEN3            0xC6
#define CMD_GET_LCD_GEN3            0xC7
#define CMD_SET_GPIO0_ROP           0xC8
#define CMD_GET_GPIO0_ROP           0xC9
#define CMD_SET_GPIO1_ROP           0xCA
#define CMD_GET_GPIO1_ROP           0xCB
#define CMD_SET_GPIO2_ROP           0xCC
#define CMD_GET_GPIO2_ROP           0xCD
#define CMD_SET_GPIO3_ROP           0xCE
#define CMD_GET_GPIO3_ROP           0xCF
#define CMD_SET_ABC_DBC_CONF        0xD0
#define CMD_GET_ABC_DBC_CONF        0xD1
#define CMD_SET_DBC_THRES           0xD4
#define CMD_GET_DBC_THRES           0xD5
#define CMD_PLL_START               0xE0
#define CMD_SET_PLL_MN              0xE2
#define CMD_GET_PLL_MN              0xE3
#define CMD_GET_PLL_STATUS          0xE4
#define CMD_ENT_DEEP_SLEEP          0xE5
#define CMD_SET_PCLK                0xE6
#define CMD_GET_PCLK                0xE7
#define CMD_SET_DATA_INTERFACE      0xF0
#define CMD_GET_DATA_INTERFACE      0xF1

#define NT35510_CS (1<<12)
#define NT35510_WR (1<<11)
#define NT35510_RD (1<<15)
#define NT35510_RS (1<<13)
#define NT35510_DT (16)

#define RS_ON  FIOSET = NT35510_RS
#define CS_ON  FIOSET = NT35510_CS
#define RD_ON  FIOSET = NT35510_RD
#define WR_ON  FIOSET = NT35510_WR

#define RS_OFF FIOCLR = NT35510_RS
#define CS_OFF FIOCLR = NT35510_CS
#define RD_OFF FIOCLR = NT35510_RD
#define WR_OFF FIOCLR = NT35510_WR

#define DT_OUTPUT FIODIR = FIODIR |  (0xFFFF<<NT35510_DT)
#define DT_INPUT  FIODIR = FIODIR &(~(0xFFFF<<NT35510_DT))

// ---------------------------------------------------
extern void NT35510_Cmd( unsigned short cmd);
// ---------------------------------------------------
extern void NT35510_Data( unsigned short data);
// ---------------------------------------------------
extern void NT35510_Data_( unsigned short data);
// ---------------------------------------------------
extern void NT35510_Reg( unsigned short cmd, unsigned short data);
// ---------------------------------------------------
extern void TFT_SetArea(int x0, int y0, int x1, int y1 );

unsigned short NT35510_ReadCMD( unsigned short cmd );
void TFT_Init();

//void TFT_SetArea(int x0, int y0, int x1, int y1 );
void TFT_FillArea(unsigned short x0, unsigned short y0,
		       unsigned short x1, unsigned short y1 );

void TFT_PutPixel(int x0, int y0);
void TFT_PutPixelColor(unsigned short x0, unsigned short y0, unsigned short color);
void TFT_DrawLine(int x1, int y1, int x2, int y2, int color);

void TFT_HorizontalLine(int x1, int x2, int y, int color);
void TFT_VerticalLine(int x, int y1, int y2, int color);
void TFT_DrawSquare(int x1, int y1, int x2, int y2, int color);
void TFT_DrawCircle(int x, int y, int radius, int color);
void TFT_FillCircle(int x, int y, int radius, int color);
void TFT_DrawFillSquare(int x1, int y1, int x2, int y2, int color);
void TFT_DrawFillSquareS(int x1, int y1, int sx, int sy, int color);

void TFT_putch(int x1, int y1, unsigned char c);
void TFT_print(int x1, int y1, char * s);
void TFT_print_centered_x(int y1, char * s);

extern int tft_x, tft_y;
void TFT_putchar(unsigned char c);
//void TFT_printf(int x, int y, char *format, ...);
#include <stdarg.h>
#define TFT_printf( x,y,format, ...) \
   tft_x = (x);\
   tft_y = (y);\
   _vputch =(void*)TFT_putchar;\
   _printf(format, __VA_ARGS__);\
   _vputch =(void*)_U0putch;
   
extern int xN;
extern int yN;
void TFT_putch_xNyN(int x1, int y1, unsigned char c);
void TFT_print_xNyN(int x1, int y1, char * s);
int TFT_print_centered_x_xNyN(int y1, char * s);
void TFT_putchar_xNyN(unsigned char c);

// Ojo!, en los argumentos x e y poner las expresiones entre paréntesis
//  de lo contrario al pasar __VA_ARGS__ se cuelga
#define TFT_printf_xNyN( x,y,format, ...) \
   tft_x = (x);\
   tft_y = (y);\
   _vputch =(void*)TFT_putchar_xNyN;\
   _printf(format, __VA_ARGS__);\
   _vputch =(void*)_U0putch;


#define _sprintf( sdir,format, ...) \
   dir = sdir;\
   _vputch =(void*)_sputchar;\
   _printf(format, __VA_ARGS__);\
   _vputch =(void*)_U0putch;

struct _current_font
{
	unsigned char x_size;
	unsigned char y_size;
	unsigned char x_step;
	unsigned char offset;
	unsigned char numchars;
	unsigned char font[];
};
extern unsigned char SmallFont[];
extern unsigned char BigFont[];
extern unsigned char SevenSegNumFont[];
extern struct _current_font *CF;
void TFT_DumpBinaryFont();

#define TFT_Color(r,g,b) (((r>>3)<<11) | ((g>>2)<<5) | (b>>3))
#define GetRED(c) ((c>>8)&0b11111000)
#define GetGREEN(c) ((c>>3)&0b11111100)
#define GetBLUE(c) ((c<<3)&0b11111000)

#define BLACK  TFT_Color(  0,  0,  0)
#define RED    TFT_Color(255,  0,  0)
#define GREEN  TFT_Color(  0,255,  0)
#define BLUE   TFT_Color(  0,  0,255)
#define YELLOW TFT_Color(255,255,  0)
#define CYAN   TFT_Color(  0,255,255)
#define MAGENT TFT_Color(255,  0,255)
#define WHITE  TFT_Color(255,255,255)

#define ORANGE TFT_Color(255,0xA5,  0)
#define BROWN  TFT_Color(100,  0,  0)

#define COL_ON  TFT_Color(200,200, 20)
#define COL_OFF TFT_Color(50,50,  0)

#define CRLF  _U0puts("\r\n");
extern unsigned short FG_Color;
extern unsigned short BG_Color;
   
#define TFT_PRINT_xN_yN
#define TFT_800x480

extern int LENX, LENY;

void TFT_On();
void TFT_Off();

void NT35510_WritePage(int page, unsigned short * data);
void NT35510_ReadPage(int page, unsigned short * data);
void NT35510_TestPages();
#endif // _NT35510_H
