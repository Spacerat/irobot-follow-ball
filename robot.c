/* robot2.c to control an iRobot via a USB serialport 
 * Compile with gcc -o robot2 -Wall robot2.c
 * Command line parameters can be any ssequence of
   N    A decimal integer in the range 0...255. Send this byte to the robot
   pN   Flush all output bytes and pause N miliseconds
   rN   Read N bytes from the robot and print them on a line
*/

#include "robot.h"
#include "delay.h"
#include <stdlib.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h> 
//#include <errno.h>
//#include <error.h>

#include <stdio.h>


#define DEVICE "/dev/ttyUSB0"
#define BUFSIZE (64)



volatile static int ser;

static int error(char* description) {
	fprintf(stderr, "%s\n", description);
	return -1;
}

int tty_init()
{
	struct termios termsettings;
	if (tcgetattr(ser, &termsettings) == -1)
		return error("tcgetattr failed");
	if (cfsetispeed(&termsettings, B57600) == -1)
		return error("tcgetattr failed");
	if (cfsetospeed(&termsettings, B57600) == -1)
		return error("tcgetattr failed");
	cfmakeraw(&termsettings);
	if (tcsetattr(ser, TCSANOW, &termsettings) == -1)
		return error("tcgetattr failed");
	
	
	return 0;
}

int roomba_open(int mode)
{
	ser = open(DEVICE, O_RDWR);
	if (ser == -1)
		return error("Open failed");
	if (tty_init() == -1)
		return -1;
	
	roomba_write(ROOMBA_START);
	delay(10);
	if (mode == ROOMBA_MODE_FULL) {
		roomba_write(ROOMBA_FULL);
		fprintf(stdout, "Starting roomba in FULL mode!\n");
	}
	else {
		roomba_write(ROOMBA_SAFE);
		fprintf(stdout, "Starting roomba in SAFE mode!\n");
	}
	delay(10);
}

int roomba_close()
{
	if (close(ser) == -1) return -1; // Close failed
	return 0;
}

int roomba_read(unsigned char buf[], int size)
{
    int rcount;
	int offset = 0;
	while (size > 0)
	{
		rcount = read(ser, buf + offset, size);
		if (size == -1) return -1; // Read failed
		size = size - rcount;
		offset = offset + rcount;
	}
	return rcount; // Return number of read bytes
}

int roomba_write(unsigned char outbyte)
{
	if (write(ser, &outbyte, 1) != 1) return -1; // Write failed
    return 0;
}

int roomba_drive(short velocity, short radius)
{
	if (roomba_write(ROOMBA_DRIVE) == -1) return -1;
	if (roomba_write((unsigned char)(velocity >> 8)) == -1) return -1;
	if (roomba_write((unsigned char)(velocity & 0xFF)) == -1) return -1;
	if (roomba_write((unsigned char)(radius >> 8)) == -1) return -1;
	if (roomba_write((unsigned char)(radius & 0xFF)) == -1) return -1;
	return 0;
}

int roomba_direct_drive(short left_velocity, short right_velocity)
{
	if (roomba_write(ROOMBA_DRIVE_DIRECT) == -1) return -1;
	if (roomba_write((unsigned char)(right_velocity >> 8)) == -1) return -1;
	if (roomba_write((unsigned char)(right_velocity & 0xFF)) == -1) return -1;
	if (roomba_write((unsigned char)(left_velocity >> 8)) == -1) return -1;
	if (roomba_write((unsigned char)(left_velocity & 0xFF)) == -1) return -1;
	return 0;
}

int roomba_led(unsigned char play, unsigned char advance, unsigned char colour, unsigned char intensity)
{
	unsigned char flags = (play ? 2 : 0) | (advance ? 8 : 0);
	if (roomba_write(ROOMBA_LED) == -1) return -1;
	if (roomba_write(flags) == -1) return -1;
	if (roomba_write(colour) == -1) return -1;
	if (roomba_write(intensity) == -1) return -1;
	return 0;
}

int roomba_read_bumps(unsigned char * left_bump, unsigned char * right_bump)
{
	unsigned char sensorPacket[10];
	if (roomba_write(ROOMBA_SENSORS) == -1) return -1;
	if (roomba_write(1) == -1) return -1;
	if (roomba_read(sensorPacket, 10) == -1) return -1;
	*left_bump  = (sensorPacket[0] & 2) ? 1 : 0;
	*right_bump = (sensorPacket[0] & 1) ? 1 : 0;
}
