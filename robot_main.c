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
	int xpos = 0, area = 0, width = 0, l_speed = 0, r_speed = 0;
	float diff = 0.f, farea = 0.f;
	float centre = 0.f, distance = 0.f;
	
	unsigned char l_bump = 0, r_bump = 0;
	//delay(5000);


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
			//No ball
			if (ballfound || 1) {
				roombath_direct_drive(300, 40);
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
			float offcentreness = 1.f - abs(diff);
			//farea -= 700.f;
			//farea = farea/(5000.f);
			distance = 180.f/(sqrt(farea));
			printf("diff: %f\ndisy: %f\n", diff, distance);
			/*
			if (distance > 1.1f) {
				if (diff < 0.1 && diff > -0.1) {
					l_speed = r_speed = 200.f;
					
				}
				else {
					l_speed = 100.f * diff;  - 100.f * flabs(diff - pdiff);
					r_speed = - 100.f * diff;  - 100.f * flabs(diff - pdiff);
				}
			}	
			else l_speed = r_speed = 0; 
			*/
			
			if (distance < 1.f) {
				l_speed = DIFF_SCALE*diff;
				r_speed = -DIFF_SCALE*diff;
			}
			else if (distance < 0.5) {
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
		delay(10);
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
