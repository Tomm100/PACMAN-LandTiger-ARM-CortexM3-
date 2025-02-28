#include "music.h"

#include "../timer/timer.h"
#include "LPC17xx.h"

void playNote(NOTE note)
{
	if(note.freq != REST)
	{
		reset_timer(1);
		init_timer(1, note.freq*AMPLIFIER*VOLUME);
		enable_timer(1);
	}
	reset_timer(3);
	init_timer(3, note.duration);
	enable_timer(3);
}

BOOL isNotePlaying(void)
{
	return ((LPC_TIM1->TCR != 0) || (LPC_TIM3->TCR != 0));
}
