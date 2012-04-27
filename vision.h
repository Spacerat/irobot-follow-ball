
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
int image_process(int * xpos, int * area);
void get_heading(int xpos, int area, float * angle, int * distance);

#endif

#ifdef __cplusplus
}
#endif
