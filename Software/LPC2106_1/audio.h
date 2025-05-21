#ifndef _AUDIO_H_
#define _AUDIO_H_

#define GUN_EFFECT 0
#define EXPLOSION_EFFECT 1

// Musical note frequencies x 100. Character _ means sustained note (# sostenido)
#define DO2     6541L
#define DO2_    6930L
#define RE2     7342L
#define RE2_    7778L
#define MI2     8241L
#define FA2     8731L
#define FA2_    9250L
#define SOL2    9800L
#define SOL2_  10383L
#define LA2    11000L
#define LA2_   11654L
#define SI2    12347L

#define DO3    13081L
#define DO3_   13859L
#define RE3    14683L
#define RE3_   15556L
#define MI3    16481L
#define FA3    17461L
#define FA3_   18500L
#define SOL3   19600L
#define SOL3_  20765L
#define LA3    22000L
#define LA3_   23308L
#define SI3    24694L

#define DO4    26163L
#define DO4_   27718L
#define RE4    29366L
#define RE4_   31113L
#define MI4    32963L
#define FA4    34923L
#define FA4_   36999L
#define SOL4   39200L
#define SOL4_  41530L
#define LA4    44000L
#define LA4_   46600L
#define SI4    49388L

#define DO5    52325L
#define DO5_   55437L
#define RE5    58733L
#define RE5_   62225L
#define MI5    65926L
#define FA5    69846L
#define FA5_   73999L
#define SOL5   78399L
#define SOL5_  83061L
#define LA5    88000L
#define LA5_   93233L
#define SI5    98777L

#define DO6   104650L
#define DO6_  110873L
#define RE6   117466L
#define RE6_  124451L
#define MI6   131851L
#define FA6   139691L
#define FA6_  147998L
#define SOL6  156798L
#define SOL6_ 166122L
#define LA6   176000L
#define LA6_  186466L
#define SI6   197553L

#define DO7   209300L
#define DO7_  221746L
#define RE7   234932L
#define RE7_  248902L
#define MI7   263702L
#define FA7   279383L
#define FA7_  295996L
#define SOL7  313596L
#define SOL7_ 332244L
#define LA7   352000L
#define LA7_  372931L
#define SI7   395107L

// ------------------------------------------------------------------------ 
// Musical notes durations (ms)
// 112.5 NEGRA/min from partiture -> NEGRA = 533.3 ms
#define BLANCA      (2*NEGRA)
#define NEGRA       (533)
#define CORCHEA     (NEGRA/2)
#define SEMICORCHEA (CORCHEA/2)
#define FUSA        (SEMICORCHEA/2)
#define SEMIFUSA    (FUSA/2)
// ------------------------------------------------------------------------


// ------------------------------------------------------------------------
// Musical notes english notation
enum notes { 
	C2=1,C2_,D2,D2_,E2,F2,F2_,G2,G2_,A2,A2_,B2, 
	C3  ,C3_,D3,D3_,E3,F3,F3_,G3,G3_,A3,A3_,B3, 
	C4  ,C4_,D4,D4_,E4,F4,F4_,G4,G4_,A4,A4_,B4,
	C5  ,C5_,D5,D5_,E5,F5,F5_,G5,G5_,A5,A5_,B5,
	C6  ,C6_,D6,D6_,E6,F6,F6_,G6,G6_,A6,A6_,B6,
	C7  ,C7_,D7,D7_,E7,F7,F7_,G7,G7_,A7,A7_,B7 
};

extern volatile int CH1_ON,CH2_ON,CH3_ON,CH4_ON;
extern volatile int iPP, rPP;
extern unsigned char PARTITURE[131][4];
extern unsigned char PARTITURE_galaga_intro_txt[][4] ;



void AUDIO_Effect(int type);
void PARTITURE_On(unsigned char *P, unsigned int N, unsigned int tempo);
void PARTITURE_Off();
#endif // _AUDIO_H_