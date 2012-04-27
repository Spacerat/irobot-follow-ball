#ifndef _vision_h_
#define _vision_h_

void vision_init();
void vision_free();
IplImage * vision_getframe();
CvCapture * vision_getcamera();
void image_process(int * xpos, int * area);
void get_heading(int xpos, int area, float * angle, int * distance);

#endif
