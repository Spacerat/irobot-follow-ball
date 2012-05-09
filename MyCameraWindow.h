#ifndef MYCAMERAWINDOW_H_
#define MYCAMERAWINDOW_H_

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QSlider>
#include "QOpenCVWidget.h"
#include <opencv/cv.h>
#include <opencv/highgui.h>

class MyCameraWindow : public QWidget
{
	Q_OBJECT
	private:
		QOpenCVWidget * cvwidget;
		CvCapture     * camera;
		QLabel        * labelHue;
		QLabel        * labelHueVal;
		QLabel        * labelSat;
		QLabel        * labelSatVal;
		QSlider       * sliderHue;
		QSlider       * sliderSat;
		QLabel        * labelPosition;

        public:
		MyCameraWindow(CvCapture * cam, QWidget * parent = 0);
		void updateUICalibration(float hue, float sat);	
		void updateUIImage();

	public slots:
		void hueValueChanged(int value);
		void satValueChanged(int value);
};


#endif /*MYCAMERAWINDOW_H_*/
