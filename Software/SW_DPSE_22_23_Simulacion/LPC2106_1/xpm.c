// =======================================================================
// Proyecto VIDEOCONSOLA Curso 2022-2023
// Fecha: 2023/03/21
// Autor: Jesús M. Hernández Mangas
// Asignatura: Desarrollo Práctico de Sistemas Electrónicos
// File: xpm.c  Rutinas manejo dibujos xpm (X-PixMap)
// =======================================================================
#include <stdlib.h>
#include "system.h"
#include "xpm.h"
//#define DEBUGXPM
// -------------------------------------------------------------------------
// Tenemos que reducir la profundidad de color para que la paleta
//  tenga como mucho 92 colores
// Obtenido con ImageMagick. Comando:
// >magick.exe a.png -colors 32 a.xpm
// Trabajaremos con ficheros con una paleta de 2-64 colores

// El orden de las claves es: (máx 92 colores) generadas con imagemagick
// " .XoO+@#$%&*=-;:>,<1234567890qwertyuipasdfghjk"
// "lzxcvbnmMNBVCZASDFGHJKLPIUYTREWQ!~^/()_`'][{}|";

// ¡OJO! En el campo de colores podría aparecer el nombre de los colores
//  como 'red','yellow','cyan',... en vez de la codificación #RRGGBB y
//  nuestro código no está preparado para eso. ¡Hay que cambiarlo!

// Para ver de qué formato gráfico es un fichero y obtener información
//  >magick.exe identify a.png
//  >magick.exe identify -verbose a.png
// También:
//  >magick.exe identify a.xpm.c
// a.xpm.c XPM 32x32 32x32+0+0 8-bit sRGB 16c 1542B 0.000u 0:00.001

// -------------------------------------------------------------------------
unsigned short paleta[96]; // Máximo 96 colores (128 bytes) No cambiar.
#define TFT_Color(r,g,b) (((r>>3)<<11) | ((g>>2)<<5) | (b>>3))
int xpm_ancho, xpm_alto, xpm_ncol, xpm_cpp;
int xpm_sx, xpm_sy; // Tamaño en pixel de los sprites
// -------------------------------------------------------------------------
int _atoi(char * s)
{ // Solo números positivos. La rutina "atoi" original da problemas de "Data abort"
 int i = 0;
 while(*s==' ') s++; // Elimino espacios
 //_printf("<%s>\n",s);
 while(*s>='0' && *s<='9')
 {
  i*=10;
  i+=(*s-'0');
  s++;
 }
 return i;
}
// -------------------------------------------------------------------------
void XPM_Extract(char ** xpm)
{
 char *p;
 int i ,j;
 unsigned int r,g,b;
 
 p = xpm[0];
/* <ancho/columnas> <altura/filas> <colores> <caracteres por pixel> */
//"48 4 2 1 ",
 i=0; j=0;
 while(p[i]!=' ') i++; 
 xpm_ancho = _atoi(&p[j]); j = i++;
 while(p[i]!=' ') i++;
 xpm_alto  = _atoi(&p[j]); j = i++;
 while(p[i]!=' ') i++;
 xpm_ncol  = _atoi(&p[j]); j = i++; 
 while(p[i]!=' ') i++;
 xpm_cpp   = _atoi(&p[j]); j = i++;
 #ifdef DEBUGXPM
 _printf("ancho = %d\r\n", xpm_ancho);
 _printf("alto  = %d\r\n", xpm_alto);
 _printf("n_col = %d\r\n", xpm_ncol);
 _printf("cpp   = %d\r\n", xpm_cpp);
 #endif
/* <Colores> */
//"a c #ffffff",
//"b c #000000",
 //if(xpm_cpp!=1) _printf("Fichero .xpm no soportado\r\n");
 // Procesa paleta de colores. Macro para convertir ASCII_HEX
 #define B(a) ((a>'9')?(a-'A'+10): (a-'0'))
 if(xpm_ncol>96) _printf("XPM palette error (%d colors)\n",xpm_ncol);
 for(j=1;j<(1+xpm_ncol);j++)
 {
  p = xpm[j];
  r = (B(p[5])<<4) | B(p[6]);
  g = (B(p[7])<<4) | B(p[8]);
  b = (B(p[9])<<4) | B(p[10]);
  
  paleta[ (int)(p[0]-32) ] = TFT_Color(r,g,b);
  #ifdef DEBUGXPM
  _printf("Paleta[%2d] %c (%02x,%02x,%02x) %04x\r\n",j,p[0], r,g,b, paleta[ p[0]-32 ]);
  #endif
 } 
 #ifdef DEBUGXPM
 _printf("OK\n");
 #endif
}
// -------------------------------------------------------------------------
void XPM_PintaAll(int x, int y, char** xpm)
{ // Pinta toda la imagen xpm (depuración)
 int i,j;
 char *p;
 CS_OFF;
 TFT_SetArea( x, y, x+xpm_ancho-1, y+xpm_alto-1 );
 RS_ON;

 for(j=0;j<xpm_alto;j++)
 {
  p = xpm[xpm_ncol+1+j]; // fila
  for(i=0;i<xpm_ancho;i++)
  { 
   //SSD1963_Data_(paleta[ p[(int) i]-32 ]);
   NT35510_Data_(paleta[ p[(int) i]-32 ]);
  }
 }
 CS_ON;
}
// -------------------------------------------------------------------------
#define TESELAS_X (xpm_ancho/xpm_sx)
// -------------------------------------------------------------------------
inline void XPM_PintaAt(int x, int y, int sprite, char** xpm)
{ // Pinta la tesela (tile/sprite) seleccionada en (x,y)
 int i,j,nx,ny;
 int col, fila;
 char *p;
 
 if(x>=LENX || y>= LENY) return;
 CS_OFF;
 TFT_SetArea( x, y, x+xpm_sx-1, y+xpm_sy-1 );
 RS_ON;
 
 ny = sprite/TESELAS_X;
 nx = sprite - TESELAS_X*ny; 
 col  = nx*xpm_sx;
 fila = ny*xpm_sx+1+xpm_ncol;
//_printf("Sprite %3d, (nx=%2d,ny=%2d)\r\n",sprite,nx,ny);
 for(j=0; j<xpm_sy; j++)
 {
  p = xpm[fila+j]; // fila
  for(i=col; i<col+xpm_sx; i++)
  { 
   NT35510_Data_(paleta[ p[(int) i]-32 ]);
  }
 }
 CS_ON;
}
// -------------------------------------------------------------------------
int xpm_xn = 1;
int xpm_yn = 1;
int xpm_sxn = 1;
int xpm_syn = 1;
int teselas_x;
// -------------------------------------------------------------------------
void XPM_SetxNyN(int xn, int yn)
{
 xpm_xn = xn;
 xpm_yn = yn; 
 xpm_sxn = xn * xpm_sx;
 xpm_syn = yn * xpm_sy;
 teselas_x = TESELAS_X;
}
// -------------------------------------------------------------------------
int xpm_xmirror = 0;
inline void XPM_PintaAtxNyN(int x, int y, int sprite, char** xpm)
{ // Pinta la tesela (tile/sprite) seleccionada en (x,y)
 int i,j,nx,ny, xn,yn, yi = y;
 unsigned short vp;
 int col, fila;
 char *p;
 
 int sxn_eff    = xpm_sxn;
 int xpm_sx_eff = xpm_sx;
 
 if((x+sxn_eff)>=LENX) {sxn_eff = LENX - x; xpm_sx_eff = sxn_eff/xn;};
 
 if(x>=LENX || y>= LENY) return;
 
 CS_OFF;
 TFT_SetArea( x, y, x+sxn_eff-1, y+xpm_syn-1 ); 
 RS_ON;
 
// ny = sprite/TESELAS_X;
// nx = sprite - TESELAS_X*ny; 
 ny = sprite / teselas_x;
 nx = sprite % teselas_x;
 
 col  = nx*xpm_sx;
 fila = ny*xpm_sx+1+xpm_ncol;
 
 //_printf("Sprite %3d, (nx=%2d,ny=%2d)\r\n",sprite,nx,ny);
 
 if(!xpm_xmirror) // Pinta de izquierda a derecha
 for(j=0; j<xpm_sy; j++)
 {
  for(yn=0;yn<xpm_yn;yn++)
  {	  
   if(yi>LENY-1) {CS_ON;return;};
   yi++;
   p = xpm[fila+j]; // fila
   for(i=col; i<col+xpm_sx_eff; i++)
   {     
    vp = paleta[ p[(int) i]-32];
    for(xn=0;xn<xpm_xn; xn++) NT35510_Data_( vp );
   }
  }
 }     
 else           // Pinta de derecha a izquierda
 for(j=0; j<xpm_sy; j++)
 {
  for(yn=0;yn<xpm_yn;yn++)
  {	  
   if(yi>LENY-1) {CS_ON;return;};
   yi++;
   p = xpm[fila+j]; // fila
   //for(i=col; i<col+xpm_sx; i++)
   for(i=col+xpm_sx_eff-1; i>=col; i--)       
   {     
    vp = paleta[ p[(int) i]-32];
    for(xn=0;xn<xpm_xn; xn++) NT35510_Data_( vp );
   }
  }
 }
 CS_ON;
}
// -------------------------------------------------------------------------
