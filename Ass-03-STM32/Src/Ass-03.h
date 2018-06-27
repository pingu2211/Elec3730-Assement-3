//     $Date: 2018-05-22 06:24:02 +1000 (Tue, 22 May 2018) $
// $Revision: 1330 $
//   $Author: Peter $

// Assignment 3 include file

#ifndef ASS_03_H_
#define ASS_03_H_

// Standard includes
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

#include "stm32f4xx_hal.h"
#include "usart.h"
#include "touch_panel.h"
#include "openx07v_c_lcd.h"
#include "fatfs.h"
#include "adc.h"

#include <stdio.h>
#include <stdint.h>
#include <malloc.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

// OS handles
extern osThreadId defaultTaskHandle;
extern osThreadId myTask02Handle;
extern osThreadId myTask03Handle;
extern osThreadId myTask04Handle;
extern osTimerId myTimer01Handle;

extern osSemaphoreId myBinarySem01Handle;
extern osSemaphoreId myBinarySem02Handle;
extern osSemaphoreId myBinarySem03Handle;
extern osSemaphoreId myBinarySem04Handle;
extern osSemaphoreId myBinarySem05Handle;
extern osSemaphoreId myBinarySem06Handle;

extern osMessageQId myQueue01Handle;
extern osMessageQId LCD_ControlMsg;
extern osPoolId     LCD_ControlPool;


extern osMutexId myMutex01Handle; // Protect LCD
extern osMutexId myMutex02Handle; // Protect console output


typedef enum {
	RECALL = -2,
	CONTINUOUS = -1,
	STOP = 0,
	ONE_SHOT=1
}TRIGGER_TYPE;

typedef struct DSP_LCD_CONTROL{
	TRIGGER_TYPE run;
	int time;
}DSP_LCD_CONTROL;


// Assignment tasks
extern void Ass_03_Task_01(void const *argument);
extern void Ass_03_Task_02(void const *argument);
extern void Ass_03_Task_03(void const *argument);
extern void Ass_03_Task_04(void const *argument);

extern void Start(void);
extern void Stop(void);
extern void Load(void);
extern void Store(void);
extern void MEM1(void);
extern void MEM2(void);
extern void MEM3(void);



// Library functions
extern uint8_t BSP_TP_Init(void);
extern uint8_t BSP_TP_GetDisplayPoint(Coordinate *pDisplay);
extern void F_ErrorInterp(FRESULT code);

// Front panel input
extern uint8_t getfp(Coordinate *display);

// STEPIEN: Safe printf() to ensure that only one task can write to
//          the console at a time

#define safe_printf(fmt, ...) \
	osMutexWait(myMutex02Handle, osWaitForever); \
	printf(fmt, ##__VA_ARGS__); \
	fflush(stdout); \
	osMutexRelease(myMutex02Handle);

volatile bool USR_DEBUG;

#endif /* ASS_03_H_ */
#define PI 3.1415926535897932
#define BUFFER_MAX 1000 //
#define SAMPLES_PER_SECOND 1000 //us
#define SECONDS_TO_MILLI 1000
#define TPD_MAX 10*BUFFER_MAX
volatile uint16_t * ADC_SCREEN;

#define XOFF 55
#define YOFF 30
#define XSIZE 250
#define YSIZE 150

#define GREEN		0x3722
#define RED			0xE0A2
#define BLUE		0x131C
#define LIGHT_BLUE  0x7E5E
#define ORANGE  	0xFCE0
#define WHITE		0xFFFF
#define BLACK 		0x0000
#define MAX_LEN 18
