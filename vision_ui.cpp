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

static MyCameraWindow * mainWin;
static QApplication * app;

extern "C" void vision_ui_init(int argc, char ** argv) {
	app = new QApplication(argc, argv);
}

extern "C" void * vision_ui_thread_func(void * ptr) {
	mainWin = new MyCameraWindow(vision_getcamera());
	mainWin->setWindowTitle("Roomba Ball Follow");
	mainWin->show();
	app->exec();
	return 0;
}

extern "C" void vision_ui_quit() {
	mainWin->close();
}

extern "C" void vision_ui_update() {
	QApplication->postEvent(mainWin, new QEvent(QEvent::Type(1001)));
}

