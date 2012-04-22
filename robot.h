#ifndef _robot_h_
#define _robot_h_

int roomba_open();
int roomba_close();
int roomba_read(unsigned char buf[], int size);
int roomba_write(unsigned char outbyte);
int roomba_drive(short velocity, short radius);
int roomba_direct_drive(short left_velocity, short right_velocity);
int roomba_led(unsigned char play, unsigned char advance, unsigned char colour, unsigned char intensity);

#endif