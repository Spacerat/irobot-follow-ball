#include "MyCameraWindow.h"
#include "vision.h"

MyCameraWindow::MyCameraWindow(CvCapture * cam, QWidget * parent) : QWidget(parent) {
    camera = cam;
    QVBoxLayout * layout = new QVBoxLayout;
    cvwidget = new QOpenCVWidget(this);
    layout->addWidget(cvwidget);
    setLayout(layout);
    resize(500, 400);

    startTimer(100);  // 0.1-second timer
}

void MyCameraWindow::timerEvent(QTimerEvent *) {
    IplImage * image = vision_getframe();
    int xpos, area, width;
    image_process(&xpos, &area, &width);
    cvwidget->putImage(image);
}

