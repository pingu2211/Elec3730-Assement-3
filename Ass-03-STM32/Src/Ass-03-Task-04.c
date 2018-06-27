//     $Date: 2018-05-22 06:24:02 +1000 (Tue, 22 May 2018) $
// $Revision: 1330 $
//   $Author: Peter $

#include "Ass-03.h"
// This is the task that reads the analog input. A buffer is divided in two to
// allow working on one half of the buffer while the other half is being
// loaded using the DMA controller.
//
// This task also plots the wave form to the screen.
//
// *** MAKE UPDATES TO THE CODE AS REQUIRED ***
//
// Note that there needs to be a way of starting and stopping the display.

uint16_t ADC_Value[BUFFER_MAX];


void Ass_03_Task_04(void const * argument)
{
	uint16_t i;
	HAL_StatusTypeDef status;
	uint16_t xpos=0;
	uint16_t ypos=0;
	uint16_t last_xpos=0;
	uint16_t last_ypos=0;
	int tpd = 10000;
	int spp;
	int pps;
	int RUNNING=-1;
	int ADC_ITTR=0;
	struct DSP_LCD_CONTROL *Control;
	osEvent Event;
	// sample per pixel = (10000us/XSIZEpix) * 1/t/s time per sample

	//int map(int x, int in_min, int in_max, int out_min, int out_max);

	osSignalWait(1,osWaitForever);
	safe_printf("/n/rHello from Task 4 - Analog Input (turn ADC knob or use pulse sensor)\n\r");

	// Draw a box to plot in
	osMutexWait(myMutex01Handle, osWaitForever);
	BSP_LCD_DrawRect(XOFF-1,YOFF-1,XSIZE+1,YSIZE+1);
	BSP_LCD_SetTextColor(BLACK);
	BSP_LCD_SetBackColor(WHITE);
	BSP_LCD_DisplayStringAt(XOFF+XSIZE,YOFF-10, "10",CENTER_MODE);
	BSP_LCD_DisplayStringAt(XOFF,YOFF-10, "0" ,CENTER_MODE);
	osMutexRelease(myMutex01Handle);
	// Start the conversion process
	status = HAL_ADC_Start_DMA(&hadc1, (uint32_t *)ADC_Value, BUFFER_MAX);
	if (status != HAL_OK)
	{
	  safe_printf("ERROR: Task 4 HAL_ADC_Start_DMA() %d\n\r", status);
	}
	// Start main loop
	char snum[5];
	spp = tpd/(XSIZE-2);
	pps = (XSIZE)/(tpd/1000);
	itoa(tpd/1000, snum, 10);
	ADC_SCREEN = malloc (tpd*sizeof(uint16_t));
	while (1)
	{
	  Event = osMessageGet(LCD_ControlMsg, 0);
	  if (Event.status == osEventMessage){
		  Control = Event.value.p;
		  if (Control->time>0){
			  tpd = Control->time;
			  ADC_SCREEN = realloc (ADC_SCREEN,tpd*sizeof(uint16_t));
			  spp = (tpd/(XSIZE-2));
			  pps = (XSIZE)/(tpd/1000);
			  itoa(tpd/1000, snum, 10);
			  osMutexWait(myMutex01Handle, osWaitForever);
			  BSP_LCD_SetTextColor(BLACK);
			  BSP_LCD_SetBackColor(WHITE);
			  BSP_LCD_DrawRect(XOFF-1,YOFF-1,XSIZE+1,YSIZE+1);
			  osMutexRelease(myMutex01Handle);
			  xpos=0;
			  last_xpos=0;
		  }
		  if(Control->run==ONE_SHOT){
			  	safe_printf("Running mode set to ONESHOT\n\r");
				RUNNING = Control->time;
		  }else if(Control->run==CONTINUOUS){
			  RUNNING = CONTINUOUS;
			  safe_printf("Running mode set to CONTINUOUS\n\r");
		  }else if(Control->run==RECALL){
			  xpos=0;
			  last_xpos=0;
			  RUNNING = Control->run;
			  safe_printf("Running mode set to RECALLl\n\r");
		  }else if(Control->run==STOP){
				safe_printf("Running mode set to STOP\n\r");
				RUNNING = Control->run;
		  }else{
			  safe_printf("Running in undefined mode %i \n\r", Control->run);
			  RUNNING = Control->run;
		  }
		  osPoolFree(LCD_ControlPool,Event.value.p);
	  }

	  if (RUNNING==CONTINUOUS || RUNNING>=ONE_SHOT){
	  // Wait for first half of buffer
		  osSemaphoreWait(myBinarySem05Handle, osWaitForever);
		  osMutexWait(myMutex01Handle, osWaitForever);
		  BSP_LCD_SetTextColor(BLACK);
		  BSP_LCD_SetBackColor(WHITE);
		  BSP_LCD_DisplayStringAt(XOFF+XSIZE,YOFF-10, snum ,CENTER_MODE);
		  osMutexRelease(myMutex01Handle);
		  if(ADC_ITTR + BUFFER_MAX<=TPD_MAX-1){
			  ADC_ITTR += BUFFER_MAX;
		  }else{
			  memcpy(ADC_SCREEN,&ADC_SCREEN[BUFFER_MAX],(TPD_MAX-BUFFER_MAX)*sizeof(uint16_t));
			  if(ADC_SCREEN==NULL){
				  safe_printf("MEMORY ERROR TOUCH SCREEN");
				  osDelay(1000);
			  }
		  }
		  for(i=0;i<BUFFER_MAX;i++)
		  {
			  if (i==BUFFER_MAX/2){
				  osSemaphoreWait(myBinarySem06Handle, osWaitForever);
				  HAL_GPIO_WritePin(GPIOD, LD4_Pin, GPIO_PIN_SET);
			  }

			  ADC_SCREEN[ADC_ITTR + i]=ADC_Value[i];

			  if(i%spp==0){
				  osMutexWait(myMutex01Handle, osWaitForever);
					  BSP_LCD_SetTextColor((xpos%pps==0)?LCD_COLOR_RED:LCD_COLOR_WHITE);
					  BSP_LCD_DrawVLine(XOFF+xpos,YOFF,YSIZE);
					  BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
					  ypos=(uint16_t)((uint32_t)((ADC_Value[i]))*YSIZE/(4064));
					  BSP_LCD_DrawLine(XOFF+last_xpos,YOFF+last_ypos,XOFF+xpos,YOFF+ypos);
				  osMutexRelease(myMutex01Handle);

				  last_xpos=xpos;
				  last_ypos=ypos;
				  xpos++;
				  if (last_xpos>=XSIZE-1)
				  {
					  xpos=0;
					  last_xpos=0;
				  }
			}
		  }

		  HAL_GPIO_WritePin(GPIOD, LD4_Pin, GPIO_PIN_RESET);
	  }else if(RUNNING==RECALL){
		  for(i=0;i<XSIZE;i++){
			  osMutexWait(myMutex01Handle, osWaitForever);
				  BSP_LCD_SetTextColor((xpos%pps==0)?LCD_COLOR_RED:LCD_COLOR_WHITE);
				  BSP_LCD_DrawVLine(XOFF+xpos,YOFF,YSIZE);
				  BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
				  ypos=(uint16_t)(((ADC_SCREEN[i*spp]))*YSIZE/(4064));
				  BSP_LCD_DrawLine(XOFF+last_xpos,YOFF+last_ypos,XOFF+xpos,YOFF+ypos);
			  osMutexRelease(myMutex01Handle);
			  last_xpos=xpos;
			  last_ypos=ypos;
			  xpos++;
			  if (last_xpos>=XSIZE-1)
			  {
				  safe_printf("RECALL Mode finished");
				  RUNNING=STOP;
				  xpos=0;
				  last_xpos=0;
			  }
		  }
	  }else{
		  osDelay(100);
	  }
	}
}

void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc)
{
	osSemaphoreRelease(myBinarySem05Handle);
	HAL_GPIO_WritePin(GPIOD, LD3_Pin, GPIO_PIN_SET);
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	osSemaphoreRelease(myBinarySem06Handle);
	HAL_GPIO_WritePin(GPIOD, LD3_Pin, GPIO_PIN_RESET);
}
/*******************************************************************************************************/
/* function to calculate the average of the input data */
uint8_t ave(double *inp_array, int array_size){	/* input array from DAC*/
	double sum = 0;
	double ave;
	int i;
	for(i=0;i<array_size; i++){
		sum = sum+inp_array[i];			// formula for average
	}
	ave = sum/(i+1);
	return ave;
}
/*******************************************************************************************************/
/*function to calculate standard deviation of input data*/
uint8_t std_dev(double *inp_array, int array_size){
	double av = ave(inp_array,array_size);
	double stdev;
	int i;
	for(i=0;i<array_size; i++){
		stdev = sqrt((pow((inp_array[i]-av),2))/(BUFFER_MAX/2));	// formula for standard deviation
	}
	return stdev;
}
/*******************************************************************************************************/
/* function for downsampling the input data */
uint8_t downsampling(double *inp_array, int array_size, int ds_factor){
	int i;										// downsampling factor, eg. if ds_factor = 100, function would make new array with every hundredth element of the input array
	double *new_array= malloc(sizeof(double)*ds_factor);
	for(i=0;i<array_size;i+=ds_factor){
		new_array[i/ds_factor] = inp_array[i];	// new array downsampled from input array by downsampling factor, eg. if ds_factor = 100, function would make new array with every hundredth element of the input array
	}
	return *new_array;							// function returns downsampled array
}
/*******************************************************************************************************/
/* function that reads array from file */
uint8_t read_from_file(char *filename, uint16_t **inp_array){
	FIL fobj;														// file object
	FRESULT res;													// result of FATFS functions -> FR_OK = 0 = success
	uint16_t time, size;													// data info: time length of sample, size of array
	uint16_t byte_size = sizeof(uint16_t);									// number of bytes to be read. Initialized as size of integer
	uint16_t array_size_bytes, array_size;								// size of array read from file. array_size = num of array elements, array_size_bytes = num of bytes

	/* open file */
	res = f_open(&fobj, filename, FA_READ);							// FATFS function opens file for reading
	if (res!=FR_OK){														// if function failed
		safe_printf("%s failed to open.\n\r", filename);			// print error
		return -1;
	}
	/* reading time*/												// if function was successful read the file with FATFS function
	res = f_read(&fobj, &time, byte_size, &array_size_bytes);		// read first element of file = time
	if(res!=FR_OK || byte_size != array_size_bytes){														// if function failed
		safe_printf("%s could not be read time.\n\r", filename);			// print error
		return -1;
	}
	safe_printf("time: %i\n\r", time);
	/* reading array size */
	res = f_read(&fobj, &size, byte_size, &array_size_bytes);		// read second element of file = array size
	if(res!=FR_OK || byte_size != array_size_bytes){														// if function failed
			safe_printf("%s could not be read size.\n\r", filename);			// print error
			return -1;
		}
	safe_printf("size: %i\n\r", size);
	/* reading array */
	byte_size = size*sizeof(uint16_t);								// predicted byte size of array = size of array times the size of double
	res = f_read(&fobj, inp_array, byte_size, &array_size_bytes);	// read third element of the file = array of data
	if(res!=FR_OK|| byte_size != array_size_bytes){														// if function failed
			safe_printf("%s could not be read buffer.\n\r", filename);			// print error
			return -1;
		}
	array_size = array_size_bytes/sizeof(uint16_t);					// array_size_ bytes = num bytes in array. Convert to array_size = num of elements in array
	f_close(&fobj);													// FATFS function closes file
	return array_size;												// return num of elements in array
}
/*******************************************************************************************************/
/* function that reads array from file */
uint8_t write_to_file(char *filename, uint16_t *inp_array, uint16_t time, uint16_t size){
	FIL fobj;														// file object
	FRESULT res;													// result of FATFS functions -> FR_OK = 0 = success
	int byte_size = sizeof(uint16_t);									// number of bytes to be read. Initialized as size of integer
	int array_size_bytes, array_size;								// size of array read from file. array_size = num of array elements, array_size_bytes = num of bytes

	/* open file */
		res = f_open(&fobj, filename, FA_CREATE_ALWAYS|FA_WRITE);						// FATFS function opens file for reading
		if (res!=FR_OK){													// if function failed
			safe_printf("%s failed to open.\n\r", filename);		// print error
			F_ErrorInterp(res);
			return -1;
		}
	/* writing time to file */										// FATFS function writes to file
	res = f_write(&fobj, time, byte_size, &array_size_bytes);		// write time to first element in file
	if(res!=FR_OK || byte_size != array_size_bytes){														// if function failed
			safe_printf("failed to store time in %s.\n\r", filename);			// print error
			F_ErrorInterp(res);
			return -1;
		}
	/* writing array size to file */
	res = f_write(&fobj, &size, byte_size, &array_size_bytes);		// writes array size to second file elemenet
	if(res!=FR_OK || byte_size != array_size_bytes){														// if function failed
			safe_printf("failed to store arsize in %s.\n\r", filename);			// print error
			F_ErrorInterp(res);
			return -1;
	}
	/* writing array to file */
	byte_size = size*sizeof(uint16_t);								// predicted byte size of array = size of array times the size of double
	res = f_write(&fobj, inp_array, byte_size, &array_size_bytes);	// write array to third element in file
	if(res!=FR_OK || byte_size != array_size_bytes){														// if function failed
			safe_printf("failed to store array in %s.\n\r", filename);			// print error
			F_ErrorInterp(res);
			return -1;
	}
	array_size = array_size_bytes/sizeof(uint16_t);					// array size in bytes converted to array size = num of elements
	f_close(&fobj);													// FATFS function closes file
	return array_size;												// returns num of elements in array stored in file
}
