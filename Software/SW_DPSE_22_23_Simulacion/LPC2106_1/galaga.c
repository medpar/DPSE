// =======================================================================
// Proyecto VIDEOCONSOLA Curso 2022-2023
// Autor: Jesús Manuel Hernández Mangas
// Asignatura: Desarrollo Práctico de Sistemas Electrónicos
// File: galaga.c
// =======================================================================
#include "system.h"
// -----------------------------------------------------------------------
#define LEFT	(1<< 0)
#define RIGHT   (1<< 1)
#define UP      (1<< 2)
#define DOWN    (1<< 3)
#define T_DOWN  (1<< 4)
#define T_LEFT  (1<< 5)
#define T_RIGHT (1<< 6)
#define T_UP    (1<< 7)

// -----------------------------------------------------------------------
int GetAction()
{
 int x, out=0;
 // Pines como entradas
 FIODIR &= ~0b111111110000000000000000;
 _delay_ms(5);
 // Lectura
 x = ~((FIOPIN >>16))&0xFF;
 if(x & (1<<0)) out |= UP;
 if(x & (1<<1)) out |= LEFT;
 if(x & (1<<2)) out |= RIGHT;
 if(x & (1<<3)) out |= DOWN;
 
 if(x & (1<<4)) out |= T_UP;
 if(x & (1<<5)) out |= T_LEFT;
 if(x & (1<<6)) out |= T_RIGHT;
 if(x & (1<<7)) out |= T_DOWN;
 // Pines como salidas
 FIODIR |= 0b111111110000000000000000;
 
 return out;
}