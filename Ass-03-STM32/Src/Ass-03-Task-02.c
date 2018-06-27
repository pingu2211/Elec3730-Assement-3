//     $Date: 2018-05-22 06:24:02 +1000 (Tue, 22 May 2018) $
// $Revision: 1330 $
//   $Author: Peter $

#include "Ass-03.h"


// DEF static screen element locations
 //button structure
typedef struct button{
	void (*handler)(void);
	uint16_t x, y ,width, hight;
	char * text;
	uint16_t fillColour;
	uint16_t altfillColour;
	uint16_t textColour;
};

// takes button structure, prints it to lcd
void button_show(struct button B){
	osMutexWait(myMutex01Handle, osWaitForever);
	BSP_LCD_SetTextColor(B.fillColour);
	BSP_LCD_FillRect(B.x, B.y, B.width,	B.hight);
	BSP_LCD_SetTextColor(B.textColour);
	BSP_LCD_SetBackColor(B.fillColour);
	BSP_LCD_DisplayStringAt(B.x+B.width/2, B.y+B.hight/2, B.text, CENTER_MODE);
	osMutexRelease(myMutex01Handle);
}

// returns true if the user is pressing the button, animates the button press
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
			osDelay(SECONDS_TO_MILLI/4);
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

// start of button functions called by button presses

char mem[10];

void Start(void){
	if(USR_DEBUG){safe_printf("Start button\n\r");}
	struct DSP_LCD_CONTROL *Control;
	Control= osPoolAlloc(LCD_ControlPool);						// allocate shared pool memory
	Control->run = CONTINUOUS;									// set pool memory
	Control->time = -1;
	osMessagePut(LCD_ControlMsg, Control, osWaitForever);		// tell task 4 there is new pool memory
	return;
}

void Stop(void){
	if(USR_DEBUG){safe_printf("Stop button\n\r");}
	struct DSP_LCD_CONTROL *Control;
	Control= osPoolAlloc(LCD_ControlPool);						// allocate shared pool memory
	Control->run = STOP;										//set pool memory
	Control->time = -1;
	osMessagePut(LCD_ControlMsg, Control, osWaitForever);		// send task 4 the pointer to the pooled memory
	return;
}

void Load(void){
	struct DSP_LCD_CONTROL *Control;
	if(USR_DEBUG){safe_printf("Load button\n\r");}
	 Stop();
	 osDelay(100);
	 if (strlen(mem)==0){strcpy(mem, "MEM1.DAT");}
	 read_from_file(mem, ADC_SCREEN);
		Control= osPoolAlloc(LCD_ControlPool);						// allocate shared pool memory
		Control->run = RECALL;									// set pool memory
		Control->time = -1;
		if(USR_DEBUG){safe_printf("Recall mode\n\r");}
	osMessagePut(LCD_ControlMsg, Control, osWaitForever);		// tell task 4 there is new pool memory
	return;
}
void Store(void){
	if (strlen(mem)==0){strcpy(mem, "MEM1.DAT");}
	if(USR_DEBUG){safe_printf("Store button\n\r");}
	 Stop();
	 write_to_file(mem, ADC_SCREEN, (uint16_t)10, (uint16_t)TPD_MAX);
	return;
}

void MEM1(void){
	if(USR_DEBUG){safe_printf("MEM1 button\n\r");}
	strcpy(mem, "MEM1.DAT");
	return;
}

void MEM2(void){
	if(USR_DEBUG){safe_printf("MEM2 button\n\r");}
	strcpy(mem, "MEM2.DAT");
	return;
}

void MEM3(void){
	if(USR_DEBUG){safe_printf("MEM3 button\n\r");}
	strcpy(mem, "MEM3.DAT");
	return;
}

void MEM4(void){
	if(USR_DEBUG){safe_printf("MEM4 button\n\r");}
	strcpy(mem, "MEM4.DAT");
	return;
}
void MEM5(void){
	if(USR_DEBUG){safe_printf("MEM4 button\n\r");}
	strcpy(mem, "MEM4.DAT");
	return;
}



struct button buttons[]={
		//{handler, X,Y,Width,Height,Lable,Fill, pressed fill, text colour}
		{Start,	(1/6.0)*XOFF, YOFF		,XOFF*(2.0/3),40,				"Start"	,GREEN,BLUE,BLACK},
		{Stop,	(1/6.0)*XOFF, YOFF+50 	,XOFF*(2.0/3),40,				"Stop"	,RED,BLUE,BLACK},
		{Load,	(1/6.0)*XOFF, YOFF+100 	,XOFF*(2.0/3),40,				"Load"	,LIGHT_BLUE,BLUE,BLACK},
		{Store,	(1/6.0)*XOFF, YOFF+150	,XOFF*(2.0/3),40,				"Store"	,LIGHT_BLUE,BLUE,BLACK},
		{MEM1,  XOFF, YOFF+YSIZE+10	,XOFF*(2.0/3),40 ,					"MEM1"	,LIGHT_BLUE,BLUE,BLACK},
		{MEM2,  XOFF+XOFF*(2.0/3)+10, YOFF+YSIZE+10	,XOFF*(2.0/3),40 ,	"MEM2"	,LIGHT_BLUE,BLUE,BLACK},
		{MEM3,  XOFF+(XOFF*(2.0/3)+10)*2, YOFF+YSIZE+10 ,XOFF*(2.0/3),40 ,"MEM3"	,LIGHT_BLUE,BLUE,BLACK},
		{MEM4,  XOFF+(XOFF*(2.0/3)+10)*3, YOFF+YSIZE+10 ,XOFF*(2.0/3),40 ,"MEM4"	,LIGHT_BLUE,BLUE,BLACK},
		{MEM5,  XOFF+(XOFF*(2.0/3)+10)*4, YOFF+YSIZE+10 ,XOFF*(2.0/3),40 ,"MEM5"	,LIGHT_BLUE,BLUE,BLACK},
		//{NULL,(1/6.0)*XOFF, YOFF+150	,XOFF*(2.0/3),40 ,"Store"	,LIGHT_BLUE,BLUE,BLACK},
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
			if(USR_DEBUG){safe_printf("checking %s\n\r", buttons[i].text);}
			if (is_pressed(buttons[i],&display)){ break;}
		}

		if((display.y > YOFF+5) && (display.y < YOFF+YSIZE-5) &&
			(display.x > XOFF+5) && (display.x < XOFF+XSIZE-5))
		{
			osMutexWait(myMutex01Handle, osWaitForever);
			BSP_LCD_FillCircle(display.x, display.y, 2);
			osMutexRelease(myMutex01Handle);
			loop++;
			if(USR_DEBUG){safe_printf("Task 2: %d (got  %3d,%3d)\n\r", loop, display.x, display.y);}
		}


    }
}
}
