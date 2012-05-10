#include "vision.h"
#include <math.h>
#include <stdio.h>

#define RED_MIN 128
#define RED_MUL 4
#define BLUEGREEN_MUL 3
#define AREA_MIN 1000

static int hueCal   = 0;
static int hueRange = 20;
static int satCal   = -51;
static int satRange = 51;

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

/*
Returns the image last retrieved from the camera.
*/
IplImage * vision_getimage() {
	return image;
}

/*
Calibrates the hue and saturation ranges to the pixel specified.
*/
float calibrate(int xpos, int ypos) {
	if(!image) return 0.f;
	if(xpos < 5 || xpos > image->width || ypos < 5 || ypos > image->height) return 0.f;
	
	unsigned char * pixel_data = (unsigned char *)(image->imageData);
	
	float totalHue = 0.f;
	float totalSat = 0.f;
	int x, y, w = image->width;	
	for(x = xpos - 5; x < xpos + 5; x++) {
		for(y = ypos - 5; y < ypos + 5; y++) {
		
			unsigned char * blue  = pixel_data + x*3 + y*w*3;
			unsigned char * green = pixel_data + x*3 + y*w*3 + 1;
			unsigned char * red   = pixel_data + x*3 + y*w*3 + 2;

			float fblue  = (float)*blue  / 255.f;
			float fgreen = (float)*green / 255.f;
			float fred   = (float)*red   / 255.f;
			
			totalHue += atan2(sqrt(3) * (fgreen - fblue), 2.f * fred - fgreen - fblue);
			totalSat += 1.f - (3.f * min(min(fred, fgreen), fblue) / (fred + fgreen + fblue));
		}
	}
	
	calibrationSetHue(totalHue / 100.f);
	calibrationSetSat(totalSat / 100.f);
	
	return 0.f;
}

/*
Reads the calibration values from the given file.
*/
int readCalibration(const char * fileName) {
	FILE * f = fopen(fileName, "rb");
	if(!f) return -1;
	float hue, sat;
	fscanf(f, "%f", &hue);
	fscanf(f, "%f", &sat);
	fclose(f);
	calibrationSetHue(hue);
	calibrationSetSat(sat);
	return 0;
}

/*
Writes the calibration values from the given file.
*/
int writeCalibration(const char * fileName) {
	FILE * f = fopen(fileName, "wb");
	if(!f) return -1;
	fprintf(f, "%f\n", calibrationGetHue());
	fprintf(f, "%f\n", calibrationGetSat());
	fclose(f);
	return 0;
}

void calibrationSetHue(float hue) {
	hueCal = (int)(hue / M_PI * 180.f);
}

void calibrationSetSat(float sat) {
	satCal = (int)(sat * 255.f);
}

float calibrationGetHue() {
	return (float)hueCal / 180.f * M_PI;
}

float calibrationGetSat() {
	return (float)satCal / 255.f;
}

int hue_test_func(unsigned int blue, unsigned int green, unsigned int red) {

	int hue;
	int rg = red - green;
	int rb = red - blue;
	int gb = green - blue;
	if (red >= green && red >= blue) { /* between -60 and 60 */
		if (green > blue) { /* between 0 and 60 */
			hue = 60 * gb / rb;
		}
		else { /* between -60 and 0 */
			hue = 60 * -gb / rg;
		}
	}
	else if (green >= red && green >= blue) { /* between 60 and 180 */
		if (red > blue) { /* between 60 and 120 */
			hue = 120 + 60 * rb / gb;
		}
		else { /* between 120 and 180 */
			hue = 120 + 60 * rb / rg;
		}
	}
	else { /* between -180 and -60 */
		if (red > green) { /* between -120 and -60 */
			hue = -120 + 60 * rg / -gb;
		}
		else { /* between -180 and -120 */
			hue = -120 + 60 * rg / rb;
		}
	}
	int sat = 255 - ((3*255 * min(min(red, green), blue)) / (red + green + blue));

	if (hue    > hueCal-hueRange       && hue    < hueCal+hueRange &&
	    sat    > satCal-satRange       && sat    < satCal+satRange) {
		return 1;
	}
	else return 0;
}

int original_test_func(unsigned char * blue, unsigned char * green, unsigned char * red) {
	if (((RED_MUL*(int)*red) > (BLUEGREEN_MUL*((int)*blue + (int)* green))) 
            & (*red > RED_MIN)) return 1;
	else return 0;
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

			if (hue_test_func(*blue, *green, *red)) {
			//if (original_test_func(blue, green, red)) {
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
		}
		return 1;
	}
	return 0;
}

