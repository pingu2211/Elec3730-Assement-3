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
#define BUFFER_MAX 1000
#define MIN_SAMPLE_TIME 0.0001
uint16_t ADC_Value[BUFFER_MAX];

void Ass_03_Task_04(void const * argument)
{
  uint16_t i;
  HAL_StatusTypeDef status;
  uint16_t xpos=0;
  uint16_t ypos=0;
  uint16_t last_xpos=0;
  uint16_t last_ypos=0;
#define XOFF 55
#define YOFF 80
#define XSIZE 250
#define YSIZE 150

  int map(int x, int in_min, int in_max, int out_min, int out_max);

  osSignalWait(1,osWaitForever);
  safe_printf("Hello from Task 4 - Analog Input (turn ADC knob or use pulse sensor)\n");

  // Draw a box to plot in
  osMutexWait(myMutex01Handle, osWaitForever);
  BSP_LCD_DrawRect(XOFF-1,YOFF-1,XSIZE+1,YSIZE+1);
  osMutexRelease(myMutex01Handle);

  // Start the conversion process
  status = HAL_ADC_Start_DMA(&hadc1, (uint32_t *)&ADC_Value, BUFFER_MAX);
  if (status != HAL_OK)
  {
	  safe_printf("ERROR: Task 4 HAL_ADC_Start_DMA() %d\n", status);
  }


//  status = HAL_ADC_Stop_DMA(&hadc1);
//  if (status != HAL_OK)
//  {
//	  safe_printf("ERROR: Task 4 HAL_ADC_Stop_DMA() %d\n", status);
//  }
  // Start main loop
  int ave = 0;
  while (1)
  {
	  // Wait for first half of buffer
	  osSemaphoreWait(myBinarySem05Handle, osWaitForever);
	  osMutexWait(myMutex01Handle, osWaitForever);
	  for(i=0;i<(BUFFER_MAX/2);i=i+500)
	  {
		  BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		  BSP_LCD_DrawVLine(XOFF+xpos,YOFF,YSIZE);
		  BSP_LCD_SetTextColor(LCD_COLOR_BLACK);

		  //ypos = map(ADC_Value[i],ave,max,YSIZE/2,YSIZE);
		  //ypos = (ypos>YSIZE)? YSIZE:ypos;
		  ypos=(uint16_t)((uint32_t)((ADC_Value[i]))*YSIZE/(4064));
		  BSP_LCD_DrawLine(XOFF+last_xpos,YOFF+last_ypos,XOFF+xpos,YOFF+ypos);
		  // BSP_LCD_FillRect(xpos,ypos,1,1);
		  last_xpos=xpos;
		  last_ypos=ypos;
		  xpos++;
		  if (last_xpos>=XSIZE-1)
		  {
			  xpos=0;
			  last_xpos=0;
		  }
	  }
	  osMutexRelease(myMutex01Handle);


	  // Wait for second half of buffer
	  osSemaphoreWait(myBinarySem06Handle, osWaitForever);
	  HAL_GPIO_WritePin(GPIOD, LD4_Pin, GPIO_PIN_SET);

	  osMutexWait(myMutex01Handle, osWaitForever);
	  for(i=0;i<(BUFFER_MAX/2);i=i+500)
	  {

		  BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		  BSP_LCD_DrawVLine(XOFF+xpos,YOFF,YSIZE);
		  BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
		  ypos=(uint16_t)((uint32_t)(ADC_Value[i+BUFFER_MAX/2])*YSIZE/(4064));
		  //ypos = map(ADC_Value[i+BUFFER_MAX/2],ave,max,YSIZE/2,YSIZE);
		  //ypos = (ypos>YSIZE)? YSIZE:ypos;
		  BSP_LCD_DrawLine(XOFF+last_xpos,YOFF+last_ypos,XOFF+xpos,YOFF+ypos);
		  // BSP_LCD_FillCircle(xpos,ypos,2);
		  last_xpos=xpos;
		  last_ypos=ypos;
		  xpos++;
		  if (last_xpos>=XSIZE-1){
			  xpos=0;
			  last_xpos=0;
		  }
	  }
	  osMutexRelease(myMutex01Handle);
	  HAL_GPIO_WritePin(GPIOD, LD4_Pin, GPIO_PIN_RESET);
  }
}

// STEPIEN: Add callback functions to see if this can be used for double buffering equivalent

void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc)
{
	osSemaphoreRelease(myBinarySem05Handle);
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	HAL_GPIO_WritePin(GPIOD, LD3_Pin, GPIO_PIN_SET);
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
uint8_t pull_array(char *filename, double *inp_array){
	FIL fobj;														// file object
	FRESULT res;													// result of FATFS functions -> FR_OK = 0 = success
	int time, size;													// data info: time length of sample, size of array
	int byte_size = sizeof(int);									// number of bytes to be read. Initialized as size of integer
	int array_size_bytes, array_size;								// size of array read from file. array_size = num of array elements, array_size_bytes = num of bytes

	/* open file */
	res = f_open(&fobj, filename, FA_READ);							// FATFS function opens file for reading
	if (!res){														// if function failed
		safe_printf("%s failed to open.\n\r", filename);			// print error
		return -1;
	}
	/* reading time*/												// if function was successful read the file with FATFS function
	res = f_read(&fobj, &time, byte_size, &array_size_bytes);		// read first element of file = time
	if(!res){														// if function failed
		safe_printf("%s could not be read.\n\r", filename);			// print error
		return -1;
	}
	if (byte_size != array_size_bytes){								// compare bytes predicted to be read to bytes actually read
		safe_printf("%s could not be read.\n\r", filename);			// if not same print error
		return -1;
	}
	/* reading array size */
	res = f_read(&fobj, &size, byte_size, &array_size_bytes);		// read second element of file = array size
	if(!res){														// if function failed
		safe_printf("%s could not be read.\n\r", filename);			// print error
		return -1;
	}
	if (byte_size != array_size_bytes){								// compare bytes predicted to be read to bytes actually read
		safe_printf("%s could not be read.\n\r", filename);			// if not same print error
		return -1;
	}
	/* reading array */
	byte_size = size*sizeof(double);								// predicted byte size of array = size of array times the size of double
	res = f_read(&fobj, inp_array, byte_size, &array_size_bytes);	// read third element of the file = array of data
	if(!res){														// if function failed
		safe_printf("%s could not be read.\n\r", filename);			// print error
		return -1;
	}
	if (byte_size != array_size_bytes){								// compare bytes predicted to be read to bytes actually read
		safe_printf("%s could not be read.\n\r", filename);			// if not same print error
		return -1;
	}
	array_size = array_size_bytes/sizeof(double);					// array_size_ bytes = num bytes in array. Convert to array_size = num of elements in array
	f_close(&fobj);													// FATFS function closes file
	return array_size;												// return num of elements in array
}
/*******************************************************************************************************/
/* function that reads array from file */
uint8_t write_to_file(char *filename, double *inp_array, int time, int size){
	FIL fobj;														// file object
	FRESULT res;													// result of FATFS functions -> FR_OK = 0 = success
	int byte_size = sizeof(int);									// number of bytes to be read. Initialized as size of integer
	int array_size_bytes, array_size;								// size of array read from file. array_size = num of array elements, array_size_bytes = num of bytes

	/* open file */
		res = f_open(&fobj, filename, FA_READ);						// FATFS function opens file for reading
		if (!res){													// if function failed
			safe_printf("%s failed to open.\n\r", filename);		// print error
			return -1;
		}
	/* writing time to file */										// FATFS function writes to file
	res = f_write(&fobj, &time, byte_size, &array_size_bytes);		// write time to first element in file
	if(!res){														// if function failed
		safe_printf("Could not write to %s.\n\r", filename);		// print error
		return -1;
	}
	if (byte_size != array_size_bytes){								// compare predicted amount of bytes to be written to actual amount written
		safe_printf("Could not write to %s.\n\r", filename);		// if not same print error
		return -1;
	}
	/* writing array size to file */
	res = f_write(&fobj, &size, byte_size, &array_size_bytes);		// writes array size to second file elemenet
	if(!res){														// if function failed
		safe_printf("Could not write to %s.\n\r", filename);		// print error
		return -1;
	}
	if (byte_size != array_size_bytes){								// compare predicted amount of bytes to be written to actual amount written
		safe_printf("Could not write to %s.\n\r", filename);		// if not same print error
		return -1;
	}
	/* writing array to file */
	byte_size = size*sizeof(double);								// predicted byte size of array = size of array times the size of double
	res = f_write(&fobj, inp_array, byte_size, &array_size_bytes);	// write array to third element in file
	if(!res){														// if function failed
		safe_printf("Could not write to file %s.\n\r", filename);	// print error
		return -1;
	}
	if (byte_size != array_size_bytes){								// compare predicted amount of bytes to be written to actual amount written
		safe_printf("Could not write to file %s.\n\r", filename);	// if not same print error
		return -1;
	}
	array_size = array_size_bytes/sizeof(double);					// array size in bytes converted to array size = num of elements
	f_close(&fobj);													// FATFS function closes file
	return array_size;												// returns num of elements in array stored in file
}
