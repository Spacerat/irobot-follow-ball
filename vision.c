#include "vision.h"
#include <math.h>

#define RED_MIN 64
#define RED_MUL 4
#define BLUEGREEN_MUL 3
#define AREA_MIN 1000

static int hueMin    = -0.3f;
static int hueMax    =  0.1f;
static int brightMin =  0.f;
static int brightMax =  1.f;
static int satMin    =  0.36f;
static int satMax    =  1.f;

#define min(x, y) ((x) < (y) ? (x) : (y))

static IplImage  * image;
static CvCapture * camera;

/*
Initiailises the camera and gets the first frame from it.
*/
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

/*
Returns the camera descriptor (CvCapture *) of that opened camera.
*/
CvCapture * vision_getcamera() {
	return camera;
}

float calibrate(int xpos, int ypos) {
	if(!image) return 0.f;
	if(xpos < 5 || xpos > image->width || ypos < 5 || ypos > image->height) return 0.f;
	
	unsigned char * pixel_data = (unsigned char *)(image->imageData);
	
	float totalHue = 0.f;
	int x, y;	
	for(x = xpos; x < xpos+1; x++) {
		for(y = ypos; y < ypos+1; y++) {
		
			unsigned char * blue  = pixel_data;
			unsigned char * green = pixel_data + 1;
			unsigned char * red   = pixel_data + 2;

			float fblue  = (float)*blue  / 255.f;
			float fgreen = (float)*green / 255.f;
			float fred   = (float)*red   / 255.f;
			
			totalHue = atan2(sqrt(3) * (fgreen - fblue), 2.f * fred - fgreen - fblue);
		}
	}
	
	float hue = totalHue / 1.f;
	
	hueMin = hue - 0.2f;
	hueMax = hue + 0.2f;
	
	return hue;
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

			if (hue    > hueMin    && hue    < hueMax &&
				bright > brightMin && bright < brightMax &&
				sat    > satMin    && sat    < satMax) {
				*red = 255;
				*area = *area + 1;
				moment = moment + x; 
			} else {
				*red = *red >> 1;
			}
			*blue = *blue >> 1;
			*green = *green >> 1;
			pixel_data = pixel_data + c; 
			x = x + 1;
		}
		y = y + 1;
	}
	if (*area > AREA_MIN) {
		*xpos = moment / *area;
		unsigned char * blue_pixel =  (unsigned char *)(image->imageData) + c * *xpos;
		for (y = 0; y < image->height; y++) {
			* blue_pixel = 255;
			blue_pixel = blue_pixel + c*w;
			y = y+1;
		}
		return 0;
	}
	return 1;
}

