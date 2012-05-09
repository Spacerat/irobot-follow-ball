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
#include "pthread.h"

static MyCameraWindow * mainWin;
static QApplication * app;
static pthread_mutex_t image_mutex;

extern "C" void vision_ui_init(int argc, char ** argv) {
	pthread_mutex_init(&image_mutex, NULL);
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
	pthread_mutex_destroy(&image_mutex);
}

extern "C" void vision_ui_lock_image() {
	pthread_mutex_lock(&image_mutex);
}

extern "C" void vision_ui_unlock_image() {
	pthread_mutex_unlock(&image_mutex);
}
