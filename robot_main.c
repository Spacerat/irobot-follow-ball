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
#include "vision_ui.h"

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
	float offset, centre, distance;
	roombath_direct_drive(500, 100);

	unsigned char l_bump, r_bump;

	while (run) {
		vision_getframe();
		area = 0;
		roombath_read_bumps(&l_bump, &r_bump);
		if (l_bump && r_bump) {
			//Hit a wall in front
			printf("Hit a wall in front.\n");
			roombath_direct_drive(-250, -250);
			delay(40);
			roombath_direct_drive(500, -500);
			delay(5);
		}
		else if (l_bump) {
			//Hit a wall on the left
			printf("Hit a wall on left.\n");
			roombath_direct_drive(-250, -250);
			delay(40);
			roombath_direct_drive(500, -500);
			delay(5);
		}
		else if (r_bump) {
			//Hit a wall on the right
			printf("Hit a wall on right.\n");
			roombath_direct_drive(-250, -250);
			delay(40);
			roombath_direct_drive(-500, 500);
			delay(5);
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
			area -= 700;
			area = area/10000;
			distance = DIST_SCALE*(1.f/(area*area));
			
			l_speed = DIFF_SCALE*diff;
			r_speed = -DIFF_SCALE*diff;

			ballfound = 1;
			roombath_direct_drive(l_speed,r_speed);
		}
		delay(10);
	}
}

int main(int argc, char ** argv)
{
	roombath_init();
	vision_ui_init(argc, argv);

	signal(SIGQUIT, shutdown);
	signal(SIGINT, shutdown);
	
	//if(roomba_open(ROOMBA_MODE_FULL) == -1) {
	//	fprintf(stderr, "Open failed. Check the USB cable!\nHave you remembered to run $sudo chmod ugo+rw /dev/ttyUSB0 ?\n");
//		return 1;
//	}
	pthread_t stdio_thread, roomba_thread, control_thread, vision_ui_thread;
	readCalibration("calibration.txt");
	vision_init();
	//pthread_create( &stdio_thread, NULL, &stdio_thread_func, NULL);
	//pthread_create( &roomba_thread, NULL, &roomba_thread_func, NULL);
	pthread_create( &control_thread, NULL, &control_thread_func, NULL);
	//pthread_create( &vision_ui_thread, NULL, &vision_ui_thread_func, NULL);
	
	vision_ui_thread_func(NULL);

	shutdown(0);
	
	//pthread_join( vision_ui_thread, NULL);
	//pthread_join( stdio_thread, NULL);
	//pthread_join( roomba_thread, NULL);
	pthread_join( control_thread, NULL);

	roombath_free();
	vision_ui_quit();

	return 0;
}
