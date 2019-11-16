#include <lpc17xx.h>
#include "GLCD.h"
#include <stdio.h>
#include <cmsis_os2.h>
#include <stdbool.h>
#include "lfsr113.h"
#include "random.h"
#include <stdlib.h>
#include <time.h>

typedef struct heads{
 unsigned int x;
 unsigned int y;
}Head;

typedef struct tails{
 unsigned int x;
 unsigned int y;
}Tail;

//global variables
unsigned char direction = 'R';
bool pause = true;
//unsigned char paused [] = "PAUSED ";
//unsigned char play [] = "PLAYING";
//unsigned char right [] = "RIGHT ";
//unsigned char left [] = "LEFT   ";
//unsigned char up [] = "UP     ";
//unsigned char down [] = "DOWN     ";
unsigned int speed = 1;
unsigned int screen[40][27]={0};
unsigned int length=3;
Head head;
Tail tail;
unsigned char bigPixel []={0x001F, 0x001F, 0x001F, 0x001F, 0x001F, 0x001F, 0x001F, 0x001F,
0x001F, 0x001F, 0x001F, 0x001F, 0x001F, 0x001F, 0x001F, 0x001F,
0x001F, 0x001F, 0x001F, 0x001F, 0x001F, 0x001F, 0x001F, 0x001F,
0x001F, 0x001F, 0x001F, 0x001F, 0x001F, 0x001F, 0x001F, 0x001F,
0x001F, 0x001F, 0x001F, 0x001F, 0x001F, 0x001F, 0x001F, 0x001F,
0x001F, 0x001F, 0x001F, 0x001F, 0x001F, 0x001F, 0x001F, 0x001F,
0x001F, 0x001F, 0x001F, 0x001F, 0x001F, 0x001F, 0x001F, 0x001F,
0x001F, 0x001F, 0x001F, 0x001F, 0x001F, 0x001F, 0x001F, 0x001F,};

unsigned char blackPixel[]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

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
		
		//snake can't turn back on itself. if player attempts to do this, ignore
		if(checkUp==0){
			if (direction!='D'){
			direction = 'U';}}
		else if (checkRight==0){
			if (direction!='L'){
			direction = 'R';}}
		else if (checkDown==0){
			if (direction!='U'){
			direction = 'D';}}
		else if (checkLeft==0){
			if (direction!='R'){
			direction = 'L';}}

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

//	while(1){
//		osThreadYield();
//		
//		if(pause)
//			GLCD_DisplayString(4,4,1, paused);
//		else if(!pause)
//			GLCD_DisplayString(4,4,1, play);
//		
//		if(direction=='U'){
//			GLCD_DisplayString(6,4,1, up);
//		}
//		else if(direction=='R'){
//			GLCD_DisplayString(6,4,1, right);
//		}
//		else if(direction=='L'){
//			GLCD_DisplayString(6,4,1, left);
//		}
//		else if(direction=='D'){
//			GLCD_DisplayString(6,4,1, down);
//		}
//	}
}

void initSnake(){
	screen[15][14]=1;
	screen[14][14]=1;
	screen[13][14]=1;
  head.x=15;
	head.y=14;
	tail.x=13;
	tail.y=14;
	GLCD_Bitmap(78,112,6,8,bigPixel);
	GLCD_Bitmap(84,112,6,8,bigPixel);
	GLCD_Bitmap(90,112,6,8,bigPixel);
}

void updateTail(void){

	screen[tail.x][tail.y]--;
	//clearing
	GLCD_Bitmap(tail.x*6, tail.y*8, 6, 8, blackPixel);
	if(screen[tail.x][tail.y-1]==1){
		tail.y=tail.y-1;
	}
	else if(screen[tail.x][tail.y+1]==1){
		tail.y=tail.y+1;
	}
	else if (screen[tail.x+1][tail.y]==1){
	  tail.x=tail.x+1;
	}
	else if (screen[tail.x-1][tail.y]==1){
		tail.x=tail.x-1;
	}
	

}


void food(){
	//srand(time(0));
	int x,y;
	//making sure food is not generated where the snake already is
	do{
		x=rand()%40;
		y=rand()%27;
	}while(screen[x][y]!=0);
	
	screen[x][y]=3;
	GLCD_Bitmap(x*6,y*8,6,8,bigPixel);
	
	
}
void snake(void *arg){
	int sum=0;
	int baseSpeed = osKernelGetTickFreq();
	
	//figure out speed in ticks
	
	while (1){
		if (direction=='R'){
			sum=screen[head.x+1][head.y];
			//found food
			if(sum==3){
				length++;
				food();
			}
			else{
				updateTail();
			}
			
			//touched a part of the snake or touched the wall
      if (sum==2||head.x==39){
				//you lose
			}		
			screen[head.x+1][head.y]++;
			head.x=head.x+1;
			
	
		}
		
		if (direction=='L'){
			sum=screen[head.x-1][head.y]+1;
			//found food
			if(sum==4){
				length++;
				food();				
			}
			else{
				updateTail();
			}
			
			//touched a part of the snake or touched the wall
      if (sum==2||head.x==0){
				//you lose
			}		
			screen[head.x-1][head.y]++;
			head.x=head.x-1;
	
		}
		
		 if (direction=='U'){
			sum=screen[head.x][head.y-1]+1;
			//found food
			if(sum==4){
				length++;	
				food();
			}
			else{
				updateTail();
			}
			
			//touched a part of the snake or touched the wall
      if (sum==2||head.y==0){
				//you lose
			}		
			screen[head.x][head.y-1]++;
			head.y=head.y-1;
	
		}
		 
		 if (direction=='D'){
			sum=screen[head.x][head.y+1]+1;
			//found food
			if(sum==4){
				length++;	
				food();
			}
			else{
				updateTail();
			}
			
			//touched a part of the snake or touched the wall
      if (sum==2||head.y==26){
				//you lose
			}		
			screen[head.x][head.y+1]++;
			head.y=head.y+1;
	
		}
	 GLCD_Bitmap(head.x*6, head.y*8, 6, 8, bigPixel);
	 osDelay(baseSpeed/speed);
	}
	

}




int main(void){
	printf("babb");
	initializeLED();
	initializePotent();
	initializeScreen();
	initSnake();
	food();
	

	osKernelInitialize();
	osThreadNew(joystick, NULL, NULL);
	//osThreadNew(score, NULL, NULL);
	osThreadNew(snake,NULL,NULL);
	
	osKernelStart();
	while(1){}
	}
