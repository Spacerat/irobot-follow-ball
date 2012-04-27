
#ifndef _roomba_threaded_h
#define _roomba_threaded_h

#include "robot.h"

void roombath_direct_drive(int l, int r);
void * roomba_thread_func(void * ptr);

#endif
