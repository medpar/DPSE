// =======================================================================
// Proyecto 
// Fecha: 2023/03/21
// Autor: Jesús M. Hernández Mangas
// Asignatura: 
// File: xpt2046.c  Rutinas para manejar el controlador del TouchPannel
// =======================================================================
#include "system.h"
// -----------------------------------------------------------------
void TP_Init()
{
 PINSEL0 = (PINSEL0 & (~((0b111111)<<8))) | ( 0b010101 << 8 ); 
 S0SPCR = 0b0000100000100000; 
		//	    ....			BITS = 1000, 8 bits per transfer
		//          .           SPIE = 0, no interrupts
		//           .          LSBF = 0, MSBit first
		//            .			MSTR = 1, Master mode
		//             .        CPOL = 0
	    //              .       CPHA = 0
		//               .      BitEnable = 0, 8 bit transfers
 S0SPCCR =30;
		// BitRate = PCLK/S0SPCCR = 4*14745600/30 = aprox 1966 kHz
 FIODIR |= TP_CS; 
}
// -----------------------------------------------------------------
unsigned char TP_EnviaRecibe(unsigned char d)
{
 S0SPDR = d;
 while(! (S0SPSR&(1<<7)) ); // Hasta que se envie bloqueado aquí
 return S0SPDR;	 
}
// -----------------------------------------------------------------
#define TP_Select()    FIOCLR = TP_CS;
#define TP_Unselect()  FIOSET = TP_CS;
// Parece que en el módulo de 3.98 pulgadas están intercambiadas las coordenadas x e y
#define TP_GET_X  0b11010000
#define TP_GET_Y  0b10010000

#ifdef REAL_SYSTEM
 int X_SINPULSAR = 799;
#else
int X_SINPULSAR = 1;
#endif
// -----------------------------------------------------------------
#ifdef REAL_SYSTEM
 MATRIX Cal={298680,760,-12782360,0,306240,-8004000,273528};  // Video consola 2022--2023 pantalla 3.98"
#else
 MATRIX Cal={52360,440,-2048380,0,55912,70372,28679};
#endif
// -----------------------------------------------------------------
int TP_Get_XY(int *x, int *y)
{
 unsigned char dh, dl;	
 TP_Select();
 TP_EnviaRecibe(TP_GET_Y);
 dh = TP_EnviaRecibe(0);
 dl = TP_EnviaRecibe(0);
 x[0] = (((dh<<8)|dl)>>3); // 12 bits	
 x[0] = ((x[0]*LENX)/4096);	
 TP_Unselect();
 
 TP_Select();	
 TP_EnviaRecibe(TP_GET_X);
 dh = TP_EnviaRecibe(0);
 dl = TP_EnviaRecibe(0);
 y[0] = (((dh<<8)|dl)>>3); // 12 bits	
 #ifdef REAL_SYSTEM
  y[0] = (y[0]*LENY)/4096;	
 #else
  y[0] = (y[0]*LENY)/4096;	
 #endif
 TP_Unselect();	 
//	_printf(" %d",x[0]);
 if((x[0]==X_SINPULSAR ) ) return 0;
 else 					   return 1;
}
// -----------------------------------------------------------------
int TP_Get_XY_Cal(int *x, int *y)
{
 if(!TP_Get_XY(x,y)) return 0;	
 POINT Real, Measured;
 Measured.x = *x;
 Measured.y = *y;	
 if( getDisplayPoint( &Real, &Measured, &Cal ) == OK)
 {
  *x = Real.x;
  *y = Real.y; 	

  return 1;
 }
 *x=0;
 *y=0;
 	
 return 0;
}
// -----------------------------------------------------------------
void TP_Aspa(int x, int y, int color)
{
 int l = 5;
 TFT_DrawLine( x-l, y, x+l, y, YELLOW);
 TFT_DrawLine( x, y-l, x, y+l, YELLOW);	
 TFT_DrawCircle(x,y,l*3,color);
}
// -----------------------------------------------------------------
#define RELEASE_TP while(  TP_Get_XY(&x,&y)) _delay_ms(10);
#define GET_XY_TP  while( !TP_Get_XY(&x,&y)) _delay_ms(10);
void TP_Calibrate()
{
 #define X0 20
 #define Y0 20
 #define X1 LENX-20
 #define Y1 20
 #define X2 20
 #define Y2 LENY-20 	
 #define CC RED
 #define NCAL 10

 int x,y,i,j;
 POINT TFT[3]={{X0,Y0},{X1,Y1},{X2,Y2}};
 POINT TP[3]={{0,0},{0,0},{0,0}};
	
 TFT_DrawFillSquare(0,0,LENX-1,LENY-1,BLACK);	
 BG_Color = BLACK;
 FG_Color = WHITE;
 TFT_print_centered_x(LENY/2,"Touch Pannel Calibration");
	
 for(j=0;j<3;j++)
 {
  TP_Aspa(TFT[j].x,TFT[j].y,CC);
  for(i=0;i<NCAL;i++)
  {
   GET_XY_TP;
   _printf("%3d:%3d\r\n",x,y);
    TP[j].x +=x; TP[j].y +=y;
  }
  TP[j].x /= NCAL;
  TP[j].y /= NCAL;
  RELEASE_TP;
  _printf("-->%3d:%3d\r\n",TP[j].x,TP[j].y);
  }
 
 setCalibrationMatrix( TFT, TP, &Cal);
 // Pinta la matriz de calibracion
 _puts("Matriz de calibracion\r\n");
 _printf("An = %8d\r\n",Cal.An);
 _printf("Bn = %8d\r\n",Cal.Bn);
 _printf("Cn = %8d\r\n",Cal.Cn);
 _printf("Dn = %8d\r\n",Cal.Dn);
 _printf("En = %8d\r\n",Cal.En);
 _printf("Fn = %8d\r\n",Cal.Fn);
 _printf("Div = %7d\r\n",Cal.Divider);
}
// -----------------------------------------------------------------