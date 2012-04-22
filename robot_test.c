#include <stdio.h>
#include "robot.h"
#include <time.h>

void delay(int ms)
{
	struct timespec t;
	t.tv_sec = ms / 1000;
	t.tv_nsec = (long)(ms % 1000) * 1000000;
	nanosleep(&t, NULL);
}

int main()
{
	if(roomba_open() == -1) fprintf(stderr, "Open failed");
	if(roomba_direct_drive(500, 500) == -1) fprintf(stderr, "Forwards failed");
	delay(1000);
	if(roomba_direct_drive(-500, -500) == -1) fprintf(stderr, "Backwards failed");
	delay(1000);
	if(roomba_direct_drive(0, 500) == -1) fprintf(stderr, "Turn Left failed");
	delay(1000);
	if(roomba_direct_drive(500, 0) == -1) fprintf(stderr, "Turn Right failed");
	delay(1000);
	if(roomba_direct_drive(-500, 500) == -1) fprintf(stderr, "Rotate Left failed");
	delay(1000);
	if(roomba_direct_drive(500, -500) == -1) fprintf(stderr, "Rotate Right failed");
	delay(1000);
	if(roomba_direct_drive(0, 0) == -1) fprintf(stderr, "Stop Drive failed");
	if(roomba_close() == -1) fprintf(stderr, "Close failed");
	return 0;
}
