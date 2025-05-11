// ----------------------------------------------------------------------------------
// Project 
// Date: 	2023/03/21
// Author: Jesús M. Hernández Mangas
// Subject: Desarrollo Práctico de Sistemas Electrónicos 2022-2023
// File: vibrador.c
// ----------------------------------------------------------------------------------
#include "system.h"
void TIMER1_ISR(void) __attribute__ ((interrupt("IRQ")));
// ----------------------------------------------------------------------------------
void TIMER1_ISR(void)
{
 T1IR = 0xFFFF;	   	   // Clear all interrupt notifications	
 //  Para el temporizador 1
 // ...

 // Para el vibrador
 // ...
 
 VICVectAddr=-1;  	   // EOI for interrupt controller	
}
// ----------------------------------------------------------------------------------
void VibratorON(unsigned int ms)
{	
 int d;
 #define PCTIM1 (2)
 PCONP |= (1<<PCTIM1);      // Power TIMER1 peripheral 	
 // Configura el temporizador 1 para que genere interrupción a los 'ms' milisegundos
 // ...


 #define TIMER1_INT (5)
 // Interrupt setup
 VICIntEnable=(1<<TIMER1_INT);                 		// TIMER1 interrupt enable
 VICVectAddr2=(unsigned int)TIMER1_ISR;             // Vector for slot 2
 VICVectCntl2=(1<<5)|TIMER1_INT;                    // Slot 2 for TIMER1 and enabled
 // Activa el vibrador 
 // ...
 
 // Enable both IRQ interrupts
 asm volatile (
  " mrs r0,cpsr\n"
  " bic r0,r0,#0x80\n"
  " msr cpsr,r0\n"
 );
}
// ----------------------------------------------------------------------------------
