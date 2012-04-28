#ifndef _robot_h_
#define _robot_h_

#define ROOMBA_START 128
#define ROOMBA_BAUD 129
#define ROOMBA_CONTROL 130
#define ROOMBA_SAFE 131
#define ROOMBA_FULL 132
#define ROOMBA_SPOT 134
#define ROOMBA_COVER 135
#define ROOMBA_DEMO 136
#define ROOMBA_DRIVE 137
#define ROOMBA_LSD 138
#define ROOMBA_LED 139
#define ROOMBA_SONG 140
#define ROOMBA_PLAY 141
#define ROOMBA_SENSORS 142
#define ROOMBA_COVER_AND_DOCK 143
#define ROOMBA_PWM_LSD 144
#define ROOMBA_DRIVE_DIRECT 145
#define ROOMBA_DIO 147
#define ROOMBA_STREAM 148
#define ROOMBA_QUERY_LIST 149
#define ROOMBA_TOGGLE_STREAM 150
#define ROOMBA_SEND_IR 151
#define ROOMBA_SCRIPT 152
#define ROOMBA_SCRIPT_RUN 153
#define ROOMBA_SCRIPT_SHOW 154
#define ROOMBA_WAIT_TIME 155
#define ROOMBA_WAIT_DISTANCE 156
#define ROOMBA_WAIT_ANGLE 157
#define ROOMBA_WAIT_EVENT 158

#define ROOMBA_MODE_SAFE 0
#define ROOMBA_MODE_FULL 1

#define ROOMBA_END 10
#define ROOMBA_CONTINUE 11

int roomba_open(int mode);
int roomba_close();
int roomba_read(unsigned char buf[], int size);
int roomba_write(unsigned char outbyte);
int roomba_drive(short velocity, short radius);
int roomba_direct_drive(short left_velocity, short right_velocity);
int roomba_led(unsigned char play, unsigned char advance, unsigned char colour, unsigned char intensity);

#endif

