#ifndef MYCAMERAWINDOW_H_
#define MYCAMERAWINDOW_H_

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QSlider>
#include "QOpenCVWidget.h"
#ifndef NO_VISION
#include <opencv/cv.h>
#include <opencv/highgui.h>
#endif

class MyCameraWindow : public QWidget
{
	Q_OBJECT
	private:
		QOpenCVWidget * cvwidget;
#ifndef NO_VISION
		CvCapture     * camera;
#endif
		QLabel        * labelHue;
		QLabel        * labelSat; 
		QSlider       * sliderHue;
		QSlider       * sliderSat;
		QLabel        * labelPos;

        public:
#ifndef NO_VISION
		MyCameraWindow(CvCapture * cam, QWidget * parent = 0);
#else
		MyCameraWindow(void * cam, QWidget * parent = 0);
#endif        
 
	protected:
		void timerEvent(QTimerEvent*);
};


#endif /*MYCAMERAWINDOW_H_*/
