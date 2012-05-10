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

#define DIFF_SCALE 50.f
#define DIST_SCALE 1.f
volatile int run = 1;

void shutdown(int __attribute__((__unused__)) sig) {
	run = 0;
	roombath_thread_end();
}

void * control_thread_func(void __attribute__((__unused__)) * ptr) {
	static int ballfound = 0;
	
	int xpos = 0, area = 0, width = 0, l_speed = 0, r_speed = 0;
	float diff = 0.f, farea = 0.f;
	float centre = 0.f, distance = 0.f;
	roombath_direct_drive(500, 100);

	unsigned char l_bump = 0, r_bump = 0;

	while (run) {
		vision_ui_lock_image();
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
			farea = (float)area;
			centre = width / 2.0f;
			diff = 2.0f*(xpos - centre)/width;
			farea -= 700.f;
			farea = farea/(10000.f-700.f);
			distance = 1.f/(farea*farea);
			
			l_speed = DIST_SCALE*distance + DIFF_SCALE*diff;
			r_speed = DIST_SCALE*distance - DIFF_SCALE*diff;

			ballfound = 1;
			roombath_direct_drive(l_speed, r_speed);
		}
		vision_ui_update_values(xpos, area, l_speed, r_speed, distance, ballfound);
		vision_ui_unlock_image();
		delay(10);
	}
}

int main(int argc, char ** argv)
{
	int ui_only_mode = 0;
	pthread_t roomba_thread, control_thread;

	roombath_init();
	vision_ui_init(argc, argv);

	readCalibration("calibration.txt");
	vision_init();

	signal(SIGQUIT, shutdown);
	signal(SIGINT, shutdown);
	
	if (roomba_open(ROOMBA_MODE_FULL) == -1) {
		fprintf(stderr, "Open failed. Running in UI-only mode...\n");
		ui_only_mode = 1;
	}
	else {
		pthread_create(&roomba_thread, NULL, &roomba_thread_func, NULL);
	}

	pthread_create(&control_thread, NULL, &control_thread_func, NULL);
	
	vision_ui_run();

	shutdown(0);
	
	if (!ui_only_mode) pthread_join(roomba_thread, NULL);
	pthread_join(control_thread, NULL);

	roombath_free();
	vision_ui_quit();

	return 0;
}
