#include <stdio.h>
#include "robot.h"
#include "delay.h"


int main()
{
	if(roomba_open(ROOMBA_MODE_SAFE) == -1) {
		fprintf(stderr, "Open failed\nHave you remembered to run $sudo chmod ugo+rw /dev/ttyUSB0 ?\n");
		return 1;
	}
	else
	{
		if(roomba_direct_drive(500, 500) == -1) fprintf(stderr, "Forwards failed\n");
		delay(1000);
		if(roomba_direct_drive(-500, -500) == -1) fprintf(stderr, "Backwards failed\n");
		delay(1000);
		if(roomba_direct_drive(0, 500) == -1) fprintf(stderr, "Turn Left failed\n");
		delay(1000);
		if(roomba_direct_drive(500, 0) == -1) fprintf(stderr, "Turn Right failed\n");
		delay(1000);
		if(roomba_direct_drive(-500, 500) == -1) fprintf(stderr, "Rotate Left failed\n");
		delay(1000);
		if(roomba_direct_drive(500, -500) == -1) fprintf(stderr, "Rotate Right failed\n");
		delay(1000);
		if(roomba_direct_drive(0, 0) == -1) fprintf(stderr, "Stop Drive failed\n");
		if(roomba_close() == -1) fprintf(stderr, "Close failed\n");
	}
	return 0;
}
