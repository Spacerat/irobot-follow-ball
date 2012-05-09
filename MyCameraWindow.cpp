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
	
	resize(500, 400);

	QGridLayout * layout = new QGridLayout(this);

	cvwidget    = new QOpenCVWidget(this);
	labelHue    = new QLabel("Hue: ", this);
	sliderHue   = new QSlider(Qt::Horizontal, this);
	labelHueVal = new QLabel("360\xb0");
	labelSat    = new QLabel("Saturation: ", this);
	sliderSat   = new QSlider(Qt::Horizontal, this);
	labelSatVal = new QLabel("100%");
	labelPosition = new QLabel("X=0 Area=0 Left Speed=0 Right Speed=0 Distance=0");

	QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	sliderSat->setSizePolicy(sizePolicy);
	sliderHue->setSizePolicy(sizePolicy);

	layout->addWidget(cvwidget, 0, 0, 1, 3);
	layout->addWidget(labelHue, 1, 0, 1, 1);
	layout->addWidget(sliderHue, 1, 1, 1, 1);
	layout->addWidget(labelHueVal, 1, 2, 1, 1);
	layout->addWidget(labelSat, 2, 0, 1, 1);
	layout->addWidget(sliderSat, 2, 1, 1, 1);
	layout->addWidget(labelSatVal, 2, 2, 1, 1);
	layout->addWidget(labelPosition, 3, 0, 1, 3);

	layout->setRowStretch(0, 1);
	layout->setColumnStretch(1, 1);

	setLayout(layout);
	
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

