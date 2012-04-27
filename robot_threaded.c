
#include "robot_threaded.h"

//TODO: Mutexes?
static int next_command;
static int leftSpeed;
static int rightSpeed;
static int run_thread;

void roombath_direct_drive(int l, int r) {
	next_command = ROOMBA_DRIVE_DIRECT;
	leftSpeed = l;
	rightSpeed = r;
}

void roombath_thread_end() {
	next_command = ROOMBA_END;
}

void * roomba_thread_func(void * ptr) {
	run_thread = 1;
	while (run_thread) {
		switch (next_command) {
			case ROOMBA_DRIVE_DIRECT:
				roomba_direct_drive(leftSpeed, rightSpeed);
				break;
			case ROOMBA_END:
				run_thread = 0;
				break;
		}
		next_command = ROOMBA_CONTINUE;
	}

}
