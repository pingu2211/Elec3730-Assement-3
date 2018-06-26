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
void F_ErrorIterp(FRESULT code);
FIL MyFile;
FIL MyFile2, MyFile3;
FRESULT Status;
bool USR_DBG = false;

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
int8_t cp(uint8_t *args_p[], uint8_t args_count);
int8_t debug(uint8_t *args_p[], uint8_t args_count);
int8_t help(uint8_t *args_p[], uint8_t args_count);
int8_t path(uint8_t *args_p[], uint8_t args_count);
int8_t rm(uint8_t *args_p[], uint8_t args_count);
bool validPath(char * path);
int string_parser (char *inp, char **array_of_words_p[]);

const command_s CommandList[] = {								// structure holding list of commands and their help displays.
{"analog", 		&analog, 	"Plot the analog input for the given period of time."},
{"ls", 			&ls, 		"List contents of current folder"},
{"cd", 			&cd, 		"Change current directory"},
{"mkdir", 		&mkdir, 	"Create new folder"},
{"cp", 			&cp, 		"Copy selected file to selected location"},
{"rm", 			&rm, 		"Deletes selected file"},
{"debug", 		&debug,		"Turns debug messages on and off"},	// debug messages on or off
{"help",       	&help,		"Show help messages"},
{"clear",       &clear,     "clears the terminal"},
{NULL, 			NULL, 		NULL}
};


int Command_Function(int args_count, char **Array_numbers[]){				// function takes input from user and compares the command with list of commands
	char **Args;
	if (args_count>1) Args = &Array_numbers[1];
	for (int i=0;CommandList[i].Command_string!=NULL;i++){
		if(strcmp(CommandList[i].Command_string,Array_numbers[0])==0){	// compare input string to command list			// implemented debug messages
			CommandList[i].Function_p(Args,args_count-1);				// reference to function the user has entered.
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

int string_parser(char *inp, char **array_of_words_p[])
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
                	  F_ErrorIterp(res);
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
        	  F_ErrorIterp(res);
          }
          return res;
}


int8_t help(uint8_t *args[], uint8_t count){   // help function to display command help messages
	if (USR_DBG)safe_printf("%s\n\r",args[0]);
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
			if (USR_DBG)printf("is not a number\n\r");					// print debug messages
			return false;
		}
	}
	if (USR_DBG)printf("\n\r|%s|string is |%lf|double\n\r",str,atof(str));
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
	if (count==0)USR_DBG=!USR_DBG;				// if user enters only debug (1 word), switch debug status. ie if ON then switch to OFF
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
int8_t analog(uint8_t *args_p[],uint8_t args_count){
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
		if (number<0){
			printf("Argument must be a positive number - time cannot be negative");
			return -1;
		}
	}
/*******************************************************************************************************/

/*******************************************************************************************************/
int8_t cd(uint8_t *args_p[],uint8_t args_count){
		FRESULT res;
	    DIR dir;
	    char * path = (args_p[0]!=NULL)?args_p[0]:"";

	    res = f_chdir(path);
	    if (res != FR_OK) {
	    	F_ErrorIterp(res);
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
			return 0;
		}
		res = f_mkdir(path);
		if (res != FR_OK){
			safe_printf("Error occurred. Unable to create directory.\n\r");
			F_ErrorIterp(res);
		} 	else {
	    		safe_printf("Folder Created: %s\n\r",path);
		}
		return res;
}
/*******************************************************************************************************/
int8_t cp(uint8_t *args_p[],uint8_t args_count){
	 FRESULT res;
	 DIR dir;
	 char * path_old = args_p[0];
	 char * path_new = args_p[1];
	 	 if (validPath(path_new)){							//check if the new path is a valid path
			 res = f_rename(path_old, path_new);			//moves the file
			 if (res != FR_OK){
				 safe_printf("Error Occurred. Could not copy %s to %s.", path_old, path_new);		// Generate error messages
				 F_ErrorIterp(res);
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
int8_t rm(uint8_t *args_p[],uint8_t args_count){
	FILINFO * info;
	FRESULT res;
	char * path = args_p[0];

	res = f_stat(path,info);						// FATfs function checks if the file exists
	if (res == FR_INVALID_NAME){					// print error if file doesnt exist
		safe_printf("%s does not exist\n\r",path);
		F_ErrorIterp(res);
		return 0;
	}
	res = f_unlink (args_p[0]);						// FATfs function deletes a file
	if (res==FR_OK){
		safe_printf("successfuly removed %s", path);
	}else if(res == FR_DENIED){						// if unsuccessful due to folder not being empty
			printf("Could not Remove %s: DIR not empty",path);
	}else{
		safe_printf("could not removed %s", path);	// generate error messages from error code
		F_ErrorIterp(res);
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

void F_ErrorIterp(FRESULT code){
	if (code == FR_OK){safe_printf("The function succeeded.");}
	else if (code == FR_DISK_ERR){safe_printf("The lower layer, disk_read, disk_write or disk_ioctl function, reported that an unrecoverable hard error occured.");}
	else if (code == FR_INT_ERR ){safe_printf("Assertion failed. An insanity is detected in the internal process. One of the following possibilities is suspected.\n\rWork area (file system object, file object or etc...) has been broken by stack overflow or any other tasks. This is the reason in most case.\nThere is an error of the FAT structure on the volume.\n\rThere is a bug in the FatFs module itself.\n\rWrong lower layer implementation.");}
	else if (code == FR_NOT_READY){safe_printf("The lower layer, disk_initialize function, reported that the storage device could not be got ready to work. One of the following possibilities is suspected.\n\rNo medium in the drive.\n\rWrong lower layer implementation.\n\rWrong hardware configuration.\n\rThe storage device has been broken.");}
	else if (code == FR_NO_FILE){safe_printf("Could not find the file.");}
	else if (code == FR_NO_PATH){safe_printf("Could not find the path.");}
	else if (code == FR_INVALID_NAME){safe_printf("The given string is invalid as the path name. One of the following possibilities is suspected.\n\rThere is any character not allowed for the file name.\n\rThe string is out of 8.3 format. (at non-LFN cfg.)\n\rFF_MAX_LFN is insufficient for the file name. (at LFN cfg.)\n\rThere is any character encoding error in the string.");}
	else if (code == FR_DENIED){safe_printf("The required access was denied due to one of the following reasons:\n\rWrite mode open against the read-only file.\n\rDeleting the read-only file or directory.\n\rDeleting the non-empty directory or current directory.\n\rReading the file opened without FA_READ flag.\n\rAny modification to the file opened without FA_WRITE flag.\n\rCould not create the object due to root directory full or disk full.\n\rCould not allocate a contiguous area to the file.");}
	else if (code == FR_EXIST){safe_printf("Name collision. An object with the same name is already existing.");}
	else if (code == FR_INVALID_OBJECT){safe_printf("The file/directory object is invalid or a null pointer is given. There are some reasons as follows:\n\r	It has been closed, or collapsed.\n\rPhysical drive is not ready to work due to a media removal.\n\r");}
	else if (code == FR_WRITE_PROTECTED){safe_printf("A write mode operation against the write-protected media.");}
	else if (code == FR_INVALID_DRIVE){safe_printf("Invalid drive number is specified in the path name. A null pointer is given as the path name. (Related option: FF_VOLUMES)");}
	else if (code == FR_NOT_ENABLED){safe_printf("Work area for the logical drive has not been registered by f_mount function.");}
	else if (code == FR_NO_FILESYSTEM){safe_printf("There is no valid FAT volume on the drive or wrong lower layer implementation.");}
	else if (code == FR_MKFS_ABORTED){safe_printf("The f_mkfs function aborted before start in format due to a reason as follows:\n\r	It is impossible to format with the given parameters.\n\rThe size of volume is too small. 128 sectors minimum with FM_SFD.\n\rThe partition bound to the logical drive coulud not be found. (Related option: FF_MULTI_PARTITION)");}
	else if (code == FR_TIMEOUT){safe_printf("The function was canceled due to a timeout of thread-safe control. (Related option: FF_TIMEOUT)");}
	else if (code == FR_LOCKED){safe_printf("The operation to the object was rejected by file sharing control. (Related option: FF_FS_LOCK)");}
	else if (code == FR_NOT_ENOUGH_CORE){safe_printf("Not enough memory for the operation.");}
	else if (code == FR_TOO_MANY_OPEN_FILES){safe_printf("Number of open objects has been reached maximum value and no more object can be opened. (Related option: FF_FS_LOCK)");}
	else if (code == FR_INVALID_PARAMETER){safe_printf("The given parameter is invalid or there is an inconsistent for the volume.");}
	else{
		safe_printf("Uspecified FATFS error");
	}

}
