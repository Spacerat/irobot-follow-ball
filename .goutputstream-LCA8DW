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
#include "delay.h"

#define DIFF_SCALE 500.f
#define DIST_SCALE 500.f
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
	roombath_direct_drive(500, 100);

	unsigned char l_bump, r_bump;

	while (run) {
		vision_getframe();
		area = 0;
		roombath_read_bumps(&l_bump, &r_bump);
		if (l_bump || r_bump) {
			//Hit a wall
			roombath_direct_drive(0, 0);
			printf("Hit a wall.\n");
		}
		else if (!image_process(&xpos, &area, &width)) {
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
			centre = width / 2.0f;
			diff = 2.0f*(xpos - centre)/width;	
			distance = DIST_SCALE*(1/(area*area));
			
			l_speed = distance + DIFF_SCALE*diff;
			r_speed = distance - DIFF_SCALE*diff;

			ballfound = 1;
			roombath_direct_drive(l_speed,r_speed);
		}
		delay(10);
	}
}

int main()
{
	roombath_init();

	signal(SIGQUIT, shutdown);
	signal(SIGINT, shutdown);
	
	if(roomba_open(ROOMBA_MODE_FULL) == -1) {
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

	roombath_free();

	return 0;
}
