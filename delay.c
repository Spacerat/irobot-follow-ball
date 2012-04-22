#include <time.h>


void delay(int ms)
{
	struct timespec t;
	t.tv_sec = ms / 1000;
	t.tv_nsec = (long)(ms % 1000) * 1000000;
	nanosleep(&t, NULL);
}

