/*
 * By Satyajeet Das
 * Dino Game for PICO
 */
/*
*Level 1: SPEED = 4 & VARY = 0.00005
*Level 2: SPEED = 5 & VARY = 0.0001
*Level 3: SPEED = 6 & VARY = 0.0003
*/

#include <stdio.h> //The standard C library
#include <stdlib.h> //The standard C library
#include "hardware/pio.h" //The hardware PIO library
#include "hardware/dma.h" //The hardware DMA library
#include <math.h> //The standard math library
#include "pico/stdlib.h" //Standard library for Pico
#include "pico/time.h" //The pico time library
#include "pico/types.h" //The pico types library
#include <time.h> // time c library
#include "hardware/gpio.h" //The hardware GPIO library
#include "hardware/irq.h"  // interrupts
#include "hardware/pwm.h"  // pwm 
#include "hardware/sync.h" // wait for interrupt 
#include "pico/stdlib.h" // pico library
#include "hardware/pll.h" // hardware library
#include "hardware/clocks.h"// hardware library
#include "hardware/structs/pll.h"// hardware library
#include "hardware/structs/clocks.h"// hardware library
#include "pwmsnd.h"
const uint8_t* CurSound = NULL;// current playing sound

volatile int SoundCnt = 0; // counter of current playing sound (<=0 no sound)
int SoundInc = SNDINT; // pointer increment
volatile int SoundAcc = 0; // pointer accumulator
const uint8_t* NextSound = NULL;
  // next sound to play repeated sound
int NextSoundCnt = 0; // counter of next sound (0=no repeated sound)

// PWM sound interrupt service
void PWMSndIrq()
{
	// clear interrupt request
	pwm_clear_irq(PWMSND_SLICE);

	// default sample if no sound		
	uint8_t samp = 128;

	// check if sound is playing
	int cnt = SoundCnt;
	if (cnt > 0)
	{
		// get next sample
		const uint8_t* snd = CurSound;
		samp = *snd;

		// increment pointer accumulator
		int acc = SoundAcc + SoundInc;
		int i = acc >> SNDFRAC; // whole increment
		snd += i;
		cnt -= i;
		acc &= (SNDINT-1);

		// repeated sample
		if (cnt <= 0)
		{
			cnt = NextSoundCnt;
			snd = NextSound;
		}

		// save new pointer
		SoundCnt = cnt;
		SoundAcc = acc;
		CurSound = snd;
	}

	// write PWM sample
	((uint16_t*)&pwm_hw->slice[PWMSND_SLICE].cc)[PWMSND_CHAN] = samp;
}

// initialize PWM sound output
//  GP19 ... MOSI + sound output (PWM1 B)
void PWMSndInit()
{
	// set GPIO function to PWM
	gpio_set_function(PWMSND_GPIO, GPIO_FUNC_PWM);

	// set IRQ handler
	SoundCnt = 0;
	pwm_clear_irq(PWMSND_SLICE);
	pwm_set_irq_enabled(PWMSND_SLICE, true);
	irq_set_exclusive_handler(PWM_IRQ_WRAP, PWMSndIrq);
	irq_set_enabled(PWM_IRQ_WRAP, true);

	// get PWM default config
	pwm_config cfg = pwm_get_default_config();

	// set clock divider (INT = 0..255, FRAC = 1/16..15/16)
	//  125 MHz: 125000000/5644800 = 22.144, INT=22, FRAC=2,
	//     real sample rate = 125000000/(22+2/16)/256 = 22069Hz
	pwm_config_set_clkdiv(&cfg, (float)clock_get_hz(clk_sys)/PWMSND_CLOCK + 0.03); // 0.03f = rounding 0.5/16

	// set period to 256 cycles
	pwm_config_set_wrap(&cfg, PWMSND_TOP);
	printf("inside PWM");
	// start PWM
	pwm_init(PWMSND_SLICE, &cfg, true);
}

// output PWM sound (sound must be PCM 8-bit mono 22050Hz)
//  snd = pointer to sound
//  len = length of sound in number of samples
//  speed = relative speed (1=normal)
//  rep = True to repeat sample
void PlaySound(const uint8_t* snd, int len)
{	printf("len %d", len);
	// stop current sound
	bool rep = false;
	float speed = 1.0;
	__dmb();
	SoundCnt = 0;
	__dmb();

	// repeated sound
	NextSoundCnt = 0;
	NextSoundCnt = 0;
	if (rep)
	{
		NextSound = snd;
		NextSoundCnt = len;
	}
	
	// sound speed
	SoundInc = (int)(SNDINT*1.0 + 0.5);
	SoundAcc = 0;

	// start current sound
	CurSound = snd;
	__dmb();
	SoundCnt = len;
	__dmb();
	printf("inside PWM Playing Sound");
}

// stop playing sound
void StopSound()
{
	__dmb();
	SoundCnt = 0;
	__dmb();
}

// update sound speed (1=normal speed)
void SpeedSound(float speed)
{
	SoundInc = (int)(SNDINT*speed + 0.5f);
}

// check if playing sound
bool PlayingSound()
{
	return SoundCnt > 0;
}

// set next repeated sound
void SetNextSound(const uint8_t* snd, int len)
{
	// check if this sound is already next sound
	if (PlayingSound() && (NextSound == snd) && (NextSoundCnt == len)) return;

	// disable next sound
	NextSoundCnt = 0;
	__dmb();

	// start sound if not playing
	if (SoundCnt == 0)
	{
		CurSound = snd;
		__dmb();
		SoundCnt = len;
		__dmb();
	}

	// set next sound
	NextSound = snd;
	__dmb();
	NextSoundCnt = len;
}
