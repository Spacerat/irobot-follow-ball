#include "MyCameraWindow.h"
#include "vision.h"
#include <QMouseEvent>
#include <stdio.h>
#include "QOpenCVWidget.h"

MyCameraWindow::MyCameraWindow(CvCapture * cam, QWidget * parent) : QWidget(parent) {
    camera = cam;
    QVBoxLayout * layout = new QVBoxLayout;
    cvwidget = new QOpenCVWidget(this);
    layout->addWidget(cvwidget);
    setLayout(layout);
    resize(500, 400);

    startTimer(100);  // 0.1-second timer
}

void QOpenCVWidget::mousePressEvent(QMouseEvent * event) {
	printf("%f\n", calibrate(event->x(), event->y()));
	writeCalibration("calibration.txt");
	int xpos, area, width;
	area = 0;
	image_process(&xpos, &area, &width);
	printf("Position: %f\nArea: %d\n", (float)xpos/(float)width, area);

}

void MyCameraWindow::timerEvent(QTimerEvent *) {
    IplImage * image = vision_getframe();
    int xpos, area, width;
    image_process(&xpos, &area, &width);
    cvwidget->putImage(image);
}

