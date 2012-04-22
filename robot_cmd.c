/*

This program is basically me playing around with threads.
It's also a nice way to remote-control the robot. 

*/

#include <stdio.h>
#include "robot.h"
#include "cbuffer.h"
#include <string.h>
#include <pthread.h>
#include <stdlib.h>

volatile int run = 1;

CircularBuffer commands;

#define goleft 1
#define goright 2
#define goforward 3
#define goback 4
#define gowait 5
#define goquit 6
void * stdio_thread_func(void * ptr) {
	char command [10];
	int delaytime;
	
	while (run) {
		printf("> ");
		delaytime = 0;
		scanf("%s %d", command, &delaytime);
		
		if (strcmp("l", command) == 0) {
			cbWrite(&commands, goleft);
			printf("Go left\n");
		}
		else if (strcmp("r", command) == 0) {
			cbWrite(&commands, goright);
			printf("Go right\n");
		}
		else if (strcmp("f", command) == 0) {
			cbWrite(&commands, goforward);
			printf("Go forward\n");
		}
		else if (strcmp("b", command) == 0) {
			cbWrite(&commands, goback);
			printf("Go backward\n");
		}
		else if (strcmp("q", command) == 0) {
			cbWrite(&commands, goquit);
		}
		
		if (delaytime > 0) {
			cbWrite(&commands, gowait);
			cbWrite(&commands, delaytime);
		}
		
		printf("\n");
	}
}

void * roomba_thread_func(void * ptr) {
	while (run) {
		if (!cbIsEmpty(&commands)) {
			buff_t dat;
			cbRead(&commands, &dat);
			switch (dat) {
				case goleft:
					roomba_direct_drive(-1000, 1000);
					break;
				case goright:
					roomba_direct_drive(1000, -1000);
					break;
				case goforward:
					roomba_direct_drive(1000, 1000);
					break;
				case goback:
					roomba_direct_drive(-1000, -1000);
					break;
				case goquit:
					run = 0;
					break;
				case gowait:
					cbRead(&commands, &dat);
					delay(dat);
					roomba_direct_drive(0, 0);
					break;
			}
		}
	}
}

int main()
{
	if(roomba_open(ROOMBA_MODE_SAFE) == -1) {
		fprintf(stderr, "Open failed. Check the USB cable!\nHave you remembered to run $sudo chmod ugo+rw /dev/ttyUSB0 ?\n");
		return 1;
	}
	cbInit(&commands, 30);
	pthread_t stdio_thread, roomba_thread;
	
	pthread_create( &stdio_thread, NULL, &stdio_thread_func, NULL);
	pthread_create( &roomba_thread, NULL, &roomba_thread_func, NULL);
	
	
    pthread_join( stdio_thread, NULL);
    pthread_join( roomba_thread, NULL);
	
	return 0;
}
