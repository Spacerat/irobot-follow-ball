#ifndef _vision_h_
#define _vision_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <opencv/cv.h>
#include <opencv/highgui.h>

void vision_init();
void vision_free();
IplImage * vision_getframe();
CvCapture * vision_getcamera();
float calibrate(int xpos, int ypos);
int image_process(int * xpos, int * area, int * width);
int readCalibration(const char * fileName);
int writeCalibration(const char * fileName);
void calibrationSetHue(float hue);
void calibrationSetSat(float sat);
float calibrationGetHue();
float calibrationGetSat();

#endif

#ifdef __cplusplus
}
#endif
