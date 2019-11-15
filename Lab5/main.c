#include <lpc17xx.h>
#include "GLCD.h"
#include <stdio.h>
#include <cmsis_os2.h>

//global variables
unsigned char direction = "R";
bool 


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
	GLCD_Clear(0x001F);//clear to blue
	GLCD_SetBackColor(0x001F);
	GLCD_SetTextColor(0xFFFF);
}

void joystick(void *arg){
	int checkUp=(LPC_GPIO1->FIOPIN &= 0x800000);
	int checkRight=(LPC_GPIO1->FIOPIN &= 0x1000000);
	int checkDown=(LPC_GPIO1->FIOPIN &= 0x2000000);
	int checkLeft=(LPC_GPIO1->FIOPIN &= 0x4000000);
	int checkPushed=(LPC_GPIO1->FIOPIN &= 0x100000);
	
	if(checkU
	

int main(void){
	initializeLED();
	initializePotent();
	initializeScreen();
	osKernelInitialize();
	osThreadNew
	
	osKernelStart();
	while(1){}
	}