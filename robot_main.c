/*

This program is basically me playing around with threads.
It's also a nice way to remote-control the robot. 

*/

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <signal.h>
#include <math.h>
#include "robot_threaded.h"
#include "robot.h"
#include "vision.h"

#define SCALE 5

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
	static int ballfound = 0;
	
	int xpos, area, width, l_speed, r_speed;
	float diff;
	float offset, centre;
	centre = width / 2.0f;
	diff = 2.0f*(xpos - centre)/width;			
	offset = 1.0f/area;
	l_speed = offset + SCALE*diff;
	r_speed = offset - SCALE*diff;
	
	roombath_direct_drive(500, 100);

	while (run) {
		vision_getframe();
		if (!image_process(&xpos, &area, &width)) {
			//No ball
			if (ballfound) {
				roombath_direct_drive(500, 100);
				printf("Ball lost.\n");
				ballfound = 0;
			}
		}
		else {
			if (ballfound == 0) {
				printf("Ball found!\n");
			}
			//Ball found
			ballfound = 1;
			roombath_direct_drive(l_speed,r_speed);

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
	readCalibration("calibration.txt");	
	vision_init();
	
	pthread_create( &stdio_thread, NULL, &stdio_thread_func, NULL);
	pthread_create( &roomba_thread, NULL, &roomba_thread_func, NULL);
	pthread_create( &control_thread, NULL, &control_thread_func, NULL);
	
	pthread_join( stdio_thread, NULL);
	pthread_join( roomba_thread, NULL);
	pthread_join( control_thread, NULL);

	return 0;
}
