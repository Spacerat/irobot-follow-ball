#include "MyCameraWindow.h"
#ifndef NO_VISION
#include "vision.h"
#endif
#include <QMouseEvent>
#include <stdio.h>
#include "QOpenCVWidget.h"

#ifndef NO_VISION
MyCameraWindow::MyCameraWindow(CvCapture * cam, QWidget * parent) : QWidget(parent) {
	camera = cam;
#else
MyCameraWindow::MyCameraWindow(void * cam, QWidget * parent) : QWidget(parent) {
#endif
	QGridLayout * layout = new QGridLayout(this);

	cvwidget  = new QOpenCVWidget(this);
	labelHue  = new QLabel("Hue: ", this);
	sliderHue = new QSlider(Qt::Horizontal, this);

	QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	labelHue->setSizePolicy(sizePolicy);
	sliderHue->setSizePolicy(sizePolicy);

	layout->addWidget(cvwidget, 0, 0, 1, 2, Qt::AlignLeft | Qt::AlignTop);
	layout->addWidget(labelHue, 1, 0, 1, 1, Qt::AlignLeft | Qt::AlignVCenter);
	layout->addWidget(sliderHue, 1, 1, 1, 1, Qt::AlignLeft | Qt::AlignVCenter);

	layout->setRowStretch(0, 1);
	layout->setColumnStretch(1, 1);

	setLayout(layout);
	resize(500, 400);
	startTimer(100);
}

void QOpenCVWidget::mousePressEvent(QMouseEvent * event) {
#ifndef NO_VISION
	printf("%f\n", calibrate(event->x(), event->y()));
	writeCalibration("calibration.txt");
	int xpos, area, width;
	area = 0;
	image_process(&xpos, &area, &width);
	printf("Position: %f\nArea: %d\n", (float)xpos/(float)width, area);
#endif
}

void MyCameraWindow::timerEvent(QTimerEvent *) {
#ifndef NO_VISION
	IplImage * image = vision_getframe();
	int xpos, area, width;
	image_process(&xpos, &area, &width);
	cvwidget->putImage(image);
#endif
}

