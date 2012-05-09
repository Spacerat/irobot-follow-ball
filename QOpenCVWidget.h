
#ifndef QOPENCVWIDGET_H
#define QOPENCVWIDGET_H

#ifndef NO_VISION
#include <opencv/cv.h>
#endif
#include <QPixmap>
#include <QLabel>
#include <QWidget>
#include <QVBoxLayout>
#include <QImage>

class QOpenCVWidget : public QWidget {
    private:
        QLabel *imagelabel;
        QVBoxLayout *layout;
        
        QImage image;
        
    public:
        QOpenCVWidget(QWidget *parent = 0);
        ~QOpenCVWidget(void);
#ifndef NO_VISION
        void putImage(IplImage *);
#endif
       	
        void mousePressEvent(QMouseEvent * event);
}; 

#endif
