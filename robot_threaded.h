
#ifndef _robot_threaded_h_
#define _robot_threaded_h_

#include "robot.h"

void roombath_init();
void roombath_free();
void roombath_thread_end();
void roombath_direct_drive(int l, int r);
void * roomba_thread_func(void * ptr);
void roombath_read_bumps(unsigned char * l, unsigned char * r);

oendif
