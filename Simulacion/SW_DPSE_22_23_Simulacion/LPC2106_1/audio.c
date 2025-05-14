// =======================================================================
// Project: Videoconsole
// Date: 	2022/03/28 - 2020/02/19
// Author:  Jesï¿½s M. Hernï¿½ndez Mangas
// Subject: Desarrollo Prï¿½ctico de Sistemas Electrï¿½nicos 2022-2023
// File:    audio.c
// =======================================================================
#include "system.h"
#include "audio.h"
// ------------------------------------------------------------------------
unsigned int NOTE[] = { 0,
     DO2, DO2_, RE2, RE2_, MI2, FA2, FA2_, SOL2, SOL2_, LA2, LA2_, SI2,
	 DO3, DO3_, RE3, RE3_, MI3, FA3, FA3_, SOL3, SOL3_, LA3, LA3_, SI3,
	 DO4, DO4_, RE4, RE4_, MI4, FA4, FA4_, SOL4, SOL4_, LA4, LA4_, SI4,
	 DO5, DO5_, RE5, RE5_, MI5, FA5, FA5_, SOL5, SOL5_, LA5, LA5_, SI5,
	 DO6, DO6_, RE6, RE6_, MI6, FA6, FA6_, SOL6, SOL6_, LA6, LA6_, SI6,
	 DO7, DO7_, RE7, RE7_, MI7, FA7, FA7_, SOL7, SOL7_, LA7, LA7_, SI7,
};
// ------------------------------------------------------------------------
#define SINUS_WAVE
//#define SQUARE_WAVE
// ------------------------------------------------------------------------
// Sinus table. Angles from 0 to 2*PI.
// 256 elements. Amplitude 512 with sign
#ifdef SINUS_WAVE
const short TABLA_SINUS_WAVE[]={
    0,   12,   25,   37,   50,   62,   75,   87,
   99,  112,  124,  136,  148,  160,  172,  184,
  195,  207,  218,  230,  241,  252,  263,  273,
  284,  294,  304,  314,  324,  334,  343,  353,
  362,  370,  379,  387,  395,  403,  411,  418,
  425,  432,  439,  445,  451,  457,  462,  468,
  473,  477,  482,  486,  489,  493,  496,  499,
  502,  504,  506,  508,  509,  510,  511,  511,
  512,  511,  511,  510,  509,  508,  506,  504,
  502,  499,  496,  493,  489,  486,  482,  477,
  473,  468,  462,  457,  451,  445,  439,  432,
  425,  418,  411,  403,  395,  387,  379,  370,
  362,  353,  343,  334,  324,  314,  304,  294,
  284,  273,  263,  252,  241,  230,  218,  207,
  195,  184,  172,  160,  148,  136,  124,  112,
   99,   87,   75,   62,   50,   37,   25,   12,
    0,  -12,  -25,  -37,  -50,  -62,  -75,  -87,
  -99, -112, -124, -136, -148, -160, -172, -184,
 -195, -207, -218, -230, -241, -252, -263, -273,
 -284, -294, -304, -314, -324, -334, -343, -353,
 -362, -370, -379, -387, -395, -403, -411, -418,
 -425, -432, -439, -445, -451, -457, -462, -468,
 -473, -477, -482, -486, -489, -493, -496, -499,
 -502, -504, -506, -508, -509, -510, -511, -511,
 -512, -511, -511, -510, -509, -508, -506, -504,
 -502, -499, -496, -493, -489, -486, -482, -477,
 -473, -468, -462, -457, -451, -445, -439, -432,
 -425, -418, -411, -403, -395, -387, -379, -370,
 -362, -353, -343, -334, -324, -314, -304, -294,
 -284, -273, -263, -252, -241, -230, -218, -207,
 -195, -184, -172, -160, -148, -136, -124, -112,
  -99,  -87,  -75,  -62,  -50,  -37,  -25,  -12
};
#endif
// Square wave
#ifdef SQUARE_WAVE
const short TABLA_SQUARE_WAVE[]={
 -511, -511, -511, -511, -511, -511, -511, -511,
 -511, -511, -511, -511, -511, -511, -511, -511,
 -511, -511, -511, -511, -511, -511, -511, -511,
 -511, -511, -511, -511, -511, -511, -511, -511,
 -511, -511, -511, -511, -511, -511, -511, -511,
 -511, -511, -511, -511, -511, -511, -511, -511,
 -511, -511, -511, -511, -511, -511, -511, -511,
 -511, -511, -511, -511, -511, -511, -511, -511,
 -511, -511, -511, -511, -511, -511, -511, -511,
 -511, -511, -511, -511, -511, -511, -511, -511,
 -511, -511, -511, -511, -511, -511, -511, -511,
 -511, -511, -511, -511, -511, -511, -511, -511,
 -511, -511, -511, -511, -511, -511, -511, -511,
 -511, -511, -511, -511, -511, -511, -511, -511,
 -511, -511, -511, -511, -511, -511, -511, -511,
 -511, -511, -511, -511, -511, -511, -511, -511,
 -511, -511,  511,  511,  511,  511,  511,  511,
  511,  511,  511,  511,  511,  511,  511,  511,
  511,  511,  511,  511,  511,  511,  511,  511,
  511,  511,  511,  511,  511,  511,  511,  511,
  511,  511,  511,  511,  511,  511,  511,  511,
  511,  511,  511,  511,  511,  511,  511,  511,
  511,  511,  511,  511,  511,  511,  511,  511,
  511,  511,  511,  511,  511,  511,  511,  511,
  511,  511,  511,  511,  511,  511,  511,  511,
  511,  511,  511,  511,  511,  511,  511,  511,
  511,  511,  511,  511,  511,  511,  511,  511,
  511,  511,  511,  511,  511,  511,  511,  511,
  511,  511,  511,  511,  511,  511,  511,  511,
  511,  511,  511,  511,  511,  511,  511,  511,
  511,  511,  511,  511,  511,  511,  511,  511,
  511,  511,  511,  511,  511,  511,  511,  511
};

#endif


// ------------------------------------------------------------------------
// Volatile variables
#define TABLA1 TABLA_SINUS_WAVE

#define duration 30000

  volatile unsigned short Fase1=0;
   volatile  unsigned short Fase2=0;
   volatile  unsigned short Fase3=0;
   volatile  unsigned short Fase4=0;

   volatile unsigned short Delta_Fase1;
   volatile  unsigned short Delta_Fase2;
   volatile  unsigned short Delta_Fase3;
   volatile  unsigned short Delta_Fase4;

   volatile unsigned short nota1, nota2, nota3, nota4;

	volatile unsigned char *Part;

	volatile unsigned int Next;
	

   volatile  unsigned int silence=0;

   volatile unsigned int cont=0; //contador de la partitura
   
   volatile int defecto=0;
   
     // Duración fija del efecto en iteraciones de retardo
    

volatile unsigned short fase = 0;
volatile unsigned short delta_fase;
 volatile unsigned int exCount = 0;  
volatile unsigned char expl=0;  
volatile unsigned int effectType;
volatile unsigned short freq;
    

// ------------------------------------------------------------------------
void TIMER0_ISR(void) __attribute__ ((interrupt("IRQ")));
void PWM0_ISR(void) __attribute__ ((interrupt("IRQ")));
void IRQ_Spurious_Routine (void)  __attribute__ ((interrupt("IRQ")));
// ------------------------------------------------------------------------

//Usado para generar numeros pseudoaleatorios
unsigned int rnd(unsigned int x) {
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = (x >> 16) ^ x;
    return x;
}

void PWM0_ISR(void)
{
 // Interrupt service subroutine for changing PWM duty cycles and 
 //  generate different waveforms: channels 1-4 from partiture 
 /// and special effects channels.
 // Remember it is called 57600 times per second. Don't waste your time here.
 // Variables
  static int d1,d2,d3,d4, dtotalIzq, dtotalDch;
    PWMIR = 0xFFFF;	   // Clear all interrupt notifications
  static  int NT1=8; //256 elementos de la tabla
   
   
   
 // Get data
   d1 = TABLA1[Fase1>>NT1];
   d2 = TABLA1[Fase2>>NT1];
   d3 = TABLA1[Fase3>>NT1];
   d4 = TABLA1[Fase4>>NT1];
   

 // Attenuate
   d1=(d1>>2); //Divido entre 3
   d2=(d2>>2);
   d3=(d3>>2);
   d4=(d4>>2);
   
 // Increment phases
    Fase1+=Delta_Fase1;
    Fase2+=Delta_Fase2;
    Fase3+=Delta_Fase3;
    Fase4+=Delta_Fase4;
 
 // Special effects
   if (expl && exCount < duration){
		if (effectType == GUN_EFFECT) {
			defecto = TABLA_SINUS_WAVE[fase >> 8];  // Usar tabla de ondas sinusoidales
			defecto=(defecto>>4); // Atenuo el efecto
		}
		else if (effectType == EXPLOSION_EFFECT) {
			if (exCount % 100 == 0) {
				defecto = ((int)(rnd(defecto) & 0x3FF) - 512) >> 4;
			}
		}
		
		//_printf("\r\n defecto %d",defecto);

        // Limitar la amplitud para evitar saturación
        if (defecto > 511) defecto = 511;
        if (defecto < -511) defecto = -511;

        // Incrementar la fase
        delta_fase = (unsigned short)((256L * freq) / 225);
        fase += delta_fase;

        // Disminuir la frecuencia para simular el decaimiento de la explosión
        if (exCount % 100 == 0) {
            freq -= 10;
        }

		exCount++;
     }
     else{
		defecto = 0;
		expl=0;
     }
 // Add components (cuadraphonic sound)
    dtotalIzq=d1+d2+defecto;
    if(dtotalIzq> 511) dtotalIzq = 511; //Saturación
    if(dtotalIzq<-511) dtotalIzq = -511;
      //Canal Derecho
    dtotalDch=d3+d4+defecto;
    if(dtotalDch> 511) dtotalDch = 511; //Saturación
    if(dtotalDch<-511) dtotalDch = -511;

 // Silence?
   /*
   if(silence||(CH1_ON+CH2_ON==0))
	  dtotalIzq=511;
   if(silence||(CH3_ON+CH4_ON==0))
	 dtotalDch=511;
   */
    
   
   
 // PMW output
   PWMMR4 = dtotalIzq +512; //Centrado

   //PWM2
   PWMMR2 = dtotalDch + 512;//Centrado

 
 PWMLER = (1<<4) | (1 << 2);


 VICVectAddr=-1;  	   // EOI for interrupt controller
}
// ------------------------------------------------------------------------
void IRQ_Spurious_Routine (void) {
 _printf("\r\n--- IRQ, Spurious interrupt\r\n");
 VICVectAddr=-1;  // End of interrupt: update priority logic
}
// ------------------------------------------------------------------------
void TIMER0_ISR(void)
{
 // Interrupt service routine for changing the partitures' notes
 // It is called each 128 ms (duration of the sixteenth (semicorchea) note)
 // It depends on tempo
 T0IR = 0xFFFF;	   	   // Clear all interrupt notifications	
 // Next note, if any, update delta phases
   nota1=((Part[cont*4+0]) & 0b00111111);
   Delta_Fase1= (unsigned int)((256L*NOTE[nota1])/22500); //coger solo los bits del 5:0
   
   
   nota2=((Part[cont*4+1]) & 0b00111111);
   Delta_Fase2= (unsigned int)((256L*NOTE[nota2])/22500);
   
   
   nota3=((Part[cont*4+2]) & 0b00111111);
   Delta_Fase3= (unsigned int)((256L*NOTE[nota3])/22500);
   
   
   nota4=((Part[cont*4+3]) & 0b00111111);
   Delta_Fase4= (unsigned int)((256L*NOTE[nota4])/22500);
   
   
   cont++;
   if(cont==Next)	cont=0; //Para que se repita cuando acabe
   

 VICVectAddr=-1;  	   // EOI for interrupt controller	
}
// ------------------------------------------------------------------------
void AUDIO_Timer_On()
{
 // Interrupt frequency will be 57600 Hz
 #define PCPWM0 (5)
 PCONP |= (1<<PCPWM0);      // Power PWM peripheral

 PINSEL0 &=~(0b11<<16);
 PINSEL0 |= (0b10<<16);     // PWM4/P0.8 as PWM

// PWM2 configuration 
 PINSEL0 &=~(0b11<<14);
 PINSEL0 |= (0b10<<14); 

 PWMTCR = 0b00000010;       // Reset & PWM mode enabled
 PWMPR  = 0;                // Prescale = 1/1
 PWMMCR = 3;                // Interrupt and Reset on match (channel #0)
 PWMPCR = (1<<12) | (1<<10) ;		    // PWM4 single edge  and output enabled
	//PWM2 single edge and output enabled

 PWMMR0 = 1024-1;			// Interrupt frequency 57600 Hz. Exact 10-bit PWM
 PWMMR4 = PWMMR0;
 PWMMR2 = PWMMR0;
 //PWMLER = (1<< 4) | (1<< 2); 			// Copy to duty reg from shadow PWM4
// PWM2 configuration mising



 PWMTCR = 0b00001001;       // Counter enable and PWM mode enabled

 #define PWM0_INT (8)
 // Interrupt setup
 VICIntEnable=(1<<PWM0_INT);                  		// PWM0 interrupt enable
 VICDefVectAddr=(unsigned int)IRQ_Spurious_Routine; // unvectored IRQs
 VICVectAddr0=(unsigned int)PWM0_ISR;               // Vector for slot 0
 VICVectCntl0=(1<<5)|PWM0_INT;                      // Slot 0 for PWM0 and enabled
 // Enable both IRQ interrupts
 asm volatile ("mrs r0,cpsr\n bic r0,r0,#0x80\n msr cpsr,r0");
 asm volatile ("mrs r0,cpsr\n orr r0,r0,#0x80\n msr cpsr,r0");
}
// ------------------------------------------------------------------------
void PARTITURE_On(unsigned char *P, unsigned int N, unsigned int tempo)
{
Part=P;
Next=N;
 int ms_negra = 60000/tempo;
 int ms_semicorchea = ms_negra/4; 
 #define PCTIM0 (1)
 PCONP |= (1<<PCTIM0);      // Power TIMER0 peripheral 	
 T0TCR = 0b00000010;		// Reset	 
 T0CTCR= 0b00000000;		// Timer mode
 T0PR  = 0;					// Prescale 1:1
 T0PC  = 0;
 T0MR0 = (PCLK/1000)*ms_semicorchea - 1; // SEMICORCHEA ms  ->aprox. 128 ms
 T0MCR = 3;					// Interrupt and reset counter
 T0TCR = 0b00000001;		// Counter enable
 #define TIMER_INT (4)
 // Interrupt setup
 VICIntEnable=(1<<TIMER_INT);                  		// TIMER0 interrupt enable
 VICDefVectAddr=(unsigned int)IRQ_Spurious_Routine; // unvectored IRQs
 VICVectAddr1=(unsigned int)TIMER0_ISR;             // Vector for slot 1
 VICVectCntl1=(1<<5)|TIMER_INT;                     // Slot 1 for TIMER0 and enabled

 // Prepare audio 
 
   
 // Instruments asignment
 
 //
 AUDIO_Timer_On();

 // Enable both IRQ interrupts
 asm volatile ("mrs r0,cpsr\n bic r0,r0,#0x80\n msr cpsr,r0");	
}
// ------------------------------------------------------------------------
void PARTITURE_Off()
{ 

 T0TCR = 0; // Deactivate timer 0
}
// ------------------------------------------------------------------------
void AUDIO_Effect(int type)
{ 
   expl=1;
   effectType = type;
   exCount=0;
   fase = 0;
   freq = duration / 100 * 10 + 10;
}