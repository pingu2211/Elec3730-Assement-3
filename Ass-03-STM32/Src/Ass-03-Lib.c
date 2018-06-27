//     $Date: 2018-05-16 16:40:22 +1000 (Wed, 16 May 2018) $
// $Revision: 1321 $
//   $Author: Peter $

#include "Ass-03.h"

//
// PLACE YOUR COMMON CODE HERE
//




#define SAMPLE_RATE 1/1000




void lowpass(uint16_t * x, int num_samples, double dt, double freq){
	double RC = 1/(freq*2*PI);
	double a=dt/(RC+dt);
	uint16_t* y = malloc(sizeof(uint16_t)*num_samples);
	y[0]=x[0];
	for (int i = 1;i<num_samples;i++){
		y[i]=y[i-1]+a*(x[i]-y[i-1]);
	}
	x=y;
	return;
}

int calchr(uint16_t * x, int num_samples){
	int max_hr = 300;// bpm
	double f = max_hr * 1/60; //hz
	lowpass(x,num_samples, SAMPLE_RATE, f);
	double ave; // wills ave function
	double stddev; // wills standard deviation func
	int lastedge=0;
	int ave_bpm=0;
	int current_bpm=0;
	int edgecount=0;
	for (int i=1;i<num_samples-1;i++){
		if (x[i+1]-x[i]>stddev){
			if (++edgecount>1){
				current_bpm = (i-lastedge)*SAMPLE_RATE * 60;
				if (current_bpm<max_hr){
					ave_bpm+=current_bpm;
				}else{
					edgecount--;
				}
			}
			lastedge=i;
		}
	}
	return (int)(ave_bpm/edgecount);
}



