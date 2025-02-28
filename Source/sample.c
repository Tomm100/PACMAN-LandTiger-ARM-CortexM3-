/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               main.c
** Descriptions:            The GLCD application function
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2010-11-7
** Version:                 v1.0
** Descriptions:            The original version
**
**--------------------------------------------------------------------------------------------------------
** Modified by:             Paolo Bernardi
** Modified date:           03/01/2020
** Version:                 v2.0
** Descriptions:            basic program for LCD and Touch Panel teaching
**
*********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "LPC17xx.h"
#include "GLCD/GLCD.h" 
#include "TouchPanel/TouchPanel.h"
#include "timer/timer.h"
#include "../joystick.h"
#include "../RIT.h"
#include "../button.h"
#include "../sample.h"
#include <stdio.h>
#include "../CAN.h"

#ifdef SIMULATOR
extern uint8_t ScaleFlag; // <- ScaleFlag needs to visible in order for the emulator to find the symbol (can be placed also inside system_LPC17xx.h but since it is RO, it needs more work)
#endif
volatile int score =0;
volatile int numPills = 240;
volatile int lives = 1;
volatile int scoreForLives=0;
volatile int countDown = 60;
volatile gameMode = 0; //con 0 il fantasma segue pacman
volatile 	int secFrightenedMode = -1;
volatile int countDownBlinkyRespawn = 0;

volatile int writeScore=0;
volatile int writecountDown=0;
volatile int writeLives=0;


/* nella matrice 1->muro , 0->standardPill, 2->noPills , 3->PowerPills */
volatile int labyrinth[ROW][COLUMNS] ={{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
                        {1,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,1},
                        {1,0,1,1,0,1,1,1,1,0,1,1,1,0,1,1,1,1,0,1,1,1,0,1},
                        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
                        {1,0,1,1,1,0,1,0,1,1,1,1,1,1,1,1,0,1,0,1,1,1,0,1},
                        {1,0,1,1,1,0,1,0,1,1,1,1,1,1,1,1,0,1,0,1,1,1,0,1},
                        {1,0,0,0,0,0,1,0,0,0,0,1,1,0,0,0,0,1,0,0,0,0,0,1},
                        {1,0,1,1,1,0,1,1,1,1,0,1,1,0,1,1,1,1,0,1,1,1,0,1},
                        {1,0,1,1,1,0,1,2,2,2,2,2,2,2,2,2,2,1,0,1,1,1,0,1},
                        {1,0,0,0,0,0,1,2,1,1,1,2,2,1,1,1,2,1,0,0,0,0,0,1},
                        {1,1,1,1,1,0,1,2,1,2,2,2,2,2,2,1,2,1,0,1,1,1,1,1},
                        {0,0,0,0,0,0,0,2,1,2,2,2,2,2,2,1,2,0,0,0,0,0,0,0},
                        {1,1,1,1,1,0,1,2,1,1,1,1,1,1,1,1,2,1,0,1,1,1,1,1},
                        {1,0,0,0,0,0,1,2,2,2,2,2,2,2,2,2,2,1,0,0,0,0,0,1},
                        {1,0,1,1,1,0,1,1,0,1,1,1,1,1,1,0,1,1,0,1,1,1,0,1},
                        {1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1},
                        {1,0,1,1,1,0,1,1,0,1,1,0,1,0,1,1,0,1,0,1,1,1,0,1},
                        {1,0,1,1,1,0,1,1,0,1,1,0,1,0,1,1,0,1,0,1,1,1,0,1},
                        {1,0,0,0,1,0,0,0,0,0,0,0,2,0,0,0,0,0,0,1,0,0,0,1},
                        {1,1,1,0,1,0,1,0,1,1,0,1,1,0,0,1,0,1,0,1,0,1,1,1},
                        {1,0,0,0,0,0,1,0,0,0,0,1,1,0,0,0,0,1,0,0,0,0,0,1},
                        {1,0,1,1,1,1,1,1,1,1,0,1,1,0,1,1,1,1,1,1,1,1,0,1},
                        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
                        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}};
/* pacman pos riga = 18, col=12*/

volatile pacMan p;
volatile ghost g;
// priorità  Bottone -> 0, RIT -> 1, TIMER0 (sposta pacman) -> 2, TIMER1 (countDown) -> 3 , TIMER2 (ghost) -> 2
int main(void)
{
  SystemInit();  												/* System Initialization (i.e., PLL)  */
	LCD_Initialization();
	joystick_init();
	BUTTON_init();
	LCD_Clear(Black);
  CAN_Init();		
	
	aggiornaPacMan(pacManStartX,pacManStartY,pacManStartI,pacManStartJ);
	aggiornaGhost(ghostStartX,ghostStartY,ghostStartI,ghostStartJ);
	generatePowerPills();
	
	
	init_RIT(0x004C4B40);  			// Inizializzo il RIT con dealy di 50ms . RICORDA IL RIT LAVORA A UNA FREQUENZA DI 100KHz
	enable_RIT(); 							// Abilito il RIT
	
	init_timer(0,0x4C4B40);   	 //200ms sec (muove pacman)
	enable_timer(0);
	
	//init_timer(1,0x23C3460);     // 1s (0x17D7840)->timer per countdown  1,5s (0x23C3460)
	//enable_timer(1);
	
	//init_timer(2,0xBEBC20);      		// per il ghost (500ms) 
	//enable_timer(2);
	
	LPC_SC->PCON |= 0x1;									/* power-down	mode										*/
	LPC_SC->PCON &= ~(0x2);						
	
	LPC_PINCON->PINSEL1 |= (1<<21);
	LPC_PINCON->PINSEL1 &= ~(1<<20);
	LPC_GPIO0->FIODIR |= (1<<26);
	
  while (1)	
  {
		__ASM("wfi");
  }
}


void drawLabirinth(){
		
	
		// START Y= 20
		// END Y=300
		// Labirinto y=280 x=240
		// dimCellaX e dimCellaY definiti in GLCD.h
		int startx=0,starty=OFFSETY,i,j;
		char scoreStr[12];  
    char livesStr[12];
		char countDownStr[12];
	for (i=0; i<ROW;i++){
			for(j=0; j<COLUMNS;j++){
				if(labyrinth[i][j]==MURO){
					DrawSquare(startx,starty,Blue);
			}
				else if (labyrinth[i][j]==standardPills){
				DrawStandardPills(startx,starty,Yellow,2);
			}
			else if(labyrinth[i][j]==powerPills){
					//funzione per disegnare powerPills
					DrawStandardPills(startx,starty,Yellow,5);
			}
			startx+=dimCellaX;
		}
			startx=0;
			starty+=dimCellaY;
	}
		GUI_Text(0, 0, (uint8_t *)"SCORE:   ", White, Black);
	  GUI_Text(152, 0, (uint8_t *)"TIME:   ", White, Black);
    GUI_Text(0, 304, (uint8_t *)"LIVES:   ", White, Black);
		sprintf(scoreStr, "%d", score);  
		GUI_Text(64, 0, (uint8_t *)scoreStr, White, Black);
	  sprintf(livesStr, "%d", lives);
	  GUI_Text(64, 304, (uint8_t *)livesStr, White, Black);
	  
}

void aggiornaPacMan(uint16_t x, uint16_t y, int i, int j){
		p.posX = x;
		p.posY=y;
		p.i= i;
		p.j=j;
		
	
}

void generatePowerPills() {
    int pairsGenerated = 0;
    int maxPairs = 6;
		int seed = 42;
    srand(seed); // Inizializza il generatore di numeri casuali con il seed

    while (pairsGenerated < maxPairs) {
        // Genera indici casuali
        int randomRow = rand() % ROW;
        int randomCol = rand() % COLUMNS;

        // Controlla se c'è una standardPills in quella posizione
        if (labyrinth[randomRow][randomCol] == standardPills) {
            // Rimpiazza con una powerPills
            labyrinth[randomRow][randomCol] = powerPills;
            pairsGenerated++;
        }
    }
		return; 
}
void aggiornaGhost(uint16_t x, uint16_t y, int i, int j){
		g.posX = x;
		g.posY=y;
		g.i= i;
		g.j=j;
		
	
}


/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
