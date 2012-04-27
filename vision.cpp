#include "vision.h"

#define RED_MIN 128
#define RED_MUL 4
#define BLUEGREEN_MUL 3
#define AREA_MIN 4000


static CvCapture * camera;
static IplImage * image;

/*
We're going to need some kind of callibrations stuff, but I'm
not sure exactly how yet.
*/
// void callibrate();

/*
Basically, this should take the output of image_process
as input, and return the angle and distance to the ball.

This is probably going to take a lot of tuning to get right.
*/
void get_heading(int xpos, int area, float * angle, int * distance) {

}

void vision_init() {
	camera = cvCreateCameraCapture(0);
	assert(camera);
	image = cvQueryFrame(camera);
	assert(image);
}

/*
Frees the camera upon exiting the program.
*/
void vision_free() {	
    cvReleaseCapture(&camera);
}

/*
Get a frame from the camera, stores it in image and
returns a pointer to it.
*/
IplImage * vision_getframe() {
	image = cvQueryFrame(camera);
	assert(image);
	return image;
}

CvCapture * vision_getcamera() {
	return camera;
}

/*
Process an IplImage (from the camera) and get values for
the X position of the ball on the screen, and its size.
*/
int image_process(int * xpos, int * area) {
	unsigned char * pixel_data = (unsigned char *)(image->imageData);
	int w = image->width;
	int h = image->height;
	int c = image->nChannels;
	*area = 0;
	int moment = 0;
	int y = 0;
	while (y<h) {
		int x = 0;
		while (x < w) {
			unsigned char * blue  = pixel_data;
			unsigned char * green = pixel_data + 1;
			unsigned char * red   = pixel_data + 2;
			if (((RED_MUL*(int)*red) > (BLUEGREEN_MUL*((int)*blue + (int)* green))) 
			& (*red > RED_MIN)) {
				*red = 255;
				*area = *area + 1;
				moment = moment + x; 
			} else {
				*red = 0;
			}
			//*blue = 0;
			//*green = 0;
			pixel_data = pixel_data + c; 
			x = x + 1;
		}
		y = y + 1;
	}
	if (*area > AREA_MIN) {
		*xpos = moment / *area;
		return 0;
	}
	return 1;
}

