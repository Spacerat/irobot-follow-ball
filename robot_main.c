/*
robot_main, software for making a Roomba robot following a ball!

Written by Ethan Bishop, Joseph Atkins-Turkish and Tom Bell
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

#define DIFF_SCALE 180.f           //Multiplier for how fast to turn
#define DIST_SCALE 85.f	           //Multiplier for how fast to charge forwards
#define DISTANCE_MULTIPLIER 180.f  //Multiplier for calculating a sensible distance value
#define STOP_LIMIT 2.2             //Distance at which we decide we have reached the ball
#define TURN_LIMIT 2.5             //Distance at which it is sensible to stop moving and just turn instead.

#define SEARCH_MINIMUM 60          //Minimum motor speed when looking for the ball
#define SEARCH_R_START 150.f       //Maximum additional speed to add to the LEFT wheel when looking for the ball.


volatile int run = 1;

/* Signal all threads to terminate */
void shutdown(int sig) {
	run = 0;
	roombath_thread_end();	
	roombath_free();
	signal(sig, SIG_DFL);
	raise(sig);
}

/* Floating point absolute value */
inline float flabs(float x) {
	if (x > 0.f) return x; else return -x;
}

/* Process camera input, tell robot to do things */
void * control_thread_func(void __attribute__((__unused__)) * ptr) {
	static int ballfound = 0;
	static float search_l = SEARCH_R_START;
	int xpos = 0, area = 0, width = 0, l_speed = 0, r_speed = 0;
	float diff = 0.f, farea = 0.f;
	float centre = 0.f, distance = 0.f;

	unsigned char l_bump = 0, r_bump = 0; //unused for now.

	while (run) {
		vision_ui_lock_image();
		vision_getframe();
		area = 0;
		roombath_read_bumps(&l_bump, &r_bump);
		if (l_bump || r_bump) {
			//Hit a wall in front
			printf("Hit a wall in front.\n");
			roombath_direct_drive(-250, -250);
			delay(40);
			roombath_direct_drive(200, -200);
			delay(80);
		}
		/* Commented out because this cases the roomba to get stuck 
		 between chair legs. Not removed because it might be useful
		 in other situations. */
/*
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
*/
		else if (!image_process(&xpos, &area, &width)) {
			// No ball
			roombath_direct_drive(SEARCH_MINIMUM + 10 + (int) search_l, SEARCH_MINIMUM);
			printf("Ball lost.\n");
			ballfound = 0;
			search_l *= 0.99;
			
		}
		else {
			if (ballfound == 0) {
				printf("Ball found!\n");
				search_l = SEARCH_R_START;
			/* This is very important! The webcam has a delay of
			 about one on the input. This means that by the time 
			 our program gets the ball position, we have already
			 turned for a second too long. So, we just turn back! */ 
				roombath_direct_drive(-150, 150);
				delay(400);
			}
			//Ball found! Let's do some control.

			farea = (float)area;
			centre = width / 2.0f;
			//How far accross the screen, (-1 to 1) is the ball?
			diff = 2.0f*(xpos - centre)/width;
			//How far away is the ball?
			distance = DISTANCE_MULTIPLIER/(sqrt(farea));
			printf("diff: %f\ndisy: %f\n", diff, distance);
			
			if (distance < STOP_LIMIT) {
				l_speed = r_speed = 0;
			}	
			else if (distance < TURN_LIMIT) {
				l_speed = DIFF_SCALE*diff;
				r_speed = -DIFF_SCALE*diff;
			}
			else {
				l_speed = DIST_SCALE*distance + DIFF_SCALE*diff;
                                r_speed = DIST_SCALE*distance - DIFF_SCALE*diff;
			}

			ballfound = 1;
			roombath_direct_drive(l_speed, r_speed);
		}
		vision_ui_update_values(xpos, area, l_speed, r_speed, distance, ballfound);
		vision_ui_unlock_image();
		delay(50);
	}
	return 0;
}

/* Here we start all the child threads and set things up.*/
int main(int argc, char ** argv)
{
	int ui_only_mode = 0;
	pthread_t roomba_thread, control_thread;

	roombath_init();

	if(readCalibration("calibration.txt")) printf("Calibration file invalid, ignoring...\n");
	vision_init();

	//Turn the robot off if we kill the program.
	signal(SIGQUIT, shutdown);
	signal(SIGINT, shutdown);
	
	//Figure out whether we run in calibration mode or control mode.
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

	if (ui_only_mode) vision_ui_quit();

	return 0;
}
