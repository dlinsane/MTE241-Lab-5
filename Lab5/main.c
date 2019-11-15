#include <lpc17xx.h>
#include "GLCD.h"
#include <stdio.h>
#include <cmsis_os2.h>
#include <stdbool.h>

//global variables
unsigned char direction = 'R';
bool pause = true;
unsigned char paused [] = "PAUSED ";
unsigned char play [] = "PLAYING";
unsigned char right [] = "RIGHT ";
unsigned char left [] = "LEFT   ";
unsigned char up [] = "UP     ";
unsigned char down [] = "DOWN     ";

void initializeLED(void){
	//initializing LED	
	LPC_GPIO1->FIODIR |= 0xB0000000;
	LPC_GPIO2->FIODIR |= 0x0000007C;
	//turn off all leds
	LPC_GPIO1->FIOCLR |= 0xB0000000;
	LPC_GPIO2->FIOCLR |= 0x0000007C;
}
void initializePotent(void){
	LPC_PINCON->PINSEL1 &= ~(0x03<<18);//clear bits 18 and 19
	LPC_PINCON->PINSEL1 |= (0x01<<18);
	LPC_SC->PCONP |= (1<<12);
	LPC_ADC->ADCR = (1 << 2) |     // select AD0.2 pin
									(4 << 8) |  // ADC clock is 25MHz/5 
									(1 << 21);     // enable ADC
}

void initializeScreen(void){
	GLCD_Init();
	GLCD_Clear(0);//clear to white
	GLCD_SetBackColor(0);
	GLCD_SetTextColor(0xFFFF);
}

void joystick(void *arg){
	while(1){
		osThreadYield();
		int checkUp=(LPC_GPIO1->FIOPIN &= 0x800000);
		int checkRight=(LPC_GPIO1->FIOPIN &= 0x1000000);
		int checkDown=(LPC_GPIO1->FIOPIN &= 0x2000000);
		int checkLeft=(LPC_GPIO1->FIOPIN &= 0x4000000);
		int checkPushed=(LPC_GPIO1->FIOPIN &= 0x100000);
		
		if(checkUp==0)
			direction = 'U';
		else if (checkRight==0)
			direction = 'R';
		else if (checkDown==0)
			direction = 'D';
		else if (checkLeft==0)
			direction = 'L';

		if(checkPushed==0){
			pause = !pause;
			while((LPC_GPIO1->FIOPIN &= 0x100000)==0)
				;
		}
	}
}

//void potentiometer(void *arg){
//	

//}


//void LED(void *arg){

//	
//}

void score(void *arg){

	while(1){
		osThreadYield();
		
		if(pause)
			GLCD_DisplayString(4,4,1, paused);
		else if(!pause)
			GLCD_DisplayString(4,4,1, play);
		
		if(direction=='U')
			GLCD_DisplayString(6,4,1, up);
		else if(direction=='R')
			GLCD_DisplayString(6,4,1, right);
		else if(direction=='L')
			GLCD_DisplayString(6,4,1, left);
		else if(direction=='D')
			GLCD_DisplayString(6,4,1, down);
	}
}

//void snake(void *arg){
//	
//	
//}

//void food(void *arg){
//	
//	
//}

int main(void){
	printf("babb");
	initializeLED();
	initializePotent();
	initializeScreen();
	
	osKernelInitialize();
	osThreadNew(joystick, NULL, NULL);
	osThreadNew(score, NULL, NULL);
	
	osKernelStart();
	while(1){}
	}
