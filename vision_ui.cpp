#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <stdio.h>
#include <assert.h>
#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include "QOpenCVWidget.h"
#include "MyCameraWindow.h"
#include "vision.h"

void * vision_ui_thread_func(void * ptr) {
	QApplication app(argc, argv);
	MyCameraWindow * mainWin = new MyCameraWindow(vision_getcamera());
	mainWin->setWindowTitle("Roomba Ball Follow");
	mainWin->show();
	app.exec();
}

void vision_ui_quit() {
	mainWin->close();
}
