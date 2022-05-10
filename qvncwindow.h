#ifndef QVNCWINDOW_H
#define QVNCWINDOW_H

#include <QtFbSupport/private/qfbwindow_p.h>

class QVncWindow : public QFbWindow
{
public:
    QVncWindow(QWindow *window);

    QImage* image();

private:
    QImage m_image;
};

#endif // QVNCWINDOW_H
