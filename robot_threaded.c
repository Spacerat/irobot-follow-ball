
#include "robot_threaded.h"

//TODO: Mutexes?
static int next_command;
static int leftSpeed;
static int rightSpeed;
static int run_thread;
static pthread_mutex_t command_mutex;

void roombath_init() {
	command_mutex = pthread_mutex_init();
}

void roombath_free() {
	pthread_mutex_destroy(command_mutex);
}

void roombath_direct_drive(int l, int r) {
	pthread_mutex_lock(command_mutex);
	leftSpeed = l;
	rightSpeed = r;
	next_command = ROOMBA_DRIVE_DIRECT;
	pthread_mutex_unlock(command_mutex);
}

void roombath_thread_end() {
	pthread_mutex_lock(command_mutex);
	next_command = ROOMBA_END;
	roomba_direct_drive(0, 0);
	run_thread = 0;
	pthread_mutex_unlock(command_mutex);
}

void * roomba_thread_func(void * ptr) {
	run_thread = 1;
	while (run_thread) {
	pthread_mutex_lock(command_mutex);
		switch (next_command) {
			case ROOMBA_DRIVE_DIRECT:
				roomba_direct_drive(leftSpeed, rightSpeed);
				printf("Driving l=%d, r=%d\n", leftSpeed, rightSpeed);
				break;
			case ROOMBA_END:
				roomba_direct_drive(0, 0);
				run_thread = 0;
				break;
		}
		next_command = ROOMBA_CONTINUE;
		pthread_mutex_unlock(command_mutex);
	}
}
