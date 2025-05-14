// =======================================================================
// Proyecto 
// Fecha:
// Autor: 
// Asignatura: 
// File: system.h Definiciones útiles e #includes
// =======================================================================
void _puts(char *);
void _printf(char *,...);
extern void (*_vputch)(int);
extern int (*_vgetch)();
#define _putch(d) ((*_vputch)(d))
#define _getch() ((*_vgetch)())
int	_gets(unsigned char *,int);
void _U0putch(unsigned char);
unsigned char _U0getch();
void _delay_loop(unsigned int);

#define FOSC    14745600        // Crystal frequency
#define MSEL    4               // PLL multiplier
//#define MSEL    5               // PLL multiplier
#define CCLK    (FOSC*MSEL)	    // CPU clock
#define PCKDIV  1               // APB divider
#define PCLK    (CCLK/PCKDIV)   // Peripheral clock

// delays
#define _delay_us(n) _delay_loop(CCLK/400*n/10000-1)
#define _delay_ms(n) _delay_loop(CCLK/4000*n-1)

#include "lpc2106.h"
#include "stdlib.h"
#include "calibrate.h"
#include "xpt2046.h"
//#include "SSD1963.h"
#include "NT35510.h"

#include "audio.h"

#ifndef REAL_SYSTEM
#undef FIODIR
#undef FIOPIN
#undef FIOSET
#undef FIOCLR

#define FIODIR IODIR
#define FIOPIN IOPIN 
#define FIOSET IOSET
#define FIOCLR IOCLR
#endif

#define   TP_CS (1<< 2)
#define   SD_CS (1<< 3)

#ifndef MODEL
 #define MODEL 2023
#endif

#if MODEL == 2020
 #define GPIO_CS (1<<7)
 #define  ADC_CS (1<<9)
 #define  ACL_CS (1<<10)
 #define  TFT_CS (1<<12)
 #define AUDIO_OUT (1<<8) 
 
#endif

#if MODEL == 2021
 #define GPIO_CS (1<<10)
 #define  ADC_CS (1<<12)
 #define  TFT_CS (0)
 #define  ACL_CS (0)
 #define AUDIO_OUT (1<<7)

#endif

#if MODEL == 2022
 #define GPIO_CS (1<<10)
 #define  ADC_CS (1<<12)
 #define  TFT_CS (0)
 #define  ACL_CS (1<< 9)
 #define AUDIO_OUT (0)

#endif

#if MODEL == 2023
 
 #define  TFT_CS (1<<12)
 #define AUDIO_OUT (0)
 #define VIBRADOR (1<<9)
#endif


#define AUDIO_ON  FIOSET = AUDIO_OUT
#define AUDIO_OFF FIOCLR = AUDIO_OUT

#define AMPLIF_ON  FIOSET = (1<<14)
#define AMPLIF_OFF FIOCLR = (1<<14)

// Sin panel tactil
//#undef _XPT2046_C_
//#undef _CALIBRATE_C_
#define _XPT2046_C_
#define _CALIBRATE_C_
/*
#define Timer_On()    \
 T1TCR  = 0b00000010; \
 T1PR   = 58;         \
 T1MR0  = (1<<31);    \
 T1MCR  = 0b010;      \
 T1CTCR = 0;          \
 T1TC   = 0;          \
 T1TCR  = 0b00000001; 
// Cuenta microsegundos, desborda en 2^32 usec -> 4000 segundos
*/

#define Timer_On()    \
 T1TCR  = 0b00000010; \
 T1PR   = PCLK/1000;  \
 T1MR0  = (1<<31);    \
 T1MCR  = 0b010;      \
 T1CTCR = 0;          \
 T1TC   = 0;          \
 T1TCR  = 0b00000001; 
// Cuenta microsegundos, desborda en 2^32 usec -> 4000 segundos

#define Get_Time() T1TC
#define VIBRATOR_ON FIOCLR = VIBRADOR
#define VIBRATOR_OFF FIOSET = VIBRADOR


void VibratorON(unsigned int ms);