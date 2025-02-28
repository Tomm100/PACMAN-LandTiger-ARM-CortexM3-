/*----------------------------------------------------------------------------
 * Name:    Can.c
 * Purpose: CAN interface for for LPC17xx with MCB1700
 * Note(s): see also http://www.port.de/engl/canprod/sv_req_form.html
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2009 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------*/

#include <LPC17xx.h>                  /* LPC17xx definitions */
#include "CAN.h"                      /* LPC17xx CAN adaption layer */
#include "../GLCD/GLCD.h"
#include <stdio.h>
#include "sample.h"


extern uint8_t icr ; 							 //icr and result must be global in order to work with both real and simulated landtiger.
extern CAN_msg       CAN_TxMsg;    /* CAN message for sending */
extern CAN_msg       CAN_RxMsg;    /* CAN message for receiving */                                

extern volatile int countDown;
extern volatile int lives;
extern volatile int score;

extern volatile int writeScore;
extern volatile int writecountDown;
extern volatile int writeLives;


/*----------------------------------------------------------------------------
  CAN interrupt handler
 *----------------------------------------------------------------------------*/

void CAN_IRQHandler(void) {
    uint8_t icr;
		icr = (LPC_CAN1 -> ICR | icr) & 0xFF; 
	
		if(icr & (1 << 0)) {
				CAN_rdMsg(1, &CAN_RxMsg); 
				LPC_CAN1 -> CMR = (1 << 2);
				//non faccio nulla
		}
		
		if(icr & (1 << 1)) {
				//non faccio nulla
		}
		
		icr = 0; 
		icr = (LPC_CAN2 -> ICR | icr) & 0xFF; 
		
    // Gestione CAN Controller 2 (solo ricezione)
    if (icr & (1 << 0)) { 
        CAN_rdMsg(2, &CAN_RxMsg); 
        LPC_CAN2->CMR = (1 << 2); 

        process_CAN_message(&CAN_RxMsg);
    }
		
		if(icr & (1 << 1)) {
				//non faccio nulla
		}
}

void send_message_CAN(){
	
		uint32_t packedData = ((uint32_t)countDown << 24) | // Primi 8 bit (MSB)
													((uint32_t)lives << 16) |  // Secondi 8 bit
													(uint32_t)score;         // Ultimi 16 bit (LSB)

		uint8_t buffer[4];
		buffer[0] = (uint8_t)((packedData >> 24) & 0xFF); // Byte 0 (MSB)
		buffer[1] = (uint8_t)((packedData >> 16) & 0xFF); // Byte 1
		buffer[2] = (uint8_t)((packedData >> 8) & 0xFF);  // Byte 2
		buffer[3] = (uint8_t)(packedData & 0xFF);         // Byte 3 (LSB)
	
		uint8_t i;
		for(i = 0; i < 4; i++) {
				CAN_TxMsg.data[i] = buffer[i];
		}

		CAN_TxMsg.len = 4;
		CAN_TxMsg.id = 2; // ID del messaggio CAN
		CAN_TxMsg.format = STANDARD_FORMAT;
		CAN_TxMsg.type = DATA_FRAME;

		CAN_wrMsg(1, &CAN_TxMsg); // Invia il messaggio tramite CAN1
}

void process_CAN_message(CAN_msg *msg) {
    uint32_t receivedData;

    // Ricostruzione del dato a 32 bit dai 4 byte del messaggio
    receivedData = ((uint32_t)msg->data[0] << 24) |
                   ((uint32_t)msg->data[1] << 16) |
                   ((uint32_t)msg->data[2] << 8) |
                   ((uint32_t)msg->data[3]);

    // Estrazione dei singoli componenti
    uint8_t t = (receivedData >> 24) & 0xFF; // Tempo
    uint8_t v = (receivedData >> 16) & 0xFF; // Vite
    uint16_t s = receivedData & 0xFFFF;      // Punteggio

    // Aggiorna il display
    char score_txt[7], time_txt[3], lives_txt[12];
    sprintf(score_txt, "%d", s);
    sprintf(time_txt, "%0d", t);
	  sprintf(lives_txt, "%0d", v);
	
    
    if(writeScore){
				writeScore = 0;
			  GUI_Text(64, 0, (uint8_t *)"   ", Black, Black);
			  GUI_Text(64, 0, (uint8_t *)score_txt, White, Black);
				
		}
		if(writecountDown){
				writecountDown = 0;
			  GUI_Text(200, 0, (uint8_t *)"  ", Black, Black);
    		GUI_Text(200, 0, (uint8_t *)time_txt, White, Black);
				
		}
		if(writeLives){
				writeLives = 0;
				GUI_Text(64, 304, (uint8_t *)"   ", Black, Black);
    		GUI_Text(64, 304, (uint8_t *)lives_txt, White, Black);
		}
}