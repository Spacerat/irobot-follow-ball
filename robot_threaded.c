#include <pthread.h>
#include <stdio.h>
#include "robot_threaded.h"

static int next_command;
static int leftSpeed;
static int rightSpeed;
static unsigned char * leftBump;
static unsigned char * rightBump;
static int run_thread;
static pthread_mutex_t command_mutex;
static char bumpsRead;

void roombath_init() {
	pthread_mutex_init(&command_mutex, NULL);
}

void roombath_free() {
	pthread_mutex_destroy(&command_mutex);
}

void roombath_direct_drive(int l, int r) {
	pthread_mutex_lock(&command_mutex);
	leftSpeed = l;
	rightSpeed = r;
	next_command = ROOMBA_DRIVE_DIRECT;
	pthread_mutex_unlock(&command_mutex);
}

void roombath_read_bumps(unsigned char * l, unsigned char * r) {
	pthread_mutex_lock(&command_mutex);
	bumpsRead = 0;
	leftBump = l;
	rightBump = r;
	next_command = ROOMBA_SENSORS;
	pthread_mutex_unlock(&command_mutex);
	while(bumpsRead) {}
	bumpsRead = 0;
}

void roombath_thread_end() {
	pthread_mutex_lock(&command_mutex);
	next_command = ROOMBA_END;
	roomba_direct_drive(0, 0);
	run_thread = 0;
	pthread_mutex_unlock(&command_mutex);
}

void * roomba_thread_func(void * ptr) {
	run_thread = 1;
	while (run_thread) {
	pthread_mutex_lock(&command_mutex);
		switch (next_command) {
			case ROOMBA_DRIVE_DIRECT:
				roomba_direct_drive(leftSpeed, rightSpeed);
				printf("Driving l=%d, r=%d\n", leftSpeed, rightSpeed);
				break;
			case ROOMBA_SENSORS:
				roomba_read_bumps(leftBump, rightBump);
				printf("Bumps report: l=%d, r=%d\n", leftSpeed, rightSpeed);
				bumpsRead = 1;
				break;
			case ROOMBA_END:
				roomba_direct_drive(0, 0);
				run_thread = 0;
				break;
		}
		next_command = ROOMBA_CONTINUE;
		pthread_mutex_unlock(&command_mutex);
	}
}
