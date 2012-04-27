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

int main(int argc, char ** argv) {
    vision_init();

    QApplication app(argc, argv);
    MyCameraWindow * mainWin = new MyCameraWindow(vision_getcamera());
    mainWin->setWindowTitle("Roomba vision test");
    mainWin->show();    
    int retval = app.exec();
    
    vision_free();
    
    return retval;
}

