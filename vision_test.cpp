#ifndef NO_VISION
#include <opencv/cv.h>
#include <opencv/highgui.h>
#endif
#include <stdio.h>
#include <assert.h>
#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include "QOpenCVWidget.h"
#include "MyCameraWindow.h"
#ifndef NO_VISION
#include "vision.h"
#endif

int main(int argc, char ** argv) {
#ifndef NO_VISION
	vision_init();
	readCalibration("calibration.txt");
#endif

	QApplication app(argc, argv);
#ifndef NO_VISION
	MyCameraWindow * mainWin = new MyCameraWindow(vision_getcamera());
#else
	MyCameraWindow * mainWin = new MyCameraWindow(NULL);
#endif
	mainWin->setWindowTitle("Roomba vision test");
	mainWin->show();    
	int retval = app.exec();
	
#ifndef NO_VISION
	vision_free();
#endif
	
	return retval;
}

