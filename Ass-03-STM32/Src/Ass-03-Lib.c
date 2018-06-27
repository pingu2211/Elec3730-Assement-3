//     $Date: 2018-05-16 16:40:22 +1000 (Wed, 16 May 2018) $
// $Revision: 1321 $
//   $Author: Peter $

#include "Ass-03.h"

//
// PLACE YOUR COMMON CODE HERE
//

#define SAMPLE_RATE 1/1000

// a low pass filtering function. takes an int 16 array, number of samples, time betweeen samples and the frequency cut off in hz
void lowpass(uint16_t ** x, int num_samples, double dt, double freq){
	double RC = 1/(freq*2*PI);										// convert frequency to radians per second
	double a=dt/(RC+dt);											// calculate alpha
	uint16_t* y = malloc(sizeof(uint16_t)*num_samples);				// assign an output array
	y[0]=*x[0];														// start the filter
	for (int i = 1;i<num_samples;i++){
		y[i]=y[i-1]+a*(*x[i]-y[i-1]);								// filter each element in the old array
	}
	*x=y;															// return the output array
	return;
}

int calchr(uint16_t * x, int num_samples){
	int edge =  0;
	int max_hr = 300;						// bpm
	double f = max_hr * 1/60; 				//bpm -> hz
	lowpass(x,num_samples, SAMPLE_RATE, f);	 // low pass the
	double ave; 							// wills ave function
	double stddev; 							// wills standard deviation func
	int lastedge=0;							// keep a track of the samples between edges
	int ave_bpm=0;							// avreage the bpm over a period of time
	int current_bpm=0;
	int edgecount=0;
	for (int i=1;i<num_samples-1;i++){
		if (x[i+1]-x[i]>stddev && edge <1 ){			// an edge occurs when the current sample is larger than the previous sample by more than the first standard deviation
			edge = 1;
			if (++edgecount>1){
				current_bpm = (i-lastedge)*SAMPLE_RATE * 60;			//if we've seen more than one edge, calculate BPM from current edge to the previous edge
				if (current_bpm<max_hr){								// we can expect to see more than one "edge" per heart beat
					ave_bpm+=current_bpm;								//
				}else{
					edgecount--;
				}
			}
			lastedge=i;
		}else if(x[i+1]-x[i]<stddev && edge == 1){
			edge = -1;
		}else{
			edge = -1;
		}
	}
	return (int)(ave_bpm/edgecount);
}

void F_ErrorInterp(FRESULT code){
	if (code == FR_OK){safe_printf("The function succeeded.");}
	else if (code == FR_DISK_ERR){safe_printf("Disk Error\n\r");}
	else if (code == FR_INT_ERR ){safe_printf("Assertion failed.\n\rThere is an error of the FAT structure on the volume.\n\rWrong lower layer implementation.");}
	else if (code == FR_NOT_READY){safe_printf("Not Ready Error.\n\rNo medium in the drive or Wrong formatting type\n\r");}
	else if (code == FR_NO_FILE){safe_printf("Could not find the file.\n\r");}
	else if (code == FR_NO_PATH){safe_printf("Could not find the path.\n\r");}
	else if (code == FR_INVALID_NAME){safe_printf("The given string is invalid as the path name.\n\rEither there is an invalid character or the string is out of 8.3 format\n\r");}
	else if (code == FR_DENIED){safe_printf("The required access was denied due to one of the following reasons:\n\rWrite mode open against the read-only file.\n\rDeleting the read-only file or directory.\n\rDeleting the non-empty directory or current directory.\n\rOr disk full.\n\r");}
	else if (code == FR_EXIST){safe_printf("A file/folder with the same name already exists.");}
	else if (code == FR_INVALID_OBJECT){safe_printf("The file/directory object is invalid or it has been closed, or collapsed.\n\rWas The Physical Media removal.\n\r");}
	else if (code == FR_WRITE_PROTECTED){safe_printf("Tried to write to a write-protected file.");}
	else if (code == FR_INVALID_DRIVE){safe_printf("Invalid drive number is specified in the path name.");}
	else if (code == FR_NOT_ENABLED){safe_printf("Work area for the logical drive has not been registered by f_mount function.");}
	else if (code == FR_NO_FILESYSTEM){safe_printf("There is no valid FAT volume on the drive");}
	else if (code == FR_MKFS_ABORTED){safe_printf("It is impossible to format with the given parameters.\n\rOR\n\rThe size of volume is too small. 128 sectors minimum with FM_SFD.\n\rOR\n\rThe partition bound to the logical drive coulud not be found.");}
	else if (code == FR_TIMEOUT){safe_printf("The function was canceled due to a timeout of thread-safe control");}
	else if (code == FR_LOCKED){safe_printf("The operation to the object was rejected by file sharing control");}
	else if (code == FR_NOT_ENOUGH_CORE){safe_printf("Not enough memory for the operation.");}
	else if (code == FR_TOO_MANY_OPEN_FILES){safe_printf("Number of open objects has been reached maximum value and no more object can be opened. ");}
	else if (code == FR_INVALID_PARAMETER){safe_printf("The given parameter is invalid or there is an inconsistent for the volume.");}
	else{
		safe_printf("Uspecified FATFS error");
	}
}


