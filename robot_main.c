/*

This program is basically me playing around with threads.
It's also a nice way to remote-control the robot. 

*/

#include <stdio.h>
#include "robot_threaded.h"
#include "vision.h"
#include <string.h>
#include <pthread.h>
#include <stdlib.h>

volatile int run = 1;



#define goleft 1
#define goright 2
#define goforward 3
#define goback 4
#define gowait 5
#define goquit 6
void * stdio_thread_func(void * ptr) {
	char command [10];
	
	while (run) {
		printf("> ");
		if (strcmp("q", command) == 0) {
			run = 0;
			roombath_thread_end();
		}
		printf("\n");
	}
}



void * control_thread_func(void * ptr) {
	int xpos, area;
	while (run) {
		image_update();
		if (image_process(&xpos, &area)) {
			//No ball
			roombath_direct_drive(1000,400);
		}
		else {
			//ball found
		}
		
	}
}

int main()
{
	if(roomba_open(ROOMBA_MODE_SAFE) == -1) {
		fprintf(stderr, "Open failed. Check the USB cable!\nHave you remembered to run $sudo chmod ugo+rw /dev/ttyUSB0 ?\n");
		return 1;
	}
	pthread_t stdio_thread, roomba_thread, control_thread;
	
	pthread_create( &stdio_thread, NULL, &stdio_thread_func, NULL);
	pthread_create( &roomba_thread, NULL, &roomba_thread_func, NULL);
	pthread_create( &control_thread, NULL, &control_thread_func, NULL);
	
	pthread_join( stdio_thread, NULL);
	pthread_join( roomba_thread, NULL);
	pthread_join( control_thread, NULL);

	return 0;
}
