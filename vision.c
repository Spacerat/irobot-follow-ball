#include "vision.h"
#include <math.h>

#define RED_MIN 64
#define RED_MUL 4
#define BLUEGREEN_MUL 3
#define AREA_MIN 1000

#define RED_HUE_MIN -0.3f
#define RED_HUE_MAX 0.1f
#define RED_BRIGHT_MIN 0.f
#define RED_BRIGHT_MAX 1.f
#define RED_SAT_MIN 0.35f
#define RED_SAT_MAX 1.f

#define min(x, y) ((x) < (y) ? (x) : (y))

#define VISION_TEST

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
	//int xpos, area;
	//image_process(&xpos, &area);
	return image;
}

CvCapture * vision_getcamera() {
	return camera;
}

/*
Process an IplImage (from the camera) and get values for
the X position of the ball on the screen, and its size.
*/
int image_process(int * xpos, int * area, int * width) {
	unsigned char * pixel_data = (unsigned char *)(image->imageData);
	int w = image->width;
	int h = image->height;
	int c = image->nChannels;
	*width = image->width;
	*area = 0;
	int moment = 0;
	int y = 0;
	while (y<h) {
		int x = 0;
		while (x < w) {
			unsigned char * blue  = pixel_data;
			unsigned char * green = pixel_data + 1;
			unsigned char * red   = pixel_data + 2;

			float fblue  = (float)*blue  / 255.f;
			float fgreen = (float)*green / 255.f;
			float fred   = (float)*red   / 255.f;

			float hue = atan2(sqrt(3) * (fgreen - fblue), 2.f * fred - fgreen - fblue);
			float bright = (0.2126f * fred) + (0.7152f * fgreen) + (0.0722f * fblue);
			float sat = 1.f - (3.f * min(min(fred, fgreen), fblue) / (fred + fgreen + fblue));

			if (hue > RED_HUE_MIN && hue < RED_HUE_MAX &&
				bright > RED_BRIGHT_MIN && bright < RED_BRIGHT_MAX &&
				sat > RED_SAT_MIN && sat < RED_SAT_MAX) {
				*red = 255;
				*area = *area + 1;
				moment = moment + x; 
			} else {
				//*red = *red >> 1;
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
		#ifdef VISION_TEST
		unsigned char * blue_pixel =  (unsigned char *)(image->imageData) + c * *xpos;
		for (y = 0; y < image->height; y++) {
			* blue_pixel = 255;
			blue_pixel = blue_pixel + c*w;
		    y = y+1;
		}
		#endif
		return 0;
	}
	return 1;
}

