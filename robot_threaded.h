
#ifndef _robot_threaded_h_
#define _robot_threaded_h_

#include "robot.h"

void roombath_direct_drive(int l, int r);
void * roomba_thread_func(void * ptr);

#endif
