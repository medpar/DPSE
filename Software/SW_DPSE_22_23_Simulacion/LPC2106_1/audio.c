// =======================================================================
// Project: Videoconsole
// Date: 	2022/03/28 - 2020/02/19 (Updated 2023-10-27)
// Author:  Jesús M. Hernández Mangas (Modified by AI)
// Subject: Desarrollo Práctico de Sistemas Electrónicos 2022-2023
// File:    audio.c
// Description: Audio generation using PWM and Timer interrupts for LPC2106
//              Plays a 4-channel partiture.
// =======================================================================
#include "system.h" // Should define PCLK
#include "audio.h"
#include "lpc2106.h"

// --- Configuration Constants ---
#define PWM_FREQUENCY 57600UL // Target PWM interrupt frequency (Hz)
#define AUDIO_CHANNELS 4      // Number of audio channels from partiture
#define WAVE_TABLE_SIZE 256   // Number of samples in the waveform table
#define PHASE_ACCUMULATOR_BITS 32 // Size of the phase accumulators
#define VOLUME_MAX 255        // Max value for channel volume (0-255)

// --- Peripheral Configuration Defines ---
#define PCTIM0 (1) // PCONP bit for TIMER0
#define PCPWM0 (5) // PCONP bit for PWM0

#define TIMER_INT (4) // VIC Timer 0 Interrupt Line
#define PWM0_INT (8)  // VIC PWM0 Interrupt Line (covers all PWM channels)

// --- Note Frequency Lookup Table ---
// Maps note enum values (1-84) to frequencies x 100
// Index 0 is unused (represents silence in partiture)
unsigned int NOTE[] = { 0,
     DO2, DO2_, RE2, RE2_, MI2, FA2, FA2_, SOL2, SOL2_, LA2, LA2_, SI2, // 1-12
	 DO3, DO3_, RE3, RE3_, MI3, FA3, FA3_, SOL3, SOL3_, LA3, LA3_, SI3, // 13-24
	 DO4, DO4_, RE4, RE4_, MI4, FA4, FA4_, SOL4, SOL4_, LA4, LA4_, SI4, // 25-36
	 DO5, DO5_, RE5, RE5_, MI5, FA5, FA5_, SOL5, SOL5_, LA5, LA5_, SI5, // 37-48
	 DO6, DO6_, RE6, RE6_, MI6, FA6, FA6_, SOL6, SOL6_, LA6, LA6_, SI6, // 49-60
	 DO7, DO7_, RE7, RE7_, MI7, FA7, FA7_, SOL7, SOL7_, LA7, LA7_, SI7, // 61-72
     // Add more notes if needed, ensure enum notes and NOTE array match
     // Padding potentially needed if partiture uses higher notes
     0,0,0,0,0,0,0,0,0,0,0,0 // Dummy entries up to 84 if needed
};
#define MAX_NOTE_INDEX (sizeof(NOTE)/sizeof(NOTE[0]) - 1)

// --- Waveform Selection ---
//#define SINUS_WAVE
#define SQUARE_WAVE
// ------------------------------------------------------------------------
// Waveform Tables (256 samples, Amplitude +/- 511)
#ifdef SINUS_WAVE
const short TABLA_SINUS_WAVE[WAVE_TABLE_SIZE]={
    0,   12,   25,   37,   50,   62,   75,   87,
   99,  112,  124,  136,  148,  160,  172,  184,
  195,  207,  218,  230,  241,  252,  263,  273,
  284,  294,  304,  314,  324,  334,  343,  353,
  362,  370,  379,  387,  395,  403,  411,  418,
  425,  432,  439,  445,  451,  457,  462,  468,
  473,  477,  482,  486,  489,  493,  496,  499,
  502,  504,  506,  508,  509,  510,  511,  511,
  512,  511,  511,  510,  509,  508,  506,  504, // Adjusted max to 511 for symmetry
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
 -512, -511, -511, -510, -509, -508, -506, -504, // Adjusted min to -511
 -502, -499, -496, -493, -489, -486, -482, -477,
 -473, -468, -462, -457, -451, -445, -439, -432,
 -425, -418, -411, -403, -395, -387, -379, -370,
 -362, -353, -343, -334, -324, -314, -304, -294,
 -284, -273, -263, -252, -241, -230, -218, -207,
 -195, -184, -172, -160, -148, -136, -124, -112,
  -99,  -87,  -75,  -62,  -50,  -37,  -25,  -12
};
#endif

#ifdef SQUARE_WAVE
const short TABLA_SQUARE_WAVE[WAVE_TABLE_SIZE]={
 // First half: -511
 -511, -511, -511, -511, -511, -511, -511, -511, -511, -511, -511, -511, -511, -511, -511, -511,
 -511, -511, -511, -511, -511, -511, -511, -511, -511, -511, -511, -511, -511, -511, -511, -511,
 -511, -511, -511, -511, -511, -511, -511, -511, -511, -511, -511, -511, -511, -511, -511, -511,
 -511, -511, -511, -511, -511, -511, -511, -511, -511, -511, -511, -511, -511, -511, -511, -511,
 -511, -511, -511, -511, -511, -511, -511, -511, -511, -511, -511, -511, -511, -511, -511, -511,
 -511, -511, -511, -511, -511, -511, -511, -511, -511, -511, -511, -511, -511, -511, -511, -511,
 -511, -511, -511, -511, -511, -511, -511, -511, -511, -511, -511, -511, -511, -511, -511, -511,
 -511, -511, -511, -511, -511, -511, -511, -511, -511, -511, -511, -511, -511, -511, -511, -511,
 // Second half: +511
  511,  511,  511,  511,  511,  511,  511,  511,  511,  511,  511,  511,  511,  511,  511,  511,
  511,  511,  511,  511,  511,  511,  511,  511,  511,  511,  511,  511,  511,  511,  511,  511,
  511,  511,  511,  511,  511,  511,  511,  511,  511,  511,  511,  511,  511,  511,  511,  511,
  511,  511,  511,  511,  511,  511,  511,  511,  511,  511,  511,  511,  511,  511,  511,  511,
  511,  511,  511,  511,  511,  511,  511,  511,  511,  511,  511,  511,  511,  511,  511,  511,
  511,  511,  511,  511,  511,  511,  511,  511,  511,  511,  511,  511,  511,  511,  511,  511,
  511,  511,  511,  511,  511,  511,  511,  511,  511,  511,  511,  511,  511,  511,  511,  511,
  511,  511,  511,  511,  511,  511,  511,  511,  511,  511,  511,  511,  511,  511,  511,  511
};
#endif
// ------------------------------------------------------------------------
// --- Volatile Global Variables for Audio State ---

// Phase accumulators (32-bit for precision)
volatile unsigned int Fase1, Fase2, Fase3, Fase4;
// Phase increments (determines frequency)
volatile unsigned int Delta1, Delta2, Delta3, Delta4;
// Channel volumes (0-255)
volatile unsigned char Vol1, Vol2, Vol3, Vol4;

// Pointer to the current waveform table
const short *pWave = 0;
// Shift value to get table index from phase accumulator
// (PHASE_ACCUMULATOR_BITS - log2(WAVE_TABLE_SIZE))
unsigned int NT1 = 0;

// Partiture state (managed by PARTITURE_On/Off and TIMER0_ISR)
unsigned char *pPartitura = 0; // Pointer to the partiture array
unsigned int nPartitura = 0;  // Total number of steps in the partiture
volatile int iPP = 0;                // Current position (step) in the partiture

// --- ISR Prototypes ---
void TIMER0_ISR(void) __attribute__ ((interrupt("IRQ")));
void PWM0_ISR(void) __attribute__ ((interrupt("IRQ")));
void IRQ_Spurious_Routine (void)  __attribute__ ((interrupt("IRQ")));
// ------------------------------------------------------------------------

// ========================================================================
// PWM0_ISR
// ========================================================================
// Called at PWM_FREQUENCY (e.g., 57600 Hz).
// Generates the audio waveform sample by sample based on channel phases.
// Outputs the mixed signal to PWM4 (P0.8).
// Keep this ISR *FAST*!
// ========================================================================
void PWM0_ISR(void)
{
    int d1, d2, d3, d4; // Individual channel samples
    int dtotal;          // Mixed sample

    // 1. Get sample for each channel based on current phase and volume
    //    Index = (FaseX >> NT1) % WAVE_TABLE_SIZE;
    //    Sample = pWave[Index] * VolX / VOLUME_MAX;
    d1 = (pWave[Fase1 >> NT1] * Vol1) >> 8; // Efficient division by 256 for Vol (0-255)
    d2 = (pWave[Fase2 >> NT1] * Vol2) >> 8;
    d3 = (pWave[Fase3 >> NT1] * Vol3) >> 8;
    d4 = (pWave[Fase4 >> NT1] * Vol4) >> 8;

    // 2. Increment phase accumulators
    Fase1 += Delta1;
    Fase2 += Delta2;
    Fase3 += Delta3;
    Fase4 += Delta4;

    // 3. Mix channels (simple addition)
    dtotal = d1 + d2 + d3 + d4;

    // 4. Clamp the mixed signal to PWM range (+/- 511)
    if(dtotal > 511) dtotal = 511;
    else if(dtotal < -511) dtotal = -511;

    // 5. Output to PWM4 centered around 512 (for 0-1023 range)
    //    PWMMR0 is 1023 (defines the period)
    PWMMR4 = dtotal + 512;
    PWMLER = (1 << 4); // Latch PWMMR4 value

    // 6. Clear PWM interrupt flag (Match 0 caused this)
    PWMIR = (1 << 0); // Clear MR0 Match interrupt flag
    VICVectAddr = 0;  // Acknowledge interrupt in VIC
}

// ========================================================================
// TIMER0_ISR
// ========================================================================
// Called at the tempo rate (e.g., every semicorchea/16th note).
// Reads the next step from the partiture and updates the phase increments
// (DeltaX) for each channel, effectively changing the notes played.
// ========================================================================
void TIMER0_ISR(void)
{
    unsigned char note1, note2, note3, note4;
    unsigned int freq; // Frequency x 100

    // Check if partiture pointer is valid
    if (!pPartitura || nPartitura == 0) {
         T0IR = 0xFF;       // Clear Timer0 interrupt flags
         VICVectAddr = 0;   // Acknowledge interrupt
         return;
    }

    // --- Get notes for the current step ---
    // Partiture format: [CH1_NOTE, CH2_NOTE, CH3_NOTE, CH4_NOTE] per step
    note1 = pPartitura[iPP * AUDIO_CHANNELS + 0];
    note2 = pPartitura[iPP * AUDIO_CHANNELS + 1];
    note3 = pPartitura[iPP * AUDIO_CHANNELS + 2];
    note4 = pPartitura[iPP * AUDIO_CHANNELS + 3];

    // --- Update Delta (Phase Increment) for each channel ---
    // Delta = (Frequency * 2^PHASE_ACCUMULATOR_BITS) / PWM_FREQUENCY
    // Frequency = NOTE[note_index] / 100
    // Delta = (NOTE[note_index] / 100) * (2^32 / PWM_FREQ)
    // Delta = (NOTE[note_index] * (2^32 / (PWM_FREQ * 100)))
    // Precompute constant: K = 2^32 / (PWM_FREQ * 100)
    // K = 4294967296 / (57600 * 100) = 4294967296 / 5760000 = 745.654...
    // Use 64-bit intermediate calculation for precision:
    // Delta = (Frequency_x100 * 4294967296ULL) / 5760000ULL;

    // Channel 1
    if (note1 > 0 && note1 <= MAX_NOTE_INDEX) {
        freq = NOTE[note1];
        //Delta1 = ((unsigned long long)freq * 4294967296ULL) / (PWM_FREQUENCY * 100UL);
    } else {
        Delta1 = 0; // Silence
    }

    // Channel 2
    if (note2 > 0 && note2 <= MAX_NOTE_INDEX) {
        freq = NOTE[note2];
  //      Delta2 = ((unsigned long long)freq * 4294967296ULL) / (PWM_FREQUENCY * 100UL);
    } else {
        Delta2 = 0; // Silence
    }

    // Channel 3
    if (note3 > 0 && note3 <= MAX_NOTE_INDEX) {
        freq = NOTE[note3];
    //    Delta3 = ((unsigned long long)freq * 4294967296ULL) / (PWM_FREQUENCY * 100UL);
    } else {
        Delta3 = 0; // Silence
    }

    // Channel 4
    if (note4 > 0 && note4 <= MAX_NOTE_INDEX) {
        freq = NOTE[note4];
      //  Delta4 = ((unsigned long long)freq * 4294967296ULL) / (PWM_FREQUENCY * 100UL);
    } else {
        Delta4 = 0; // Silence
    }

    // --- Advance to the next step in the partiture ---
    iPP++;
    if (iPP >= nPartitura) {
        iPP = 0; // Loop partiture
        // Optionally: Call PARTITURE_Off() here if no looping desired
    }

    // --- Acknowledge Interrupt ---
    T0IR = 0xFF;       // Clear Timer0 interrupt flags (MR0 match + others)
    VICVectAddr = 0;   // Acknowledge interrupt in VIC
}

// ========================================================================
// IRQ_Spurious_Routine
// ========================================================================
// Handles unexpected interrupts.
// ========================================================================
void IRQ_Spurious_Routine (void) {
    // Optional: Add logging or debugging here (e.g., using _printf if available/safe)
    // _printf("\r\n--- IRQ, Spurious interrupt\r\n");
    VICVectAddr = 0;  // Acknowledge interrupt
}

// ========================================================================
// AUDIO_Configure_PWM
// ========================================================================
// Configures PWM peripheral for audio output on PWM4 (P0.8).
// Sets up PWM frequency and enables PWM interrupt.
// Called internally by PARTITURE_On.
// ========================================================================
static void AUDIO_Configure_PWM(void)
{
    // --- Calculate PWM settings ---
    // PWMMR0 determines the period (and resolution). We want 1024 steps (10-bit).
    // PWM_CLK = PCLK / (PWMPR + 1)
    // Period = (PWMMR0 + 1) * (1 / PWM_CLK)
    // PWM_Frequency = PWM_CLK / (PWMMR0 + 1)
    // We want PWM_Frequency = 57600 Hz.
    // If PWMPR = 0, PWM_CLK = PCLK.
    // Then (PWMMR0 + 1) = PCLK / PWM_Frequency
    // Assuming PCLK = 58.9824 MHz (common with 14.7456 MHz crystal * 4 PLL)
    // PWMMR0 + 1 = 58982400 / 57600 = 1024
    // So PWMMR0 = 1023. This gives 10-bit resolution.
    #ifndef PCLK
    #warning "PCLK not defined, assuming 58982400UL for PWM calculation."
    #define PCLK 58982400UL
    #endif
    unsigned int pwm_mr0 = (PCLK / PWM_FREQUENCY) - 1;
    // Use fixed 1024-1 for 10-bit resolution if PCLK allows it
    pwm_mr0 = 1024 - 1;

    // --- Enable Power for PWM ---
    PCONP |= (1 << PCPWM0);      // Power PWM peripheral

    // --- Configure Pin P0.8 as PWM4 ---
    PINSEL0 &= ~(0b11 << 16); // Clear P0.8 function bits
    PINSEL0 |= (0b10 << 16);  // Set P0.8 function to PWM4

    // --- Configure PWM ---
    PWMTCR = (1 << 1);       // Reset PWM Timer Counter and Prescaler
    PWMPR  = 0;              // Prescale = 0 => PWM clock = PCLK
    PWMMCR = (1 << 1) | (1 << 0); // Interrupt and Reset on PWMMR0 match
    PWMPCR = (1 << 12);		 // Enable PWM4 output (PWMSEL4=0 => single edge)

    PWMMR0 = pwm_mr0;		 // Set PWM cycle length (period) -> determines PWM frequency
    PWMMR4 = pwm_mr0 / 2 + 1; // Initialize PWM4 duty cycle to ~50% (silence)
    PWMLER = (1 << 4) | (1 << 0); // Latch PWMMR0 and PWMMR4 values

    PWMIR = 0xFF;            // Clear any pending PWM interrupts
    PWMTCR = (1 << 0) | (1 << 3); // Counter Enable and PWM Mode Enable

    // --- Configure VIC for PWM Interrupt ---
    VICIntSelect &= ~(1 << PWM0_INT);                   // Select PWM0 as IRQ
    VICVectAddr0 = (unsigned long)PWM0_ISR;             // Set address of PWM0 ISR
    VICVectCntl0 = (1 << 5) | PWM0_INT;                 // Enable slot 0, assign PWM0 IRQ
    VICIntEnable |= (1 << PWM0_INT);                    // Enable PWM0 interrupt in VIC
}

// ========================================================================
// PARTITURE_On
// ========================================================================
// Starts playing a multi-channel partiture.
// P: Pointer to the partiture data array.
// N: Number of steps (rows) in the partiture array.
// tempo: Tempo in beats (negra/quarter notes) per minute.
// ========================================================================
void PARTITURE_On(unsigned char* P, unsigned int N, unsigned int tempo)
{
    // --- Calculate timing for Timer0 ---
    // Tempo is Negra (quarter notes) per minute.
    // We need interval for Semicorchea (16th note).
    // ms per Negra = (60 seconds/min * 1000 ms/sec) / tempo BPM
    // ms per Semicorchea = (ms per Negra) / 4
    unsigned int ms_negra = 60000 / tempo;
    unsigned int ms_semicorchea = ms_negra / 4;
    // Timer0 Match Value = (PCLK * ms_semicorchea / 1000) - 1
    unsigned int timer_mr0 = ((PCLK / 1000) * ms_semicorchea) - 1;

    // --- Stop current playback if any ---
    PARTITURE_Off();

    // --- Store partiture info ---
    pPartitura = P;
    nPartitura = N;
    iPP = 0;          // Start from the beginning

    // --- Initialize Audio State ---
    Fase1 = Fase2 = Fase3 = Fase4 = 0;
    Delta1 = Delta2 = Delta3 = Delta4 = 0; // Start silent
    Vol1 = Vol2 = Vol3 = Vol4 = VOLUME_MAX; // Default to max volume

    // --- Select Waveform ---
    #ifdef SINUS_WAVE
        pWave = TABLA_SINUS_WAVE;
    #elif defined(SQUARE_WAVE)
        pWave = TABLA_SQUARE_WAVE;
    #else
        #error "No waveform defined (define SINUS_WAVE or SQUARE_WAVE)"
        // Or default to one: pWave = TABLA_SQUARE_WAVE;
    #endif
    // Calculate shift amount NT1 = AccBits - log2(TableSize)
    // log2(256) = 8
    NT1 = PHASE_ACCUMULATOR_BITS - 8;

    // --- Enable Power for Timer 0 ---
    PCONP |= (1 << PCTIM0);

    // --- Configure Timer 0 ---
    T0TCR = (1 << 1);       // Reset Timer Counter and Prescaler
    T0CTCR= 0b00000000;		// Timer mode
    T0PR  = 0;				// Prescaler = 0 => Timer clock = PCLK
    T0MR0 = timer_mr0;      // Set match value for semicorchea interval
    T0MCR = (1 << 1) | (1 << 0); // Interrupt and Reset on MR0 match
    T0IR  = 0xFF;           // Clear pending Timer0 interrupts
    T0TCR = (1 << 0);       // Enable Timer Counter

    // --- Configure PWM Audio Output ---
    AUDIO_Configure_PWM(); // Sets up PWM and its interrupt (Slot 0)

    // --- Configure VIC for Timer 0 Interrupt ---
    VICIntSelect &= ~(1 << TIMER_INT);                  // Select Timer0 as IRQ
    VICVectAddr1 = (unsigned long)TIMER0_ISR;           // Set address of Timer0 ISR
    VICVectCntl1 = (1 << 5) | TIMER_INT;                // Enable slot 1, assign Timer0 IRQ
    VICIntEnable |= (1 << TIMER_INT);                   // Enable Timer0 interrupt in VIC

    // --- Configure VIC Default Handler ---
    VICDefVectAddr = (unsigned long)IRQ_Spurious_Routine; // unvectored IRQs

    // --- Enable IRQ in CPU (if not already enabled) ---
    // This line clears the IRQ disable bit in CPSR
    asm volatile ("mrs r0,cpsr\n bic r0,r0,#0x80\n msr cpsr_c,r0" : : : "r0");
}

// ========================================================================
// PARTITURE_Off
// ========================================================================
// Stops the partiture playback and silences audio output.
// ========================================================================
void PARTITURE_Off()
{
    // --- Disable Timer 0 ---
    T0TCR = 0; // Disable Timer0 counter
    PCONP &= ~(1 << PCTIM0); // Power down Timer0 (optional)

    // --- Disable PWM ---
    PWMTCR = (1 << 1); // Reset PWM timer
    // Set PWM output to silence (~50% duty cycle)
    unsigned int pwm_mr0 = (PCLK / PWM_FREQUENCY) - 1; // Re-calculate or use stored value if available
    pwm_mr0 = 1024 - 1; // Assuming 10-bit
    PWMMR4 = pwm_mr0 / 2 + 1;
    PWMLER = (1 << 4);
    // Consider disabling PWM output entirely: PWMPCR &= ~(1 << 12);
    // Or power down PWM: PCONP &= ~(1 << PCPWM0); // Careful if other PWM channels used

    // --- Disable Interrupts in VIC ---
    VICIntEnClr = (1 << TIMER_INT); // Disable Timer0 interrupt
    VICIntEnClr = (1 << PWM0_INT);  // Disable PWM0 interrupt

    // --- Clear Partiture State ---
    pPartitura = 0;
    nPartitura = 0;
    iPP = 0;
    Delta1 = Delta2 = Delta3 = Delta4 = 0; // Ensure silence

    // Optional: Disable IRQ in CPU if no other interrupts needed
    // asm volatile ("mrs r0,cpsr\n orr r0,r0,#0x80\n msr cpsr_c,r0" : : : "r0");
}

// ========================================================================
// AUDIO_Effect (Placeholder)
// ========================================================================
// Play a short sound effect (e.g., on a dedicated channel or override).
// note: Note index from enum notes.
// duration: Duration in milliseconds.
// volume: Volume (0-255 or similar scale).
// Note: Implementation requires managing effect state, potentially another
// timer or using existing timers carefully. This is a basic placeholder.
// ========================================================================
void AUDIO_Effect(int note, int duration, int volume)
{
    // --- To be implemented ---
    // Example idea: Use a dedicated effect channel (if hardware supports more PWMs)
    // Or temporarily override one of the partiture channels (e.g., Channel 4)
    // Need state variables: effect_note, effect_volume, effect_remaining_ms
    // Need a way to count down duration (e.g., another timer, or count ticks in PWM/Timer ISR)

    // Placeholder: Just ignore for now
   /*
    (void)note;      // Mark as unused
    (void)duration;  // Mark as unused
    (void)volume;    // Mark as unused
   */
}