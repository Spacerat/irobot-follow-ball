/*

This program is basically me playing around with threads.
It's also a nice way to remote-control the robot. 

*/

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include  <signal.h>
#include "robot_threaded.h"
#include "robot.h"
#include "vision.h"

volatile int run = 1;


void shutdown(int sig) {
	run = 0;
	roombath_thread_end();
}

void * stdio_thread_func(void * ptr) {
	char command [10];
	
	while (run) {
		printf("> ");
		scanf("%s", command);
		if (strcmp("q", command) == 0) {
			shutdown(0);
		}
		printf("\n");
	}
}



void * control_thread_func(void * ptr) {
	int xpos, area;
	static int ballfound = 0;
	
	while (run) {
		vision_getframe();
		if (image_process(&xpos, &area)) {
			//No ball
			if (ballfound) {
				roombath_direct_drive(1000,100);
				printf("Ball lost.\n");
				ballfound = 0;
			}
		}
		else {
			//Ball found
			ballfound = 1;
			roombath_direct_drive(100,100);

		}
		
	}
}

int main()
{
	signal(SIGQUIT, shutdown);
	signal(SIGINT, shutdown);
	
	if(roomba_open(ROOMBA_MODE_SAFE) == -1) {
		fprintf(stderr, "Open failed. Check the USB cable!\nHave you remembered to run $sudo chmod ugo+rw /dev/ttyUSB0 ?\n");
		return 1;
	}
	pthread_t stdio_thread, roomba_thread, control_thread;
	
	vision_init();
	
	pthread_create( &stdio_thread, NULL, &stdio_thread_func, NULL);
	pthread_create( &roomba_thread, NULL, &roomba_thread_func, NULL);
	pthread_create( &control_thread, NULL, &control_thread_func, NULL);
	
	pthread_join( stdio_thread, NULL);
	pthread_join( roomba_thread, NULL);
	pthread_join( control_thread, NULL);

	return 0;
}
