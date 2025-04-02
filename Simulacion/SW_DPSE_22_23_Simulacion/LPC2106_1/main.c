// =======================================================================
// Proyecto VIDEOCONSOLA Curso 2022-2023
// Autor: Jesús Manuel Hernández Mangas
// Asignatura: Desarrollo Práctico de Sistemas Electrónicos
// File: main.c  Programa principal
// =======================================================================
#include "system.h"
#include "xpm.h"

int GetAction();
// -----------------------------------------------------------------------------
int main (void)
{ 
 int x,y,i,k,t; 
   
 SCS = 1; // Muy importante para activar el FAST GPIO
 //          33222222222211111111110000000000
 //          10987654321098765432109876543210 
 FIODIR  = 0b11111111111111111111111111011101;  
 FIOSET  = 0xFFFFFFFF;  
 
 PINSEL0 = 0x00029505;
 PINSEL1 = 0x00000000;
 _delay_ms(100); // Espero a que el terminal del PC esté preparado

 _printf("Desarrollo Practico de Sistemas Electronicos\r\n"
         "Ingenieria de Sistemas Electronicos\r\n");

 AMPLIF_OFF;
 TFT_Init();    
 TP_Init();
 //TP_Calibrate();

 VibratorON(50);
 
 // Ejemplo de dibujo .....
 TFT_DrawFillSquareS(0,0,LENX,LENY,TFT_Color(0,0,0)); // Borro pantalla
 CF = (struct _current_font*) BigFont;				  // Selecciono fuente
 FG_Color = BLUE;									  // ForeGround Color
 BG_Color = TFT_Color(100,200,10);					  // BackGround Color
 xN = 4;											  // Tamaño fuente horizontal x2
 yN = 5;											  // Tamaño fuente vertical x3
 TFT_print_xNyN(100,100,"Hola Mundo");				  // Pinta "Hola Mundo" 

 TFT_DrawLine(0,0,LENX,LENY,WHITE);					  // Linea
 TFT_FillCircle(LENX/2, LENY/2, 30,WHITE); 
 
 TFT_FillCircle(LENX/4, LENY+3, 30,WHITE); 
 TFT_DrawLine(0,0,LENX,LENY,WHITE);					  // Linea
 
 XPM_Extract(bloques);
 xpm_sx = 15; xpm_sy = 15; // Tamaño de las teselas (tiles)
 XPM_SetxNyN( 5, 5);       // Amplificación
 i=0;
 
 xpm_xmirror  =1;
 
 for(k=0;k<10;k++) XPM_PintaAtxNyN(k*15*xpm_xn, 300, k, bloques);
 for(k=0;k<10;k++) XPM_PintaAtxNyN(k*15*xpm_xn, 300+15*xpm_yn, k+10, bloques);
  
 Timer_On();
 t=Get_Time();
 for(k=0;k<1;k++)
 { 
  for(y=0; y<LENY/xpm_sy; y++)
   for(x=0; x<LENX/xpm_sx; x++)
    XPM_PintaAt(x*xpm_sx, y*xpm_sy, (i++)&0b1111, bloques);   
  i++;
 }
 _printf("%d\n", Get_Time()-t);
 
 // -------------------------------------------------------
 //TP_Calibrate();
 // Test de los botones y el panel táctil 
 CS_ON;
 FIOSET = SD_CS|TP_CS;
 _printf("\r\n""BT ""(X   ,Y   )""\r\n");
 while(1)
 {
  x = GetAction();
  _printf("%3x", x);
  if(x&0x02) VibratorON(100);
  
  // Test Touch Pannel and TFT --------------  
  if( TP_Get_XY(&x,&y) ) 
  //if( TP_Get_XY_Cal(&x,&y) ) 
  {	  
   TFT_FillCircle(x,y,10,YELLOW); 
  }	  
  _printf("(%4d,%4d)", x,y); 
  _delay_ms(20);   
  _printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b"); 
 }
// -----------------------------------------
}   
