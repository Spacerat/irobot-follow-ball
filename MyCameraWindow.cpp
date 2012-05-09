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
	labelHueVal = new QLabel("0.00\xb0");
	labelSat    = new QLabel("Saturation: ", this);
	sliderSat   = new QSlider(Qt::Horizontal, this);
	labelSatVal = new QLabel("100.00%");
	labelPosition = new QLabel("X=0 Area=0 Left Speed=0 Right Speed=0 Distance=0");

	QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	sliderSat->setSizePolicy(sizePolicy);
	sliderHue->setSizePolicy(sizePolicy);

	sliderHue->setMinimum(-50000);
	sliderHue->setMaximum(50000);
	sliderSat->setMaximum(100000);
	sliderSat->setMinimum(0);
	sliderSat->setValue(100000);
	
	QObject::connect(sliderHue, SIGNAL(valueChanged(int)), this, SLOT(hueValueChanged(int)));
	QObject::connect(sliderSat, SIGNAL(valueChanged(int)), this, SLOT(satValueChanged(int)));

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
	
#ifndef NO_VISION
	updateUICalibration(calibrationGetHue(), calibrationGetSat());
#endif

	setLayout(layout);
	
	startTimer(50);
}

void MyCameraWindow::updateUICalibration(float hue, float sat) {
	sliderHue->setValue((int)(hue / M_PI * 50000.f));
	sliderSat->setValue((int)(sat * 100000.f));
	hueValueChanged((int)(hue / M_PI * 50000.f));
	satValueChanged((int)(sat * 100000.f));
}

void MyCameraWindow::hueValueChanged(int value) {
	char text[128] = {0};
	sprintf(text, "%.2f\xb0", (float)value / 50000.f * 180.f);
	labelHueVal->setText(text);
#ifndef NO_VISION
	calibrationSetHue((float)value / 50000.f * M_PI);
	writeCalibration("calibration.txt");
#endif
}

void MyCameraWindow::satValueChanged(int value) {
	char text[128] = {0};
	sprintf(text, "%.2f%%", (float)value / 1000.f);
	labelSatVal->setText(text);
#ifndef NO_VISION
	calibrationSetSat((float)value / 100000.f);
	writeCalibration("calibration.txt");
#endif
}

void QOpenCVWidget::mousePressEvent(QMouseEvent * event) {
#ifndef NO_VISION
	printf("%f\n", calibrate(event->x(), event->y()));
	writeCalibration("calibration.txt");
	int xpos, area, width;
	area = 0;
	image_process(&xpos, &area, &width);
	printf("Position: %f\nArea: %d\n", (float)xpos/(float)width, area);
	((MyCameraWindow *)parent())->updateUICalibration(calibrationGetHue(), calibrationGetSat());
#endif
}

void MyCameraWindow::timerEvent(QTimerEvent *) {
#ifndef NO_VISION
	IplImage * image = vision_getimage();
	if (image) cvwidget->putImage(image);
#endif
}

