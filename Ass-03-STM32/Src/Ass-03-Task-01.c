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
void F_ErrorInterp(FRESULT code);
FIL MyFile;
FIL MyFile2, MyFile3;
FRESULT Status;

enum CONTROL_CHARS {NUL=0,SOH,STX,ETX,EOT,ENQ,ACK,BEL,BS,TAB,LF,VT,FF,CR,SO,SI,DLE,DC1,DC2,DC3,DC4,NAK,SYN,ETB,CAN,EM,SUB,ESC,FS,GS,RS,US=31,DEL=127};
#define MAX_PATH_LENGTH 100
#define TABSTOP 8

typedef struct{
int8_t *Command_string; 											// Command string
int8_t (*Function_p)(uint8_t *args_p[], uint8_t args_count);		// Function pointer				//
int8_t *Help_s; 													// Help information
} command_s;

int8_t ls(uint8_t *args_p[], uint8_t args_count);
int8_t mkdir(uint8_t *args_p[], uint8_t args_count);
int8_t analog(uint8_t *args_p[], uint8_t args_count);
int8_t cd(uint8_t *args_p[], uint8_t args_count);
int8_t clear(uint8_t *args_p[], uint8_t args_count);
int8_t mov(uint8_t *args_p[], uint8_t args_count);
int8_t cp(uint8_t *args_p[], uint8_t args_count);
int8_t debug(uint8_t *args_p[], uint8_t args_count);
int8_t help(uint8_t *args_p[], uint8_t args_count);
int8_t path(uint8_t *args_p[], uint8_t args_count);
int8_t rm(uint8_t *args_p[], uint8_t args_count);
int8_t CL_start(uint8_t *args_p[], uint8_t args_count);
int8_t CL_stop(uint8_t *args_p[], uint8_t args_count);
int8_t CL_load(uint8_t *args_p[], uint8_t args_count);
int8_t CL_store(uint8_t *args_p[], uint8_t args_count);
int8_t CL_mem1(uint8_t *args_p[], uint8_t args_count);
int8_t CL_mem2(uint8_t *args_p[], uint8_t args_count);
int8_t CL_mem3(uint8_t *args_p[], uint8_t args_count);
int8_t CL_mem4(uint8_t *args_p[], uint8_t args_count);
int8_t CL_mem5(uint8_t *args_p[], uint8_t args_count);
bool validPath(char * path);
int string_parser (char *inp, char **array_of_words_p[]);

const command_s CommandList[] = {								// structure holding list of commands and their help displays.
{"analog", 		&analog, 	"Plot the analog input for the given period of time."},
{"ls", 			&ls, 		"List contents of current folder"},
{"cd", 			&cd, 		"Change current directory"},
{"mkdir", 		&mkdir, 	"Create new folder"},
{"cp",			&cp,		"Copies selected file to destination"},
{"mov", 		&mov, 		"Move selected file to selected location"},
{"rm", 			&rm, 		"Deletes selected file"},
{"debug", 		&debug,		"Turns debug messages on and off"},	// debug messages on or off
{"help",       	&help,		"Show help messages"},
{"clear",       &clear,     "clears the terminal"},
{"stop",			&CL_stop,			"stops adc display conversion (equivelent to touch panel button)"},
{"start",			&CL_start,			"starts adc display (equivelent to touch panel button)"},
{"load",			&CL_load,			"loads adc display from file (equivelent to touch panel button)"},
{"store",			&CL_store,			"stores adc display to file(equivelent to touch panel button)"},
{"mem1",			&CL_mem1,			"set working memory to mem1 (equivelent to touch panel button)"},
{"mem2",			&CL_mem2,			"set working memory to mem2 (equivelent to touch panel button)"},
{"mem3",			&CL_mem3,			"set working memory to mem3 (equivelent to touch panel button)"},
{"mem4",			&CL_mem4,			"set working memory to mem3 (equivelent to touch panel button)"},
{"mem5",			&CL_mem5,			"set working memory to mem3 (equivelent to touch panel button)"},
{NULL, 			NULL, 		NULL}
};


int Command_Function(int args_count, char **Array_numbers[]){				// function takes input from user and compares the command with list of commands
	char **Args;
	if (args_count>1) Args = &Array_numbers[1];
	for (int i=0;CommandList[i].Command_string!=NULL;i++){
		if(strcmp(CommandList[i].Command_string,Array_numbers[0])==0){	// compare input string to command list			// implemented debug messages
			CommandList[i].Function_p(Args,args_count-1);// reference to function the user has entered.
			return 0;
		}
	}
	safe_printf("%s not found",Array_numbers[0]);
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
	char workingdir[MAX_PATH_LENGTH];
	safe_printf("Hello from Task 1 - Console (serial input)\n\r");
	safe_printf("INFO: Initialise LCD and TP first...\n\r");
	FRESULT Result;
	USR_DEBUG=false;
  // STEPIEN: Initialize and turn on LCD and calibrate the touch panel
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
  int i=0;
  char c=0;
  while (1)
  {
	c=0;
	buff = malloc(2*sizeof(char));
	if (buff == NULL){
		safe_printf("ERROR ALLOCATING MEMORY\n\r");
		while(1);
	}

	Result = f_getcwd(workingdir, MAX_PATH_LENGTH);
	if (Result == FR_OK){
		safe_printf("0:%s>",workingdir);
	}else{
		safe_printf("0:/>");
	}
	for (i=0;!(c==CR || c==LF);i++){
		buff = realloc(buff,(i+1)*sizeof(char));
		c=getchar();
		if (c==BS||c==FF){
			buff[--i]='\0';
			safe_printf("%c %c",BS,BS);
			continue;
		}
		//safe_printf("%i",c);
		buff[i] = ((c==CR || c==LF))?'\0':c;
		if(!isControlChar(buff[i]))safe_printf("%c",buff[i]);
		HAL_GPIO_TogglePin(GPIOD, LD4_Pin); 		// Toggle LED4
	}
	safe_printf("\n\r");
	count = string_parser (buff, &words);
	if (count>0)Command_Function(count, words);
	if(USR_DEBUG){safe_printf("String Passer Count %i\n\r");}
	free(buff);
	free(words);
  }

}

int string_parser(char *inp, char **array_of_words_p[])
{
	//Check if parsed string is valid, doesn't equal null and is not empty
	if(inp == NULL || strlen(inp) == 0)
	{
		return -1;
	}
	//Count number of words by increasing word_c each time a space character  is reached after a normal character
	int i = 0;
	int word_c = 1;
	while(inp[i] == ' ')
	{
		i++;
	}
	while('\0' != inp[i] && '\n' != inp[i])
	{
		if (inp[i] == ' ' && inp[i+1] != ' ' && inp[i+1] != '\0' && inp[i+1] != '\n\r')
		{
			word_c ++;
		}
		i++;
	}
	//Create the array_of_words which holds pointers to the word strings
	char ** array_of_words;
	array_of_words = (char**) malloc(word_c*sizeof(char*));
	if(array_of_words == NULL)
	{
		safe_printf("ERROR - error in Malloc 5\n\r");
		return -1;
	}
	/*
	 * the next block of code stores each word in 'tempstring' and this is used to
	 * allocate the right amount of space in the array_of_words and then the contents
	 * of 'tempstring' is copied into the array_of_words
	 */

	int word_start = 0;
	i = 0;
	int k = 0; //local loop counter
	int p = 0; //local loop counter
	int character_c = 0;
	char* tempstring = NULL;
	char* temp_pointer = NULL;
	//while p is less than number of words counted previously
	while(p < word_c)
	{
		//increase i to the start of the word that isnt a space
		while(inp[word_start] == ' ')
		{
			word_start++;
		}
		//count number of characters in the word
		i = word_start;
		while(inp[i] != ' ' && inp[i] != '\0' && inp[i] != '\n')
		{
			character_c ++;
			i++;
		}
		//Allocate memory for tempstring according to number of characters counted + 1 for null character

		temp_pointer = (char*) realloc(tempstring,(character_c+1)*sizeof(char));
		if(temp_pointer == NULL)
		{
			safe_printf("ERROR - error in Malloc 6\n\r");
			return -1;
		}
		tempstring = temp_pointer;
		temp_pointer = NULL;
		//copy each character of the word to the tempstring
		i = word_start;
		k = 0;
		while(k < character_c)
		{
			tempstring[k] = inp[i];
			k++;
			i++;
		}
		//add null character to end of string to make a valid null terminated string
		tempstring[k] = '\0';
		//array_of_words[] is then allocated memory the same size of the current 'tempstring' and the contents of tempstring are copied into the array_of_words[]
		array_of_words[p] = (char*) malloc(sizeof(char)*(strlen(tempstring)+1));
		if(array_of_words[p] == NULL)
		{
			safe_printf("ERROR - error in Malloc 7\n\r");
			return -1;
		}
		strcpy(array_of_words[p], tempstring);
		word_start = word_start + character_c;
		k = 0;
		character_c = 0;

		p++;
	}

	*array_of_words_p = array_of_words; //put array of words into array of words pointer
	free(tempstring); //free malloc'd memory
	return (word_c);  //return word count
}

int8_t ls(uint8_t *args_p[], uint8_t args_count){
          FRESULT res;
          DIR dir;
          uint8_t pathlen,namelen;
          static FILINFO fno;
          char * path = (args_count<0)?args_p[0]:"";
          res = f_opendir(&dir, path);                       /* Open the directory */
          if (res == FR_OK) {
              for (;;) {
                  res = f_readdir(&dir, &fno);                   /* Read a directory item */
                  if (res != FR_OK){
                	  F_ErrorInterp(res);
                	  break;  /* Break on error or end of dir */
                  }else if(fno.fname[0] == 0){
                	  break;
                  }
                  pathlen = strlen(path);
                  namelen = strlen(fno.fname);
                  safe_printf("%s",fno.fname);
                  for (int t=0;t < 5-(namelen/TABSTOP);t++){
                	  safe_printf("\t");
                  }
                  if (fno.fattrib & AM_DIR) {                    /* It is a directory */
                      safe_printf("(DIR)\n\r");
                  } else {                                       /* It is a file. */
                	  safe_printf("(%i Bytes)\n\r",fno.fsize);
                  }
              }
              f_closedir(&dir);
          }else{
        	  F_ErrorInterp(res);
          }
          return res;
}


int8_t help(uint8_t *args[], uint8_t count){   // help function to display command help messages
	if(USR_DEBUG){safe_printf("%s\n\r",args[0]);}
	if (count==0){												// when user types 'help' and no command
			for (int h=0;CommandList[h].Command_string!=NULL;h++){
			safe_printf("%s\t\t%s\n\r",CommandList[h].Command_string,CommandList[h].Help_s);	// print ALL command help messages
			}
		}else{
		for (int i=0;CommandList[i].Command_string!=NULL;i++){  	// compare the help command
			if(strcmp(CommandList[i].Command_string,args[0])==0){
				safe_printf("\%s\n\r",CommandList[i].Help_s);
			}
		}
	}
return 0;
}


/*******************************************************************************************************/
bool isNumber(char * str){												// checks input against ascii table
	for (int i=0;i<strlen(str);i++){									// makes sure input is a number
		if ( (str[i] < 48 || str[i] >57)&&!(str[i]==45||str[i]==46) ){
			if(USR_DEBUG){safe_printf("is not a number\n\r");}					// print debug messages
			return false;
		}
	}
	if (USR_DEBUG){printf("\n\r|%s|string is |%lf|double\n\r",str,atof(str));}
	return true;
}
/*******************************************************************************************************/
int8_t clear(uint8_t *args[], uint8_t count){	// function clears the terminal window
	printf("\n\rclear\n\r");
	printf("\e[1;1H\e[2J");
	return 0;
}

/*******************************************************************************************************/
int8_t debug(uint8_t *args[], uint8_t count){		// function that is used to turn debug messages on and off
	if (count==0)USR_DEBUG=!USR_DEBUG;				// if user enters only debug (1 word), switch debug status. ie if ON then switch to OFF
	if (count==1){
		if (strcmp(args[1],"on")==0||strcmp(args[1],"ON")==0){	// checks if user entered the command 'debug on (or ON)'
			USR_DEBUG=true;										// if so, turn debug messages on
		}else{
			USR_DEBUG=false;										// if user types anything else, ie 'debug off'
		}														// switch debug messages off
	}
	if (USR_DEBUG){printf("\n\rDEBUG ON\n\r");}		// print ON or OFF when debug message settings are switched
	else printf("\n\rDEBUG OFF\n\r");
	return 0;
}
/*******************************************************************************************************/
int8_t analog(uint8_t *args_p[],uint8_t args_count){
	int number;
	struct DSP_LCD_CONTROL *Control;
	if(args_count==0){
		printf("Must enter command followed by a single number.");		// if no numbers are entered return error message
		return -1;
	}
	if (args_count>2){
		printf("Too many arguments entered. Only one number is necessary.");	// if more than one number is entered return error message.
		return -1;
	}

	if (!isNumber(args_p[0])){						// if input is not a number
			printf("Arguments must be real numbers");	// return a message to user that the input was not a number
			return -1;
	}else{
			number=atof(args_p[0]);					// take the input number that is a string of characters and convert to a double
			if (number<0){
				printf("Argument must be a positive number - time cannot be negative");
				return -1;
			}else{
				safe_printf("TX time: %d", number*SECONDS_TO_MILLI);
				Control= osPoolAlloc(LCD_ControlPool);
				Control->run = (strcmp("-c",args_p[1])==0) ? CONTINUOUS:ONE_SHOT;
				if(USR_DEBUG){safe_printf("Starting ADC in %s mode\n\r", (Control->run==CONTINUOUS)?"CONTINUOUS":"ONE SHOT");}
				Control->time = number*SECONDS_TO_MILLI;
				if(USR_DEBUG){safe_printf("Running for %i Milisecconds \n\r", Control->time);}
				osMessagePut(LCD_ControlMsg, Control, osWaitForever);
			}
		}
	return 0;
	}
/*******************************************************************************************************/

/*******************************************************************************************************/
int8_t cd(uint8_t *args_p[],uint8_t args_count){
		FRESULT res;
	    DIR dir;
	    char * path = (args_p[0]!=NULL)?args_p[0]:"";
	    res = f_chdir(path);
	    if (res != FR_OK) {
	    	F_ErrorInterp(res);
	    } 	else {
	   		safe_printf("%s\n\r",path);
	   	}
	    return res;
}
/*******************************************************************************************************/
int8_t mkdir(uint8_t *args_p[],uint8_t args_count){
		FRESULT res;
		char * path = (args_p[0]!=NULL)?args_p[0]:"";
		if (!validPath(path)){
			safe_printf("Invalid Folder Name, folders cannot contain ,.'\"~`!@^*|\\");
			return -1;
		}
		res = f_mkdir(path);
		if (res != FR_OK){
			safe_printf("Error occurred. Unable to create directory.\n\r");
			F_ErrorInterp(res);
		} 	else {
	    		safe_printf("Folder Created: %s\n\r",path);
		}
		return res;
}
/*******************************************************************************************************/
int8_t mov(uint8_t *args_p[],uint8_t args_count){
	 FRESULT res;
	 DIR dir;
	 char * path_old = args_p[0];
	 char * path_new = args_p[1];
	 	 if (validPath(path_new)){							//check if the new path is a valid path
			 res = f_rename(path_old, path_new);			//moves the file
			 if (res != FR_OK){
				 safe_printf("Error Occurred. Could not copy %s to %s.", path_old, path_new);		// Generate error messages
				 F_ErrorInterp(res);
				 return res;
			 } else {
				 safe_printf("%s -> %s",path_old,path_new);											// make pretty sucess msgs
				 return res;
			 }
	 	 }else{
	 		 return FR_INVALID_NAME;
	 	 }
}


/*******************************************************************************************************/
int8_t cp(uint8_t *args_p[], uint8_t args_count){ 		// Copy selected file to selected location
    FIL f_source, f_dest;     										// File objects
    BYTE buffer[4096];   								// File copy buffer
    FRESULT res;          								// FatFs function common result code
    UINT br, bw;         								// File read/write count
    DIR dir;
    FILINFO info;
    char f_source_extension[4];
    char * scratch_source;
	char new_dest[255];
   	char * source = args_p[0];
   	char * dest = args_p[1];
   	 	 if (validPath(dest)){							//check if the new path is a valid path
   	 		res = f_stat(dest,&info);
   	 		if (res != FR_OK || !(info.fattrib & AM_DIR)){
   	 			res = f_open(&f_source, source, FA_READ);	 	// Open source file// FATFS function opens file
   	 			if (res != FR_OK){							// if function fails return error
   	 				return -1;
   	 			}
   	 			/*edit path*/
				scratch_source = strrchr(source,'.');
				if (scratch_source == NULL){
					strcat(new_dest,source);
					strcat(new_dest,"_C");
				}else{
					strncpy(new_dest,source,(scratch_source-source));
					new_dest[(source-scratch_source)]='\0';
					strcat(new_dest,"_C");
					strcat(new_dest,scratch_source);
				}

   	 			/* Create destination file */
   	 			res = f_open(&f_dest, new_dest, FA_WRITE | FA_CREATE_ALWAYS);	// FATFS function opens file
   	 			if (res != FR_OK){											// if function fails return error
   	 				return -1;
   	 			}
   	 			res = f_open(&f_source, source, FA_READ);
   	 			if (res != FR_OK){											// if function fails return error
   	 				return -1;
   	 			}
   	 			/* Copy source to destination */
   	 			for (;;) {
   	 				res = f_read(&f_source, buffer, sizeof buffer, &br);  	// Read a chunk of source file
   	 				if (res || br == 0){ break; 							// error or eof
   	 				}
   	 				res = f_write(&f_dest, buffer, br, &bw);            		// Write it to the destination file
   	 				if (res || bw < br){ break; 							// error or disk full
   	 				}
   	 			}
   	 			safe_printf("%s does not exist. %s copied in current directory.\n\r", dest, source);
   	 		}else{
				res = f_open(&f_source, source, FA_READ);	 	// Open source file - FATFS function opens file
				if (res != FR_OK){								// if function fails return error
					return -1;
				}
				/* Create destination file */
				scratch_source = strrchr(source,'/');
				if (scratch_source == NULL){
					strcpy(new_dest,dest);
					strcat(new_dest,"/");
					strcat(new_dest,source);
				}else{
					strcpy(new_dest,dest);
					strcat(new_dest,"/");
					strcat(new_dest,scratch_source+1);
				}
				safe_printf("copying to %s",new_dest);
				res = f_open(&f_dest, new_dest, FA_WRITE | FA_CREATE_ALWAYS);	// FATFS function opens file
				if (res != FR_OK){												// if function fails return error
					return -1;
				}

				/* Copy source to destination */
				for (;;) {
					res = f_read(&f_source, buffer, sizeof buffer, &br);  	// Read a chunk of source file
					if (res || br == 0){ break; 							// error or eof
					}
					res = f_write(&f_dest, buffer, br, &bw);            	// Write it to the destination file
					if (res || bw < br){ break; 							// error or disk full
					}
				}
				safe_printf("%s copied to %s\n\r", source, dest)
			 }
   	 	 }
    /* Close open files */
    f_close(&f_source);
    f_close(&f_dest);

    return res;
}
/*******************************************************************************************************/
int8_t rm(uint8_t *args_p[],uint8_t args_count){
	FILINFO * info;
	FRESULT res;
	char * path = args_p[0];

	res = f_stat(path,info);						// FATfs function checks if the file exists
	if (res == FR_INVALID_NAME){					// print error if file doesnt exist
		safe_printf("%s does not exist\n\r",path);
		F_ErrorInterp(res);
		return -1;
	}
	res = f_unlink (args_p[0]);						// FATfs function deletes a file
	if (res==FR_OK){
		safe_printf("Successfully removed %s", path);
	}else if(res == FR_DENIED){						// if unsuccessful due to folder not being empty
			printf("Could not Remove %s: DIR not empty",path);
	}else{
		safe_printf("Could not Remove %s", path);	// generate error messages from error code
		F_ErrorInterp(res);
	}
	return 0;
}
/*******************************************************************************************************/

bool validPath(char * path){			// checks path for invalid characters, returns true if path is ok
	char * invalidchars= ",.'\"~`!@^*|\\";	// list of invalid characters
	for (int i=0; i < strlen(invalidchars);i++){
		if (strchr(path,invalidchars[i]))return false;		// step through every invalid charaacter, if any are found return 0
	}
	return true;
}

/*******************************************************************************************************/
int8_t CL_start(uint8_t *args_p[],uint8_t args_count){
	Start();
	return 0;
}

/*******************************************************************************************************/
int8_t CL_stop(uint8_t *args_p[],uint8_t args_count){
	Stop();
	return 0;
}

/*******************************************************************************************************/
int8_t CL_load(uint8_t *args_p[],uint8_t args_count){
	Load();
	return 0;
}

/*******************************************************************************************************/
int8_t CL_store(uint8_t *args_p[],uint8_t args_count){
	Store();
	return 0;
}

/*******************************************************************************************************/
int8_t CL_mem1(uint8_t *args_p[],uint8_t args_count){
	MEM1();
	return 0;
}
/*******************************************************************************************************/
int8_t CL_mem2(uint8_t *args_p[],uint8_t args_count){
	MEM2();
	return 0;
}
/*******************************************************************************************************/
int8_t CL_mem3(uint8_t *args_p[],uint8_t args_count){
	MEM3();
	return 0;
}
int8_t CL_mem4(uint8_t *args_p[],uint8_t args_count){
	MEM4();
	return 0;
}
int8_t CL_mem5(uint8_t *args_p[],uint8_t args_count){
	MEM5();
	return 0;
}



