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

#define DIFF_SCALE 180.f
#define DIST_SCALE 85.f
#define DISTANCE_MULTIPLIER 180.f
#define STOP_LIMIT 0.5
#define TURN_LIMIT 1.f

volatile int run = 1;

void shutdown(int __attribute__((__unused__)) sig) {
	run = 0;
	roombath_thread_end();
}

float flabs(float x) {
	if (x > 0.f) return x; else return -x;
}

void * control_thread_func(void __attribute__((__unused__)) * ptr) {
	static int ballfound = 0;
	static float pdiff = 0.0;
	static float search_l = 300.f;
	int xpos = 0, area = 0, width = 0, l_speed = 0, r_speed = 0;
	float diff = 0.f, farea = 0.f;
	float centre = 0.f, distance = 0.f;

	unsigned char l_bump = 0, r_bump = 0; //unused for now.

	while (run) {
		vision_ui_lock_image();
		vision_getframe();
		area = 0;
		//roombath_read_bumps(&l_bump, &r_bump);
		if (l_bump && r_bump) {
			//Hit a wall in front
			printf("Hit a wall in front.\n");
			roombath_direct_drive(-250, -250);
			delay(40);
			roombath_direct_drive(200, -200);
			delay(5);
		}
		else if (l_bump) {
			//Hit a wall on the left
			printf("Hit a wall on left.\n");
			roombath_direct_drive(-250, -250);
			delay(40);
			roombath_direct_drive(200, -200);
			delay(5);
		}
		else if (r_bump) {
			//Hit a wall on the right
			printf("Hit a wall on right.\n");
			roombath_direct_drive(-250, -250);
			delay(40);
			roombath_direct_drive(-200, 200);
			delay(5);
		}
		else if (!image_process(&xpos, &area, &width)) {
			// No ball
			roombath_direct_drive((int)search_l, 40);
			printf("Ball lost.\n");
			ballfound = 0;
			search_l += 1;
			
		}
		else {
			if (ballfound == 0) {
				printf("Ball found!\n");
				search_l = 300;
			}
			//Ball found
			farea = (float)area;
			centre = width / 2.0f;
			diff = 2.0f*(xpos - centre)/width;

			/* I'm not sure how good of an idea this is but it seems to 
			 not break things at least. Basically, the more we need to turn,
			 the less we should be going forward. */
			float offcentreness = 1.f - flabs(diff);
			
			distance = DISTANCE_MULTIPLIER/(sqrt(farea));
			printf("diff: %f\ndisy: %f\n", diff, distance);
			
			if (distance < TURN_LIMIT) {
				l_speed = DIFF_SCALE*diff;
				r_speed = -DIFF_SCALE*diff;
			}
			else if (distance < STOP_LIMIT) {
				l_speed = r_speed = 0;
			}
			else {
				
				l_speed = DIST_SCALE*distance*offcentreness + DIFF_SCALE*diff*diff*diff;
                                r_speed = DIST_SCALE*distance*offcentreness - DIFF_SCALE*diff*diff*diff;
			}

			ballfound = 1;
			roombath_direct_drive(l_speed, r_speed);
		}
		vision_ui_update_values(xpos, area, l_speed, r_speed, distance, ballfound);
		vision_ui_unlock_image();
		delay(50);
		pdiff = diff;
	}
}

int main(int argc, char ** argv)
{
	int ui_only_mode = 0;
	pthread_t roomba_thread, control_thread;

	roombath_init();

	if(readCalibration("calibration.txt")) printf("Calibration file invalid, ignoring...\n");
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
	
	if (ui_only_mode) {
		vision_ui_init(argc, argv);
		vision_ui_run();
	}
	else getchar();
	shutdown(0);
	
	if (!ui_only_mode) pthread_join(roomba_thread, NULL);
	pthread_join(control_thread, NULL);

	roombath_free();
	if (ui_only_mode) vision_ui_quit();

	return 0;
}
