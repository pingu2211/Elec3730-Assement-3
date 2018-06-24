//     $Date: 2018-05-22 06:24:02 +1000 (Tue, 22 May 2018) $
// $Revision: 1330 $
//   $Author: Peter $

#include "Ass-03.h"

// This is the console task. Can use your code from the previous assignment
// and expand.
//
// The example code is very basic, showing access to the SD card. Note that
// if the SD card is access from both this task and the pulse rate monitor
// task, a MUTEX will be required.
//
// *** MAKE UPDATES TO THE CODE AS REQUIRED ***
//

uint8_t myReadFile();
uint8_t myWriteFile();
FRESULT scan_files (char* path);
FIL MyFile;
FIL MyFile2, MyFile3;
FRESULT Status;

enum CONTROL_CHARS {NUL=0,SOH,STX,ETX,EOT,ENQ,ACK,BEL,BS,TAB,LF,VT,FF,CR,SO,SI,DLE,DC1,DC2,DC3,DC4,NAK,SYN,ETB,CAN,EM,SUB,ESC,FS,GS,RS,US=31,DEL=127};

typedef struct{
int8_t *Command_string; 											// Command string
int8_t (*Function_p)(uint8_t *numbers_p[], uint8_t num_count);		// Function pointer				//
int8_t *Help_s; 													// Help information
} command_s;

int8_t ls(uint8_t *numbers_p[], uint8_t num_count);
int string_parser (char *inp, char **array_of_words_p[]);

bool USR_DBG = true;
const command_s CommandList[] = {												// structure holding list of commands and their help displays.
{"ls", 		&ls, 		"lists the contense of the currentr directory"},							// addition function
{NULL, 			NULL, 		NULL}
};

int Command_Function(int num_count, char **Array_numbers){				// function takes input from user and compares the command with list of commands
	char **Args;
	safe_printf("%s\n",Array_numbers[0]);						// each command references a particular function (add, multiply, etc.)
	if (num_count>1) Args = &Array_numbers[1];
	for (int i=0;CommandList[i].Command_string!=NULL;i++){
		if(strcmp(CommandList[i].Command_string,Array_numbers[0])==0){	// compare input string to command list			// implemented debug messages
			CommandList[i].Function_p(Args,num_count-1);				// reference to function the user has entered.
		}
	}
	return 0;
}

int isControlChar(char c){				// checks input character against the ascii table
	if (c<32||c==127) return true;		// returns true if input is a valid character
	else return false;					// valid characters are 32-127 on ascii table
}



void Ass_03_Task_01(void const * argument)
{
	char * buff;
	char ** words;
	char count;
  safe_printf("Hello from Task 1 - Console (serial input)\n");
  safe_printf("INFO: Initialise LCD and TP first...\n");

  // STEPIEN: Initialize and turn on LCD and calibrate the touch panel
  BSP_LCD_Init();
  BSP_LCD_DisplayOn();
  //BSP_TP_Init();
  //
  // Signal other tasks to start
  osSignalSet(myTask02Handle, 1);
  osSignalSet(myTask03Handle, 1);
  osSignalSet(myTask04Handle, 1);

  // Display welcome message
  osMutexWait(myMutex01Handle, osWaitForever);
  BSP_LCD_Clear(LCD_COLOR_WHITE);
  BSP_LCD_SetFont(&Font12);
  BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
  BSP_LCD_DisplayStringAt(5, 5, (uint8_t*)"ELEC3730 Assignment 3 (v03 $Rev: 1330 $)",LEFT_MODE);
  osMutexRelease(myMutex01Handle);

  while (1)
  {
		buff = malloc(2*sizeof(char));
		if (buff == NULL) printf("error allocating mememory\n");
		int i=0;
		char c=0;
		for (i=0;!(c==CR || c==LF);i++){
			buff = realloc(buff,(i+1)*sizeof(char));
			c=getchar();
			buff[i] = ((c==CR || c==LF))?'\0':c;
			printf("%c\n",buff[i]);
			HAL_GPIO_TogglePin(GPIOD, LD4_Pin); 		// Toggle LED4
		}
		count = string_parser (buff, &words);
		Command_Function(count, words);
		free(buff);
  }

}

void myGetLine(char * buff_out[]){
	char *buff = malloc(2*sizeof(char));
	if (buff == NULL) return;

	int i=0;
	char c=0;
	for (i=0;!(c==CR || c==LF);i++){
		buff = realloc(buff,(i+1)*sizeof(char));
		c=getchar();
		buff[i] = ((c==CR || c==LF))?'\0':c;
		printf("%c\n",buff[i]);
		HAL_GPIO_TogglePin(GPIOD, LD4_Pin); 		// Toggle LED4
	}
	*buff_out = buff;
	return;
}

int string_parser (char *inp, char **array_of_words_p[]){
	int i;
	char ** words;
	words = malloc (1*sizeof(char*));
	    for (int i=0;words[i] != NULL;i++)
	    {
	      words=realloc(words,i*sizeof(char*));
	      if (words!=NULL){
	    	  words[i] = strtok ((i==0)?inp:NULL," ,.-");
	      }else{
	    	  safe_printf("Error alocating memeory\n");
	    	  return 0;
	      }
	    }
	    array_of_words_p= &words;
	    return i+1;
  }


uint8_t myReadFile()
{
#define READ_FILE "Hello.txt"
#define BUFF_SIZE 256
	uint8_t rtext[BUFF_SIZE];
	FRESULT res;
	uint32_t bytesread;

	// Open file Hello.txt
	if((res = f_open(&MyFile, READ_FILE, FA_READ)) != FR_OK)
	{
		safe_printf("ERROR: Opening '%s'\n", READ_FILE);
		return 1;
	}
	safe_printf("Task 1: Opened file '%s'\n", READ_FILE);

	// Read data from file
	if ((res = f_read(&MyFile, rtext, BUFF_SIZE-1, &bytesread)) != FR_OK)
	{
		safe_printf("ERROR: Reading '%s'\n", READ_FILE);
		f_close(&MyFile);
		return 1;
	}
	rtext[bytesread] = '\0';
	safe_printf("Task 1: Read: '%s'\n", rtext);

	// Close file
	f_close(&MyFile);

	return 0;
}

uint8_t myWriteFile()
{
#define WRITE_FILE "There.txt"
	FRESULT res;
	UINT byteswritten;

	// Open file There.txt
	if((res = f_open(&MyFile, WRITE_FILE, FA_CREATE_ALWAYS | FA_WRITE)) != FR_OK)
	{
		safe_printf("ERROR: Opening '%s'\n", WRITE_FILE);
		return 1;
	}
	safe_printf("Task 1: Opened file '%s'\n", WRITE_FILE);

	// Write to file
	if ((res = f_write(&MyFile, "Hello", 6, &byteswritten)) != FR_OK)
	{
		safe_printf("ERROR: Writing '%s'\n", WRITE_FILE);
		f_close(&MyFile);
		return 1;
	}
	safe_printf("Task 1: Written: %d bytes\n", byteswritten);

	// Close file
	f_close(&MyFile);


	return 0;
}




int8_t ls(uint8_t *args_p[], uint8_t num_count){
          FRESULT res;
          DIR dir;
          UINT i;
          static FILINFO fno;
          char * path = (args_p[0]!=NULL)?args_p[0]:"";
          res = f_opendir(&dir, path);                       /* Open the directory */
          if (res == FR_OK) {
              for (;;) {
                  res = f_readdir(&dir, &fno);                   /* Read a directory item */
                  if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
                  if (fno.fattrib & AM_DIR) {                    /* It is a directory */
                      i = strlen(path);
                      safe_printf("%s/%s\t\t\tDIR\n", path, fno.fname);
                  } else {                                       /* It is a file. */
                	  safe_printf("%s/%s\t\t%i Bytes\n", path, fno.fname,fno.fsize);
                  }
              }
              f_closedir(&dir);
          }
          return res;
}

int8_t help(char *args[], uint8_t count){   // help function to display command help messages
	if (USR_DBG)safe_printf("%s\n",args[0]);
	for (int i=0;CommandList[i].Command_string!=NULL;i++){  	// compare the help command

		if(strcmp(CommandList[i].Command_string,args[0])==0){
			safe_printf("\%s\n",CommandList[i].Help_s);
		}
		if (count==0){												// when user types 'help' and no command
			for (int h=0;CommandList[h].Command_string!=NULL;h++){
			safe_printf("\%s\n",CommandList[h].Help_s);	// print ALL command help messages
			}
		}
	}
	return 0;
}
