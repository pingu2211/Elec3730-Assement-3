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
int false = 0;
int true = !0;
int count = 0;
bool USR_DBG = false;
/*******************************************************************************************************/
/* function declarations for Q1 */
int8_t ls(uint8_t *numbers_p[], uint8_t num_count);
int8_t ls(uint8_t *args_p[], uint8_t args_count);
int8_t mkdir(uint8_t *args_p[], uint8_t args_count);
int8_t analog(uint8_t *args_p[], uint8_t args_count);
int8_t cd(uint8_t *args_p[], uint8_t args_count);
int8_t clear(uint8_t *args_p[], uint8_t args_count);
int8_t cp(uint8_t *args_p[], uint8_t args_count);
int8_t debug(uint8_t *args_p[], uint8_t args_count);
int8_t help(uint8_t *args_p[], uint8_t args_count);
int8_t path(uint8_t *args_p[], uint8_t args_count);
int8_t rm(uint8_t *args_p[], uint8_t args_count);
int string_parser (char *inp, char **array_of_words_p[]);
/*******************************************************************************************************/
/* command structure*/
enum CONTROL_CHARS {NUL=0,SOH,STX,ETX,EOT,ENQ,ACK,BEL,BS,TAB,LF,VT,FF,CR,SO,SI,DLE,DC1,DC2,DC3,DC4,NAK,SYN,ETB,CAN,EM,SUB,ESC,FS,GS,RS,US=31,DEL=127};
#define MAX_PATH_LENGTH 100
typedef struct{
int8_t *Command_string; 											// Command string
int8_t (*Function_p)(uint8_t *args_p[], uint8_t args_count);		// Function pointer				//
int8_t *Help_s; 													// Help information
} command_s;

const command_s CommandList[] = {		// structure holding list of commands and their help displays.
{"analog", 		&analog, 	"Plot the analog input for the given period of time. Input looks like: analog <time>"},	// plots analog input for given time
{"ls", 			&ls, 		"List contents of current folder"},				// Lists contents of current folder/directory
{"cd", 			&cd, 		"Change current directory. Input looks like: cd <directory>"},	// Change current directory
{"mkdir", 		&mkdir, 	"Create new folder. Input looks like: mkdir <directoty>"},		// make new folder/directory
{"cp", 			&cp, 		"Copy selected file to selected location. Input looks like: cp <file> <directory>"},	// copy file to new location
{"rm", 			&rm, 		"Deletes selected file. Input looks like: rm <file>"},						// delete selected file
{"debug", 		&debug,		"Turns debug messages on and off. Input looks like: debug <on>||<off>"},	// debug messages on or off
{"help",       	&help,		"Show help messages. Input looks like: help <command>"},					// display help message
{"clear",       &clear,     "clears the terminal"},			// clears terminal and the console
{NULL, 			NULL, 		NULL}
};
/*******************************************************************************************************/
int Command_Function(int args_count, char **Array_numbers[]){				// function takes input from user and compares the command with list of commands
	char **Args;
	if (args_count>1) Args = &Array_numbers[1];
	for (int i=0;CommandList[i].Command_string!=NULL;i++){
		if(strcmp(CommandList[i].Command_string,Array_numbers[0])==0){	// compare input string to command list			// implemented debug messages

			// CommandList[i].Function_p(Args,num_count-1);				// reference to function the user has entered.

			CommandList[i].Function_p(Args,args_count-1);				// reference to function the user has entered.

		}
	}
	return 0;
}
/*******************************************************************************************************/
//int Command_Function(int arg_count, char **Array_numbers){				// function takes input from user and compares the command with list of commands
//	if (USR_DBG)printf("%s\n",Array_numbers[0]);						// each command references a particular function (add, multiply, etc.)
//	char **Args = &Array_numbers[1];
//	for (int i=0;CommandList[i].Command_string!=NULL;i++){
//		if(strcmp(CommandList[i].Command_string,Array_numbers[0])==0){	// compare input string to command list
//			if (USR_DBG)printf("first arg = %s\n",Args[0]);				// implemented debug messages
//			CommandList[i].Function_p(Args,arg_count-1);				// reference to function the user has entered.
//		}
//	}
//	return 0;
//}
/*******************************************************************************************************/
void Ass_03_Task_01(void const * argument)
{


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
  myWriteFile();
  while (1)
  {
  buff = malloc(2*sizeof(char));
	if (buff == NULL){
		safe_printf("ERROR ALLOCATING MEMORY\n\r");
		return;
	}
	int i=0;
	char c=0;
	Result = f_getcwd(workingdir, MAX_PATH_LENGTH);
	if (Result == FR_OK){
		safe_printf("\n\r%s>",workingdir);
	}else{
		safe_printf("\n\r>");
	}
	for (i=0;!(c==CR || c==LF);i++){
		buff = realloc(buff,(i+1)*sizeof(char));
		c=getchar();
		if (c==BS){
			buff[--i]='\0';
			i--;
			safe_printf("%c %c",BS,BS);
			continue;
		}
		buff[i] = ((c==CR || c==LF))?'\0':c;
		safe_printf("%c",buff[i]);
		HAL_GPIO_TogglePin(GPIOD, LD4_Pin); 		// Toggle LED4
	}
	safe_printf("\n\r");
	count = string_parser (buff, &words);
	if (count>0)Command_Function(count, words);
	free(buff);
	free(words);
  }

}

void myGetLine(char ** buff){
	buff = malloc(sizeof(char*));
	buff = malloc(2*sizeof(char));
	if (buff == NULL){
		safe_printf("ERROR ALLOCATING MEMORY\n\r");
		return;
	}

	int i=0;
	char c=0;
	for (i=0;!(c==CR || c==LF);i++){
		*buff = realloc(*buff,(i+1)*sizeof(char));
		c=getchar();
		if (c==BS){
			*buff[i]='\0';
			i--;
			continue;
		}
		*buff[i] = ((c==CR || c==LF))?'\0':c;
		safe_printf("%c",*buff[i]);
		HAL_GPIO_TogglePin(GPIOD, LD4_Pin); 		// Toggle LED4
	}
	return;
}
/*******************************************************************************************************/
int string_parser(char *inp, char **array_of_words_p[])		// string parser function
{
	//Check if parsed string is valid, doesn't equal null and is not empty
	if(inp == NULL || strlen(inp) == 0)
	{
		safe_printf("ERROR - NULL String \n\r");
		return -1;
	}
	//Count number of words by increasing word_c each time a space character  is reached after a normal character
	int i = 0;
	int word_c = 1;
	while(inp[i] == ' ')
	{
		i++;
	}
	while('\0' != inp[i] && '\n\r' != inp[i])
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
		return 0;
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
		while(inp[i] != ' ' && inp[i] != '\0' && inp[i] != '\n\r')
		{
			character_c ++;
			i++;
		}
		//Allocate memory for tempstring according to number of characters counted + 1 for null character

		temp_pointer = (char*) realloc(tempstring,(character_c+1)*sizeof(char));
		if(temp_pointer == NULL)
		{
			safe_printf("ERROR - error in Malloc 6\n\r");
			return 0;
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
			return 0;
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
/*******************************************************************************************************/
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
		safe_printf("ERROR: Opening '%s'\n\r", READ_FILE);
		return 1;
	}
	safe_printf("Task 1: Opened file '%s'\n\r", READ_FILE);

	// Read data from file
	if ((res = f_read(&MyFile, rtext, BUFF_SIZE-1, &bytesread)) != FR_OK)
	{
		safe_printf("ERROR: Reading '%s'\n\r", READ_FILE);
		f_close(&MyFile);
		return 1;
	}
	rtext[bytesread] = '\0';
	safe_printf("Task 1: Read: '%s'\n\r", rtext);

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
		safe_printf("ERROR: Opening '%s'\n\r", WRITE_FILE);
		return 1;
	}
	safe_printf("Task 1: Opened file '%s'\n\r", WRITE_FILE);

	// Write to file
	if ((res = f_write(&MyFile, "Hello", 6, &byteswritten)) != FR_OK)
	{
		safe_printf("ERROR: Writing '%s'\n\r", WRITE_FILE);
		f_close(&MyFile);
		return 1;
	}
	safe_printf("Task 1: Written: %d bytes\n\r", byteswritten);

	// Close file
	f_close(&MyFile);
	return 0;
}

/*******************************************************************************************************/
int8_t help(uint8_t *args[], uint8_t count){   	// help function to display command help messages
	if (USR_DBG)safe_printf("%s\n\r",args[0]);
	if (count==0){												// when user types 'help' and no command
			for (int h=0;CommandList[h].Command_string!=NULL;h++){
			safe_printf("%s\t\t%s\n\r",CommandList[h].Command_string,CommandList[h].Help_s);	// print ALL command help messages
			}
		}else{
		for (int i=0;CommandList[i].Command_string!=NULL;i++){
			if(strcmp(CommandList[i].Command_string,args[0])==0){	// compare argument to the list of commands
				safe_printf("\%s\n\r",CommandList[i].Help_s);		// print the help message linked with the command in the structure
			}
		}
	}
return 0;
}


/*******************************************************************************************************/
bool isNumber(char * str){												// checks input against ascii table
	for (int i=0;i<strlen(str);i++){									// makes sure input is a number
		if ( (str[i] < 48 || str[i] >57)&&!(str[i]==45||str[i]==46) ){
			if (USR_DBG)printf("is not a number\n\r");					// print debug messages
			return false;
		}
	}
	if (USR_DBG)printf("\n\r|%s|string is |%lf|double\n\r",str,atof(str));	// print debug message
	return true;
}
/*******************************************************************************************************/
int8_t clear(uint8_t *args[], uint8_t count){	// function clears the terminal window

	printf("\n\rclear\n\r");	// clears terminal
	printf("\e[1;1H\e[2J");		// clears console
	return 0;
}

/*******************************************************************************************************/
int8_t debug(uint8_t *args[], uint8_t count){		// function that is used to turn debug messages on and off
	if (count==0)USR_DBG=!USR_DBG;					// if user enters only debug (1 word), switch debug status. ie if ON then switch to OFF
	if (count==1){
		if (strcmp(args[1],"on")==0||strcmp(args[1],"ON")==0){	// checks if user entered the command 'debug on (or ON)'
			USR_DBG=true;										// if so, turn debug messages on
		}else{
			USR_DBG=false;										// if user types anything else, ie 'debug off'
		}														// switch debug messages off
	}
	if (USR_DBG)printf("\n\rDEBUG ON\n\r");		// print ON or OFF when debug message settings are switched
	else printf("\n\rDEBUG OFF\n\r");
	return 0;
}
/*******************************************************************************************************/
int8_t analog(uint8_t *args_p[],uint8_t args_count){	// plot the analog input data on the touch panel for a given time step
	int number;
	if (sizeof(args_count)>1){
		printf("Too many arguments entered. Only one number is necessary.");	// if more than one number is entered return error message.
		return -1;
	}
	if(sizeof(args_count)==0){
		printf("Must enter command followed by a single number.");		// if no numbers are entered return error message
		return -1;
	}
		if (!isNumber(args_p[0])){						// if input is not a number
			printf("Arguments must be real numbers");	// return a message to user that the input was not a number
			return -1;
		}else{
			number=atof(args_p[0]);					// take the input number that is a string of characters and convert to a double
		}
		if (number<0){								// if the input number is negative
			printf("Argument must be a positive number - time cannot be negative");	// print error - time must be a positive number
			return -1;
		}

}
/*******************************************************************************************************/
int8_t ls(uint8_t *args_p[], uint8_t args_count){		// function lists the contents of the current directory
          FRESULT res;									// result of FATfs functions -> FR_OK = 0 = success
          DIR dir;										// current directory
          UINT i;
          static FILINFO fno;
          char * path = (args_count<0)?args_p[0]:"";			// pointer to path entered by user
          res = f_opendir(&dir, path);                       	// Open the directory
          if (res == FR_OK) {									// if open function was successful
              for (;;) {
                  res = f_readdir(&dir, &fno);                  // Read a directory item
                  if (res != FR_OK || fno.fname[0] == 0) break; // Break on error or end of dir
                  if (fno.fattrib & AM_DIR) {                   // if it is a directory
                      i = strlen(path);
                      safe_printf("%s/%s\t\t\tDIR\n\r\n\r", path, fno.fname);
                  } else {                                      // if  it is a file.
                	  safe_printf("%s\t\t%i Bytes\n\r\n\r", fno.fname,fno.fsize);
                  }
              }
              f_closedir(&dir);
          }
          return res;		// return result res = FR_OK = 0 OR res = false
}
/*******************************************************************************************************/
int8_t cd(uint8_t *args_p[],uint8_t args_count){						// function changes the current directory to a new directory entered by the user
		FRESULT res;													// result of FATfs functions -> FR_OK = 0 = success
	    char * path = (args_p[0]!=NULL)?args_p[0]:"";					// pointer to path entered by the user
	    res = f_chdir(path);											// FATfs function that changes current directory
	    if (res != FR_OK) {												// if function was not successful
	    	safe_printf("Error occurred. Directory not found.\n\r");	// print error
	    } 	else {														// if function was successful
	   		safe_printf("%s>\n\r",path);								// print the new directory name
	   	}
	    return res;
}
/*******************************************************************************************************/
int8_t mkdir(uint8_t *args_p[],uint8_t args_count){			// function creates new folder/directory
		FRESULT res;										// result of FATfs functions -> FR_OK = 0 = success
		char * path = (args_p[0]!=NULL)?args_p[0]:"";		// pointer to path entered by the user
		res = f_mkdir(path);								// FATfs function creates new folder
		if (res != FR_OK){									// if function was not successful
			safe_printf("Error occurred. Unable to create directory.\n\r");	// print error
		} 	else {											// if function was successful
	    		safe_printf("New Folder: %s\n\r",path);		// print new folder name
		}
		return res;
}
/*******************************************************************************************************/
int8_t cp(uint8_t *args_p[],uint8_t args_count){		// function copies a selected file to a selected location
	 FRESULT res;										// result of FATfs functions -> FR_OK = 0 = success
	 DIR dir;
	 char * path_old = (args_p[0]!=NULL)?args_p[0]:"";	// pointer to old path
	 char * path_new = (args_p[1]!=NULL)?args_p[1]:"";	// pointer to new path
	 char * new_name;
	 res = f_stat(path_new);
	 if (res != FR_OK){
		 new_name = strcat(path_old,(path_old)2);
		 res = frename(path_old,new_name);
		 if (res != FR_OK){
			 safe_printf("Error Occurred. Could not copy file.");
			 return -1;
		 } else {
			 safe_printf("Destination could not be found. File %s copied to current directory.\n\r New File: %s\n\r", path_old, new_name);
			 return res;
		 }
	 }else {
		 res = f_rename(path_old, path_new);			// FATfs function copies file to location
		 if (res != FR_OK){								// if copy was not successful
			 safe_printf("Error Occurred. Could not copy %s to %s.\n\r", path_old, path_new);
			 return -1;
		 } else {
			 safe_printf("File %s copied to %s\n\r",path_old, path_new);
			 return res;
		 }

	}
	return 0;
}

/*******************************************************************************************************/
int8_t rm(uint8_t *args_p[],uint8_t args_count){	// function deletes a selected file
	FRESULT res;									// result of FATfs functions -> FR_OK = 0 = success
	char * path = (args_p[0]!=NULL)?args_p[0]:"";	// pointer to path entered by the user
	res = f_stat(path);								// FATfs function checks if the file exists
	if (res == FR_INVALID_NAME){					// if invalid name is entered
		safe_printf("%s does not exist\n\r",path);	// print error
		return -1;
	} else {
		if (res == FR_OK){							// if file exists
			res = f_unlink (path);					// FATfs function deletes a file
				if (res==FR_OK){					// if function successful
					safe_printf("%s deleted.\n\r", path);	// print deleted file
					return res;
				}else{										// if unsuccessfully deleted
					safe_printf("%s could not be deleted.\n\r", path);	// print error
					return -1;
				}
		}
	}

	return 0;
}
/*******************************************************************************************************/
