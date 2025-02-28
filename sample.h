#ifndef SAMPLE_H
#define SAMPLE_H

#define dimCellaX 10
#define dimCellaY 11
#include "LPC17xx.h"
#define ROW 24
#define COLUMNS 24
#define MURO 1
#define standardPills 0
#define vuota 2
#define powerPills 3
#define OFFSETY 20

#define pacManStartX 120
#define pacManStartY 218
#define pacManStartI 18
#define pacManStartJ 12


#define ghostStartX 120
#define ghostStartY 130
#define ghostStartI 10
#define ghostStartJ 12


extern volatile int labyrinth[ROW][COLUMNS];

typedef struct {
	uint16_t posX;
	uint16_t posY;
	int i;
	int j;

}pacMan;

typedef struct{
	uint16_t posX;
	uint16_t posY;
	int i;
	int j;
		
}ghost;
void drawLabirinth();
void aggiornaPacMan(uint16_t x,uint16_t y,int i, int j);
void generatePowerPills();
void aggiornaGhost(uint16_t x, uint16_t y, int i, int j);

#endif