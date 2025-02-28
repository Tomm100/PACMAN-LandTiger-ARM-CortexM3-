
#include "button.h"
#include "LPC17xx.h"

/**
 * @brief  Function that initializes Buttons
 */
void BUTTON_init(void) {

  LPC_PINCON->PINSEL4    |= (1 << 20);		 /* INT0 (PIN10) configurato per scatenare Interrupt */
  LPC_GPIO2->FIODIR      &= ~(1 << 10);    /* Setto tramite GPIO la direzione in Input */

  LPC_PINCON->PINSEL4    |= (1 << 22);     /* KEY1 (PIN11) configurato per scatenare Interrupt */
  LPC_GPIO2->FIODIR      &= ~(1 << 11);    /* Setto tramite GPIO la direzione in Input */
  
  LPC_PINCON->PINSEL4    |= (1 << 24);     /* KEY2 (PIN12) configurato per scatenare Interrupt */
  LPC_GPIO2->FIODIR      &= ~(1 << 12);    /* Setto tramite GPIO la direzione in Input */

  LPC_SC->EXTMODE = 0x7;  /* Setto la modalit? dei button su edge-sensitive*/
	

  NVIC_EnableIRQ(EINT2_IRQn);              /* Abilito le interruzioni tramite IC per KEY2 */
	NVIC_SetPriority(EINT2_IRQn, 1);				 /* Setto la priorit? per KEY2 */
  NVIC_EnableIRQ(EINT1_IRQn);              /* Abilito le interruzioni trmaite IC per KEY1  */
	NVIC_SetPriority(EINT1_IRQn, 2);				 /* Setto la priorit? per KEY1 */
  NVIC_EnableIRQ(EINT0_IRQn);              /* Abilito le interruzioni tramite IC per INT0*/
	NVIC_SetPriority(EINT0_IRQn, 0);				 /* Setto la priorit? per INT0 */
	
	
	/* DISABILITARE I BUTTON INTERRUPT A LIVELLO DI IC
	
	NVIC_DisableIRQ(EINT0_IRQn)
	NVIC_DisableIRQ(EINT1_IRQn)
	NVIC_DisableIRQ(EINT2_IRQn)
	
	
	*/
}
