//#define TEST_NO_FREQ_HOP	// Compile No Frequency Hopping

// Oscillator
#define TEST
// System Definitions
#define SAMPLE_RATE					8		// Sampling frequency in kHz.

// Packet Definitions
#define AUDIO_SAMPLES				30
#define AUDIO_BUFFER_LEVEL			40

#define DAC_SAMPLES  				27

#define F_CPU   				72000
#define  PCLK1DIV 			2
#define TIM2_Pre				2
#define TIM3_Pre				5
#define TIM4_Pre				5
#define F_TIM2				(F_CPU/(TIM2_Pre+1))		// Frequency in kHz
#define F_TIM3				(F_CPU/(TIM3_Pre+1))		      // Frequency in kHz
#define F_TIM4				(F_CPU/(TIM4_Pre+1))		      // Frequency in kHz

#define TIM2_CCR2_Val                     ((F_TIM2/(SAMPLE_RATE*2)))
//#define FRAME_PERIOD			  ((F_TIM3/SAMPLE_RATE)*AUDIO_SAMPLES-1) // 0xafc7
#define FRAME_PERIOD			  ((F_TIM3/SAMPLE_RATE)*DAC_SAMPLES-1) // 0xafc7

#define	SERVER_ID		0x3A
#define	CLIENT_ID11	0x11
#define	CLIENT_ID22	0x22
#define	CLIENT_ID33	0x33
#define	CLIENT_ID44	0x44

