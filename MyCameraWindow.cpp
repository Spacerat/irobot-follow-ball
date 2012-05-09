#include "MyCameraWindow.h"
#include "vision.h"
#include "vision_ui.h"
#include <QMouseEvent>
#include <stdio.h>
#include "QOpenCVWidget.h"

MyCameraWindow::MyCameraWindow(CvCapture * cam, QWidget * parent) : QWidget(parent) {
	camera = cam;
	
	resize(500, 400);

	QGridLayout * layout = new QGridLayout(this);

	cvwidget      = new QOpenCVWidget(this);
	labelHue      = new QLabel("Hue: ", this);
	sliderHue     = new QSlider(Qt::Horizontal, this);
	labelHueVal   = new QLabel("0.00\xb0");
	labelSat      = new QLabel("Saturation: ", this);
	sliderSat     = new QSlider(Qt::Horizontal, this);
	labelSatVal   = new QLabel("100.00%");
	labelPosition = new QLabel("X=\tArea=\tL.Speed=\tR.Speed=\tDistance=");

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
	
	updateUICalibration(calibrationGetHue(), calibrationGetSat());

	setLayout(layout);
	
	startTimer(50);
}

void MyCameraWindow::timerEvent(QTimerEvent *) {
	vision_ui_lock_image();
	updateUIImage();
	
	char text[128] = {0};
	int x, area, l_speed, r_speed;
	float distance;

	vision_ui_get_values(&x, &area, &l_speed, &r_speed, &distance);
	sprintf(text, "X=%d\tArea=%d\tL.Speed=%d R.Speed=%d Distance=%.2f", x, area, l_speed, r_speed, distance);
	labelPosition->setText(text);

	vision_ui_unlock_image();
}

void MyCameraWindow::updateUIImage() {
	IplImage * image = vision_getimage();
        if (image) cvwidget->putImage(image);
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
	calibrationSetHue((float)value / 50000.f * M_PI);
	writeCalibration("calibration.txt");
}

void MyCameraWindow::satValueChanged(int value) {
	char text[128] = {0};
	sprintf(text, "%.2f%%", (float)value / 1000.f);
	labelSatVal->setText(text);
	calibrationSetSat((float)value / 100000.f);
	writeCalibration("calibration.txt");
}

void QOpenCVWidget::mousePressEvent(QMouseEvent * event) {
	printf("%f\n", calibrate(event->x(), event->y()));
	writeCalibration("calibration.txt");
	int xpos, area, width;
	area = 0;
	image_process(&xpos, &area, &width);
	printf("Position: %f\nArea: %d\n", (float)xpos/(float)width, area);
	((MyCameraWindow *)parent())->updateUICalibration(calibrationGetHue(), calibrationGetSat());
}

