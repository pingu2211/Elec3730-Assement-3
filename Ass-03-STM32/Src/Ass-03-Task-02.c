//     $Date: 2018-05-22 06:24:02 +1000 (Tue, 22 May 2018) $
// $Revision: 1330 $
//   $Author: Peter $

#include "Ass-03.h"

//
// This task can be used as the main pulse rate application as it takes
// input from the front panel.
//
// *** MAKE UPDATES TO THE CODE AS REQUIRED ***
//
// Draw the boxes that make up the buttons and take action when the
// buttons are pressed. See suggested updates for the touch panel task
// that checks for button presses. Can do more in that task.

#define XOFF 55
#define YOFF 80
#define XSIZE 250
#define YSIZE 150

#define GREEN	0x3722
#define RED		0xE0A2
#define BLUE	0x131C
#define ORANGE  0xFCE0
#define WHITE	0xFFFF
#define BLACK 	0x0000
#define MAX_LEN 18

#define Second 1000

typedef struct button{
	void (*handler)(void);
	uint16_t x, y ,width, hight;
	char * text;
	uint16_t fillColour;
	uint16_t altfillColour;
	uint16_t textColour;
};

void button_show(struct button B){
	osMutexWait(myMutex01Handle, osWaitForever);
	BSP_LCD_SetTextColor(B.fillColour);
	BSP_LCD_FillRect(B.x, B.y, B.width,	B.hight);
	BSP_LCD_SetTextColor(B.textColour);
	BSP_LCD_SetBackColor(B.fillColour);
	BSP_LCD_DisplayStringAt(B.x+B.width/2, B.y+B.hight/2, B.text, CENTER_MODE);
	osMutexRelease(myMutex01Handle);
}

bool is_pressed(struct button B,Coordinate *point){
	osMutexWait(myMutex01Handle, osWaitForever);
	if (point->x > B.x && point->x < B.x+B.width && point->y > B.y && point->y < B.y+B.hight){
			BSP_LCD_SetTextColor(B.altfillColour);
			BSP_LCD_SetBackColor(B.altfillColour);
			BSP_LCD_FillRect(B.x, B.y, B.width,	B.hight);
			BSP_LCD_SetTextColor(B.textColour);
			BSP_LCD_SetBackColor(B.altfillColour);
			BSP_LCD_DisplayStringAt(B.x+B.width/2, B.y+B.hight/2, B.text, CENTER_MODE);
			osMutexRelease(myMutex01Handle);
			B.handler();
			osDelay(Second/4);
			osMutexWait(myMutex01Handle, osWaitForever);
			BSP_LCD_SetTextColor(B.fillColour);
			BSP_LCD_SetBackColor(B.fillColour);
			BSP_LCD_FillRect(B.x, B.y, B.width,	B.hight);
			BSP_LCD_SetTextColor(B.textColour);
			BSP_LCD_SetBackColor(B.fillColour);
			BSP_LCD_DisplayStringAt(B.x+B.width/2, B.y+B.hight/2, B.text, CENTER_MODE);
			osMutexRelease(myMutex01Handle);
		 return true;
		 }
	else{
			BSP_LCD_SetTextColor(B.fillColour);
			BSP_LCD_SetBackColor(B.fillColour);
			BSP_LCD_FillRect(B.x, B.y, B.width,	B.hight);
			BSP_LCD_SetTextColor(B.textColour);
			BSP_LCD_SetBackColor(B.fillColour);
			BSP_LCD_DisplayStringAt(B.x+B.width/2, B.y+B.hight/2, B.text, CENTER_MODE);
			osMutexRelease(myMutex01Handle);
		 return false;
	}

}

void Start(void){
	safe_printf("Start button\n\r");
	return;
}
void Stop(void){
	safe_printf("Stop button\n\r");
	return;
}
void Load(void){
	safe_printf("Load button\n\r");
	return;
}
void Store(void){
	safe_printf("Store button\n\r");
	return;
}

struct button buttons[]={
		//{handler, X,Y,Width,Height,Lable,Fill, pressed fill, text colour}
		{Start,	(1/6.0)*XOFF, YOFF	  ,XOFF*(2.0/3),40,"Start",GREEN,BLUE,BLACK},
		{Stop,	(1/6.0)*XOFF, YOFF+50 ,XOFF*(2.0/3),40,"Stop",RED,BLUE,BLACK},
		//{Load,0*xgrid,1*ygrid,xgrid,ygrid,"Load",FILL_COLOUR,ALT_COLOUR,TEXT_COLOUR},
		//{Store,0*xgrid,1*ygrid,xgrid,ygrid,"Store",FILL_COLOUR,ALT_COLOUR,TEXT_COLOUR},
		//{NULL,0*xgrid,1*ygrid,xgrid,ygrid,"Mem 1",FILL_COLOUR,ALT_COLOUR,TEXT_COLOUR},
		//{NULL,0*xgrid,1*ygrid,xgrid,ygrid,"Mem 2",FILL_COLOUR,ALT_COLOUR,TEXT_COLOUR},
		//{NULL,0*xgrid,1*ygrid,xgrid,ygrid,"Mem 3",FILL_COLOUR,ALT_COLOUR,TEXT_COLOUR},
		{NULL,NULL,NULL,NULL,NULL,NULL,NULL}
};


void Ass_03_Task_02(void const * argument)
{

	uint32_t loop=0;

	Coordinate display;

	osSignalWait(1,osWaitForever);
	safe_printf("Hello from Task 2 - Pulse Rate Application (touch screen input)\n\r");

	for (int i=0;buttons[i].handler!=NULL;i++){
		  button_show(buttons[i]);
	}

while (1)
{
	if (getfp(&display) == 0)
    {

		for (int i=0;buttons[i].handler!=NULL;i++){
			if (is_pressed(buttons[i],&display)) break;
		}
		if((display.y > YOFF+5) && (display.y < YOFF+YSIZE-5) &&
			(display.x > XOFF+5) && (display.x < XOFF+XSIZE-5))
		{
			osMutexWait(myMutex01Handle, osWaitForever);
			BSP_LCD_FillCircle(display.x, display.y, 2);
			osMutexRelease(myMutex01Handle);
			loop++;
			safe_printf("Task 2: %d (got  %3d,%3d)\n\r", loop, display.x, display.y);
		}


    }
}
}
