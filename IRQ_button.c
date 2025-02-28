#include "button.h"
#include "LPC17xx.h"
#include "../timer/timer.h"
#include "../RIT.h"

extern int INT0;
extern int KEY1;
extern int KEY2;

void EINT0_IRQHandler (void)	  	/* INT0	ISR ROUTINE	*/
{
	/*
	------- IMPLEMENTA QUI ISR PER INT0
	
	*/
	
	/*
	----- SE DA GESTIRE TRAMITE RIT tolgi i commenti qui sotto, e implementa tutto nell'handler del RIT:
	*/
	
	NVIC_DisableIRQ(EINT0_IRQn); // --> Scollego dall'IC
	LPC_PINCON->PINSEL4 &= ~(1 << 20);  // --> Disattivo la possibilit? di scatenare Interrupt
	INT0=1; // --> Aggiorno a 1 il valore della variabile globale
	
	LPC_SC->EXTINT &= (1 << 0);     /* 			clear pending interrupt        */
}


void EINT1_IRQHandler (void)	  	/* KEY1	ISR ROUTINE */
{
  	/*
	------- IMPLEMENTA QUI ISR PER KEY1
	*/
	
	/*
	----- SE DA GESTIRE TRAMITE RIT tolgi i commenti qui sotto, e implementa tutto nell'handler del RIT:
	*/
	
	// NVIC_DisableIRQHandler(EINT1_IRQn); // --> Scollego dall'IC
	// LPC_PINCON->PINSEL4 &= ~(1 << 22);  // --> Disattivo la possibilit? di scatenare Interrupt
	// KEY1=1; // --> Aggiorno a 1 il valore della variabile globale
	
	LPC_SC->EXTINT &= (1 << 1);     /* clear pending interrupt         */
}

void EINT2_IRQHandler (void)	  	/* KEY2	ISR ROUTINE */
{
		/*
	------- IMPLEMENTA QUI ISR PER KEY2
	*/
	
	/*
	----- SE DA GESTIRE TRAMITE RIT tolgi i commenti qui sotto, e implementa tutto nell'handler del RIT:
	*/
	
	// NVIC_DisableIRQHandler(EINT2_IRQn); // --> Scollego dall'IC
	// LPC_PINCON->PINSEL4 &= ~(1 << 24);  // --> Disattivo la possibilit? di scatenare Interrupt
	// KEY2=1; // --> Aggiorno a 1 il valore della variabile globale
	
  LPC_SC->EXTINT &= (1 << 2);     /* clear pending interrupt         */   
}
