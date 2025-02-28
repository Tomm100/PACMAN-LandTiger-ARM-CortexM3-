/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_timer.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    timer.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include <string.h>
#include "LPC17xx.h"
#include "timer.h"
#include "../GLCD/GLCD.h" 
#include "../TouchPanel/TouchPanel.h"
#include <stdio.h> /*for sprintf*/
#include "../sample.h"

/******************************************************************************
** Function name:		Timer0_IRQHandler
**
** Descriptions:		Timer/Counter 0 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
extern volatile int posizione; // 1->su , 2-> dx, 3-> giu, 4->sx 
extern pacMan p;
extern volatile int score;
extern volatile int numPills;
extern volatile int scoreForLives;
extern volatile int lives;
extern volatile int countDown;
extern volatile int pausa;
extern volatile ghost g;
extern volatile int pausa;
extern volatile int gameMode;
extern volatile int secFrightenedMode;
extern volatile int countDownBlinkyRespawn;

extern volatile int startGame;
extern volatile int gameOver;

extern volatile int writeScore;
extern volatile int writecountDown;
extern volatile int writeLives;

void updateScore(int i,int j){
	
	

			// Nella 0->standardPill, 2->noPills , 3->PowerPills
	char scoreStr[12];  // Buffer per contenere il punteggio come stringa (fino a 11 cifre più terminatore)
  char livesStr[12];  
	
	if(labyrinth[i][j]==standardPills){
			
			score+=10;
			
			writeScore=1;
			//GUI_Text(64, 0, (uint8_t *)"   ", Black, Black);
			//sprintf(scoreStr, "%d", score);  // Converte l'intero in una stringa
			//GUI_Text(64, 0, (uint8_t *)scoreStr, White, Black);
		  scoreForLives+=10;
			numPills--;
	}
	
	if(labyrinth[i][j]==powerPills){
			score+=50;
			// cambio gameMode per il fantasma
			gameMode = 1;
			secFrightenedMode = 10;
		  
		  writeScore=1;
			//GUI_Text(64, 0, (uint8_t *)"   ", Black, Black);
			//sprintf(scoreStr, "%d", score);  // Converte l'intero in una stringa
			//GUI_Text(64, 0, (uint8_t *)scoreStr, White, Black);
		  scoreForLives+=50;
			numPills--;
	}
	
	if (scoreForLives>=1000){
		lives++;
		writeLives=1;
		
//		sprintf(livesStr, "%d", lives);  
//		GUI_Text(64, 304, (uint8_t *)"   ", Black, Black);
//		GUI_Text(64, 304, (uint8_t *)livesStr, White, Black);
		scoreForLives-=1000;
	}
	if(numPills==0){
		/*schermata di vittoria */
		disable_timer(0);
		disable_timer(1);
		disable_timer(2);
		disable_timer(3);
		disable_RIT();
		ShowWinScreen();
		
	}
	
	
	
	
}

void blinkyMangiaPacman(){

	char livesStr[12];

	if(gameMode==0){
	if((g.i==p.i) && (g.j==p.j)){
			//Blinky mangia pacman
			ClearPacman(p.posX,p.posY);
			aggiornaPacMan(pacManStartX,pacManStartY,pacManStartI,pacManStartJ);
			posizione = -1;
			
			DrawPacman(p.posX,p.posY,Yellow,Black);
			if(lives>0)
					lives--;
			
		  writeLives=1;
//			sprintf(livesStr, "%d", lives);  
//			GUI_Text(64, 304, (uint8_t *)"   ", Black, Black);
//			GUI_Text(64, 304, (uint8_t *)livesStr, White, Black);
		}
	}
}



void pacmanMangiaBlinky(){

	
  char scoreStr[12];
	
	if(gameMode==1){
	if((g.i==p.i) && (g.j==p.j)){
			//Pacman mangia Blinky
			countDownBlinkyRespawn = 3;
			score +=100;
			gameMode=0;
			secFrightenedMode=0;
			ClearPacman(g.posX,g.posY);
			aggiornaGhost(ghostStartX,ghostStartY,ghostStartI,ghostStartJ);
		  writeScore=1;
//			GUI_Text(64, 0, (uint8_t *)"   ", Black, Black);
//			sprintf(scoreStr, "%d", score);  // Converte l'intero in una stringa
//			GUI_Text(64, 0, (uint8_t *)scoreStr, White, Black);
			
		}
	}
}









int manhattan_distance(int i1, int j1, int i2, int j2) {
    int distance = 0;
    if (i1 > i2) {
		
		distance += i1 - i2;}
    else {distance += i2 - i1;
		}
    if (j1 > j2) {distance += j1 - j2;}
    else {distance += j2 - j1;}

    return distance;
}

void blinky_follow_pacman() {
    int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}}; // Su, Giù, Sinistra, Destra
    int opposite_directions[4] = {1, 0, 3, 2}; // Direzioni opposte: Su-Giù, Sinistra-Destra
    static int previous_direction = -1; // Direzione precedente, -1 indica nessuna mossa precedente
    int min_distance = 10000; // Un valore grande
    int next_i = g.i;
    int next_j = g.j;
    int d, pacmanI, pacmanJ;
		int distance;
		
    pacmanI = p.i;
    pacmanJ = p.j;

    for (d = 0; d < 4; d++) {
        // Evita di muoversi nella direzione opposta alla precedente
        if (previous_direction != -1 && d == opposite_directions[previous_direction]) {
            continue;
        }

        int new_i = g.i + directions[d][0];
        int new_j = g.j + directions[d][1];

        // Controlla se il passo è valido
        if (new_i >= 0 && new_i < ROW && new_j >= 0 && new_j < COLUMNS && labyrinth[new_i][new_j] != MURO) {
					
						if((g.i>=8) && (g.j>=7) && (g.i>=8) && (g.j<=14) && (g.i<=11) && (g.j>=7) && (g.i<=11) && (g.j<=14)){
								distance = manhattan_distance(new_i, new_j, 8, 15);
						
						}
						else{
								distance = manhattan_distance(new_i, new_j, pacmanI, pacmanJ);
						}
            if (distance < min_distance) {
                min_distance = distance;
                next_i = new_i;
                next_j = new_j;
                previous_direction = d; // Aggiorna la direzione scelta
            }
        }
    }

    // Aggiorna la posizione di Blinky
    ClearPacman(g.posX, g.posY);
    if (labyrinth[g.i][g.j] == standardPills) {
        DrawStandardPills(g.posX, g.posY, Yellow, 2);
    } else if (labyrinth[g.i][g.j] == powerPills) {
        DrawStandardPills(g.posX, g.posY, Yellow, 4);
    }
    aggiornaGhost(next_j * dimCellaX, next_i * dimCellaY + OFFSETY, next_i, next_j);
		
		blinkyMangiaPacman();
    DrawGhost(g.posX, g.posY, Red, Black);
		
}





void blinky_escape() {
    int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}}; // Su, Giù, Sinistra, Destra
    int max_distance = -1; 
    int next_i = g.i;
    int next_j = g.j;
		int d, pacmanI, pacmanJ;
		pacmanI = p.i;
		pacmanJ = p.j;
		

    for (d = 0; d < 4; d++) {
        int new_i = g.i + directions[d][0];
        int new_j = g.j + directions[d][1];

        // Controlla se il passo è valido
        if (new_i >= 0 && new_i < ROW && new_j >= 0 && new_j < COLUMNS && labyrinth[new_i][new_j] != MURO) {
            int distance = manhattan_distance(new_i, new_j, pacmanI, pacmanJ);
            if (distance > max_distance) {
                max_distance = distance;
                next_i = new_i;
                next_j = new_j;
            }
        }
    }

    // Aggiorna la posizione di Blinky
		
		ClearPacman(g.posX,g.posY);
		if(labyrinth[g.i][g.j]==standardPills){
					DrawStandardPills(g.posX,g.posY,Yellow,2);
		}
		
		else if(labyrinth[g.i][g.j]==powerPills){
			DrawStandardPills(g.posX,g.posY,Yellow,4);
		}
		aggiornaGhost(next_j*dimCellaX,next_i*dimCellaY+OFFSETY,next_i,next_j);
		
		
		
		
		
    DrawGhost(g.posX,g.posY,Blue,Black);
		
}







void TIMER0_IRQHandler (void)
{
	
	static int pausaSettata=0;
	static int eroInPausa =0;
	
	if(startGame==1 && pausa==1 & pausaSettata==0){
			pausaSettata=1;
			ShowPauseScreen();
		  eroInPausa = 1;
	
	}
	else if(pausa==0 & eroInPausa==0){
		//se non sono in pausa e non ero in pausa
	if(posizione==1){
	// UP
		uint16_t oldPosX = p.posX;
		uint16_t oldPosY = p.posY;
		int oldI = p.i;
		int oldJ = p.j;
		int newI = oldI-1;
		
		uint16_t newPosY = oldPosY-dimCellaY;
		
		if((labyrinth[newI][oldJ]!=MURO) ){
					
					
			ClearPacman(oldPosX,oldPosY);
			DrawPacman(oldPosX,newPosY,Yellow, Black);
			updateScore(newI,oldJ);
			aggiornaPacMan(oldPosX,newPosY,newI,oldJ);
			labyrinth[oldI][oldJ]=2;
			
			//oldPosX = p.posX;
			//oldPosY = p.posY;
			//oldI = p.i;
			//oldJ = p.j;
			//newI = oldI-1;
			//newPosY = oldPosY-dimCellaY;
					
				}
	}
	
	if(posizione==2){
	//RIGHT
		uint16_t oldPosX = p.posX;
		uint16_t oldPosY = p.posY;
		int oldI = p.i;
		int oldJ = p.j;
		int newJ = oldJ +1;
		uint16_t newPosX = oldPosX+dimCellaX;
		
		/* manca implementazione tunnel */
		if(labyrinth[oldI][newJ]!=MURO){
			ClearPacman(oldPosX,oldPosY);
			DrawPacman(newPosX,oldPosY,Yellow, Black);
			updateScore(oldI,newJ);
			aggiornaPacMan(newPosX,oldPosY,oldI,newJ);
			labyrinth[oldI][oldJ]=2;
			
			//oldPosX = p.posX;
			//oldPosY = p.posY;
			//oldI = p.i;
			//oldJ = p.j;
			//newJ = oldJ +1;
			//newPosX = oldPosX+dimCellaX;
				}
		if(newJ==COLUMNS){
			
			newJ=0;
			newPosX=0;
			ClearPacman(oldPosX,oldPosY);
			DrawPacman(newPosX,oldPosY,Yellow, Black);
			updateScore(oldI,newJ);
			aggiornaPacMan(newPosX,oldPosY,oldI,newJ);
			labyrinth[oldI][oldJ]=2;
		}
		
		
	}

		if(posizione==3){
	//DOWN
				uint16_t oldPosX = p.posX;
				uint16_t oldPosY = p.posY;
				int oldI = p.i;
				int oldJ = p.j;
				int newI = oldI+1;
				uint16_t newPosY = oldPosY+dimCellaY;
			if(labyrinth[newI][oldJ]!=MURO){
					
					ClearPacman(oldPosX,oldPosY);
					DrawPacman(oldPosX,newPosY,Yellow, Black);
					updateScore(newI,oldJ);
					aggiornaPacMan(oldPosX,newPosY,newI,oldJ);
					labyrinth[oldI][oldJ]=2;
					
					//oldPosX = p.posX;
					//oldPosY = p.posY;
					//oldI = p.i;
					//oldJ = p.j;
					//newI = oldI+1;
					//newPosY = oldPosY+dimCellaY;
				}
			
			
	}
	if(posizione==4){
	//LEFT
		
		
		uint16_t oldPosX = p.posX;
				uint16_t oldPosY = p.posY;
				int oldI = p.i;
				int oldJ = p.j;
				int newJ = oldJ -1;
				uint16_t newPosX = oldPosX-dimCellaX;
		
		/* manca implementazione tunnel */
				if(labyrinth[oldI][newJ]!=MURO){
					ClearPacman(oldPosX,oldPosY);
					DrawPacman(newPosX,oldPosY,Yellow, Black);
					updateScore(oldI,newJ);
					aggiornaPacMan(newPosX,oldPosY,oldI,newJ);
					labyrinth[oldI][oldJ]=2;
					
					//oldPosX = p.posX;
					//oldPosY = p.posY;
					//oldI = p.i;
					//oldJ = p.j;
					//newJ = oldJ -1;
					//newPosX = oldPosX-dimCellaX;
				}
				
				if(newJ==-1){
			
			newJ=COLUMNS-1;
			newPosX=240-dimCellaX;
			ClearPacman(oldPosX,oldPosY);
			DrawPacman(newPosX,oldPosY,Yellow, Black);
			updateScore(oldI,newJ);
			aggiornaPacMan(newPosX,oldPosY,oldI,newJ);
			labyrinth[oldI][oldJ]=2;
		}
	}	
	pacmanMangiaBlinky();
	blinkyMangiaPacman();
  
}
	else if (pausa==1 & pausaSettata==0){
			// se sono in pausa e non ho ancora stampato la schermata
		  pausaSettata=1;
			ShowPauseScreen();
		  eroInPausa = 1;
	}
	else if(pausa==0 & eroInPausa==1){
		// se non sono in pausa e prima ero in pausa devo disegnare labirinto e pacman
		LCD_Clear(Black);
		drawLabirinth();
		DrawPacman(p.posX,p.posY,Yellow,Black);
		DrawGhost(g.posX, g.posY,Red,Black);
		eroInPausa=0;
		pausaSettata = 0;
	}
	LPC_TIM0->IR = 1;			/* clear interrupt flag */
  return;
}


/******************************************************************************
** Function name:		Timer1_IRQHandler
**
** Descriptions:		Timer/Counter 1 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/






uint16_t SinTable[45] =                                       /* ÕýÏÒ±í                       */
{
    410, 467, 523, 576, 627, 673, 714, 749, 778,
    799, 813, 819, 817, 807, 789, 764, 732, 694, 
    650, 602, 550, 495, 438, 381, 324, 270, 217,
    169, 125, 87 , 55 , 30 , 12 , 2  , 0  , 6  ,   
    20 , 41 , 70 , 105, 146, 193, 243, 297, 353
};
void TIMER1_IRQHandler (void)
{
	
	
	
	
	
	if(startGame==1){
			static int sineticks=0;
	/* DAC management */	
	static int currentValue; 
	currentValue = SinTable[sineticks];
	currentValue -= 410;
	currentValue /= 1;
	currentValue += 410;
	LPC_DAC->DACR = currentValue <<6;
	sineticks++;
	if(sineticks==45) {sineticks=0;}
	}
	
	else if(gameOver==1){
	
			static int sineticks=0;
			/* DAC management */	
			static int currentValue; 
			currentValue = SinTable[sineticks];
			currentValue -= 410;
			currentValue /= 1;
			currentValue += 410;
			LPC_DAC->DACR = currentValue <<6;
			sineticks++;
			if(sineticks==45) {sineticks=0;}
	}
	
	
	
	else{
	char countDownStr[12];  
	
	if(pausa==0 &&startGame==0){
	if(countDown==0 || lives==0){
					/*Game over*/
		
					disable_timer(0);
					disable_timer(2);
					ShowGameOverScreen();
					gameOver=1;
					disable_timer(1);
					
	}
	else{
		countDown--;
		if(countDown==30){
			    disable_timer(2);
			    reset_timer(2);
				  init_timer(2,0x4C4B40);      		// valocizzo il ghost (200ms) 
					enable_timer(2);
		}
		writecountDown=1;
//		sprintf(countDownStr, "%d", countDown);  
//		GUI_Text(200, 0, (uint8_t *)"  ", Black, Black);
//		GUI_Text(200, 0, (uint8_t *)countDownStr, White, Black);
		if(gameMode==1 && secFrightenedMode>=0){
			secFrightenedMode--;
			if (secFrightenedMode==0){
					gameMode=0;
			}
			
		}
		if(countDownBlinkyRespawn>0){
				countDownBlinkyRespawn--;
				
			
			}
		
		
	}
	
  
}
	}
	LPC_TIM1->IR = 1;			/* clear interrupt flag */
  return;
}



void TIMER2_IRQHandler (void){
	
	if((pausa==0) && (posizione!=0) && (countDownBlinkyRespawn==0)){  //se non sono in pausa e pacman è in movimento
		
		if (gameMode==0){
		blinky_follow_pacman();
		}
		else {
			blinky_escape();
		
		}
		
	}
	LPC_TIM2->IR = 1;			/* clear interrupt flag */
	return;
}


void TIMER3_IRQHandler (void){
	
	
			disable_timer(1);
	LPC_TIM3->IR = 1;			/* clear interrupt flag */
	return;
}
/******************************************************************************
**                            End Of File
******************************************************************************/
