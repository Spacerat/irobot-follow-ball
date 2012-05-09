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
#include "vision_ui.h"

static MyCameraWindow * mainWin;
static QApplication * app;
static pthread_mutex_t image_mutex;
static int cur_xpos = 0, cur_area = 0, cur_l = 0, cur_r = 0;
static float cur_distance = 0.;


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

extern "C" void vision_ui_update_values(int xpos, int area, int l_speed, int r_speed, float distance) {
	cur_xpos     = xpos;
	cur_area     = area;
	cur_l        = l_speed;
	cur_r        = r_speed;
	cur_distance = distance;
}

extern "C" void vision_ui_get_values(int * xpos, int * area, int * l_speed, int * r_speed, float * distance) {
	*xpos     = cur_xpos;
	*area     = cur_area;
	*l_speed  = cur_l;
	*r_speed  = cur_r;
	*distance = cur_distance;
}
