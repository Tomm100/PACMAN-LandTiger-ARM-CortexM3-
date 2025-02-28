/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_RIT.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    RIT.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "LPC17xx.h"

#include "RIT.h"
#include "../sample.h"
#include "../timer/timer.h"
#include "./GLCD.h"
#include "./music.h"
#define UPTICKS 1

extern pacMan p;
NOTE song[] = {
    // 1 (Intro - first 5 seconds of Pac-Man theme, further lowered octave)
    {NOTE_B2, time_16th},
    {NOTE_B3, time_16th},
    {NOTE_FS3, time_16th},
    {NOTE_DS3, time_16th},
    {NOTE_B3, time_16th},
    {NOTE_FS3, time_16th},
    {NOTE_DS3, time_16th},
    {NOTE_C3, time_16th},

    // 2
    {NOTE_C4, time_16th},
    {NOTE_G3, time_16th},
    {NOTE_E3, time_16th},
    {NOTE_C4, time_16th},
    {NOTE_G3, time_16th},
    {NOTE_E3, time_16th},
    {NOTE_B3, time_16th},
    {NOTE_FS3, time_16th},

    // 3
    {NOTE_D3, time_16th},
    {NOTE_B3, time_16th},
    {NOTE_FS3, time_16th},
    {NOTE_D3, time_16th},
    {NOTE_B3, time_16th},
    {NOTE_FS3, time_16th},
    {NOTE_D3, time_16th},
    {NOTE_G3, time_16th},

    // 4
    {NOTE_B2, time_16th},
    {NOTE_C3, time_16th},
    {NOTE_D3, time_16th},
    {NOTE_E3, time_16th},
    {NOTE_FS3, time_16th},
    {NOTE_G3, time_16th},
    /*
		{NOTE_A3, time_16th},
    {NOTE_B3, time_16th},

    // 5
		
    {NOTE_C4, time_16th},
    {NOTE_D4, time_16th},
    {NOTE_E4, time_16th},
    {NOTE_FS4, time_16th},
    {NOTE_G4, time_16th},
    {NOTE_A4, time_16th},
    {NOTE_B4, time_16th},
    */
		{REST, time_16th},
		
};

NOTE gameOverSong[] = {
    // Prima parte: Salita drammatica
    {NOTE_C3, time_16th},    // Do
    {NOTE_E3, time_16th},    // Mi
    {NOTE_G3, time_16th},    // Sol
    {NOTE_C4, time_16th},    // Do (ottava superiore)

    // Discesa
    {NOTE_G3, time_16th},    // Sol
    {NOTE_E3, time_16th},    // Mi
    {NOTE_C3, time_16th},    // Do
    {NOTE_G2, time_16th},    // Sol (ottava inferiore)

    // Seconda parte: Effetto rapido di "crollo"
    {NOTE_E3, time_32nd},    // Mi
    {NOTE_DS3, time_32nd},   // Mi bemolle
    {NOTE_D3, time_32nd},    // Re
    {NOTE_C3, time_16th},    // Do

    // Conclusione: Nota finale e pausa
    {NOTE_C2, time_8th},     // Do grave
    {REST, time_8th}         // Pausa
};
/******************************************************************************
** Function name:		RIT_IRQHandler
**
** Descriptions:		REPETITIVE INTERRUPT TIMER handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
/* OCCORE DICHIARARLE GLOBALI IN QUANTO CONDIVISE CON I BUTTON
*/
volatile int INT0=0;
volatile int KEY1=0; 
volatile int KEY2=0;
volatile int posizione=0;	// 1->su , 2-> dx, 3-> giu, 4->sx
volatile int pausa = 1; // 0-> non in pausa, 1 in pausa
volatile int startGame = 1; //1 suono la musica poi setto a 0
volatile int gameOver = 0;


void RIT_IRQHandler (void)
{					
	
	
	// VARIABILI DEL JOYSTICK
	
	static int up=0;
	static int down=0;
	static int left=0;
	static int right=0;
	
	
	
  if(startGame==1){
		//per la musica uso timer1 e timer3
	  static int currentNote = 0;
		static int ticks = 0;
		if(!isNotePlaying())
		{
			++ticks;
			if(ticks == UPTICKS)
			{
				ticks = 0;
				playNote(song[currentNote++]);
			}
		}
		
		if(currentNote == (sizeof(song) / sizeof(song[0])))
		{
			
			startGame=0;
			disable_timer(3);
			disable_timer(1);
			reset_timer(1);
			
			init_timer(1,0x23C3460);     // 1s (0x17D7840)->timer per countdown  1,5s (0x23C3460)
			enable_timer(1);
			
			init_timer(0,0x4C4B40);   	 //200ms sec (muove pacman)
	    enable_timer(0);
			init_timer(2,0xBEBC20);      		// per il ghost (500ms) 
	    enable_timer(2);
		}
	}
	
	if (gameOver==1){
		static int currentNote = 0;
		static int ticks = 0;
		if(!isNotePlaying())
		{
			++ticks;
			if(ticks == UPTICKS)
			{
				ticks = 0;
				playNote(gameOverSong[currentNote++]);
			}
		}
		if(currentNote == (sizeof(gameOverSong) / sizeof(gameOverSong[0])))
		{
		
			disable_RIT();
		
		}
	
	
	}
	
	
		
	
	
	/* INT0 management 
	*/
	if(INT0>=1){ 
		if((LPC_GPIO2->FIOPIN & (1<<10)) == 0){	/* INT0 pressed */
			switch(INT0){				
				case 2:				/* pay attention here: please see slides 19_ to understand value 2 */
				
				/* IMPLEMENTA QUI COSA FARE */
				if(pausa==0){
					//dobbiamo mettere in pausa
					pausa = 1;
					
					}
				else{
						//usciamo dalla pausa
						
					
					if(startGame==1){
					
					disable_timer(3);
					disable_timer(1);
					reset_timer(1);
					
					init_timer(1,0x23C3460);     // 1s (0x17D7840)->timer per countdown  1,5s (0x23C3460)
					enable_timer(1);
					
					init_timer(0,0x4C4B40);   	 //200ms sec (muove pacman)
					enable_timer(0);
					init_timer(2,0xBEBC20);      		// per il ghost (500ms) 
					enable_timer(2);
					startGame=0;
					}
					pausa=0;
						
				}
			
				
				default:
					break;
			}
			INT0++;
		}
		else {	/* button released */
			INT0=0;			
			NVIC_EnableIRQ(EINT0_IRQn);							 /* enable Button interrupts			*/
			LPC_PINCON->PINSEL4    |= (1 << 20);     /* External interrupt 0 pin selection */
		}
	}

	
	
	
	if(pausa==0){
	/* UP management
	*/
	if((LPC_GPIO1->FIOPIN & (1<<29)) == 0){	
		/* Joytick UP pressed */
		up++;
				
		switch(up){
			case 1:
				/*
				INSERISCI QUI LA TUA FUNZIONE
				*/
				posizione = 1;
				break;
			case 60:	//3sec = 3000ms/50ms = 60 (Valore modificabile)
				/*
				INSERISCI QUI LA TUA FUNZIONE
				*/
				break;
			default:
				break;
		}
	}
	else{
			up=0;
	}
	
	
		
	/* DOWN management
	*/
	if((LPC_GPIO1->FIOPIN & (1<<26)) == 0){	
		/* Joytick DOWN pressed */
		down++;
		
		switch(down){
			case 1:
				/*
				INSERISCI QUI LA TUA FUNZIONE
				*/
				posizione = 3;
			
			
			
				
				break;
			case 60:	//3sec = 3000ms/50ms = 60 (Valore modificabile)
				/*
				INSERISCI QUI LA TUA FUNZIONE
				*/
				break;
			default:
				break;
		}
	}
	else{
			down=0;
	}
	
	
	
	
	/* RIGHT management
	*/
	if((LPC_GPIO1->FIOPIN & (1<<28)) == 0){	
		/* Joytick RIGHT pressed */
		right++;
		
		switch(right){
			case 1:{
				/* INSERISCI QUI LA TUA FUNZIONE*/
			
				
				posizione = 2;
				
				
				
				break;
			}
			case 60:	//3sec = 3000ms/50ms = 60 (Valore modificabile)
				/*
				INSERISCI QUI LA TUA FUNZIONE
				*/
				break;
			default:
				break;
		}
	}
	else{
			right=0;
	}
	
	
	
	
	
	/* LEFT management
	*/
	if((LPC_GPIO1->FIOPIN & (1<<27)) == 0){	
		/* Joytick LEFT pressed */
		left++;
		
		switch(left){
			case 1:
				/*
				INSERISCI QUI LA TUA FUNZIONE
				*/
			
				
				posizione = 4;
				
			
			
			
				break;
			case 60:	//3sec = 3000ms/50ms = 60 (Valore modificabile)
				/*
				INSERISCI QUI LA TUA FUNZIONE
				*/
				break;
			default:
				break;
		}
	}
	else{
			left=0;
	}
	
}

	
	if(pausa ==0 && gameOver==0){
	
			send_message_CAN();
	
	}
	
	
	
  LPC_RIT->RICTRL |= 0x1;	/* clear interrupt flag */
	
  return;
}
	

/******************************************************************************
**                            End Of File
******************************************************************************/
