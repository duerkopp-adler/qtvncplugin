#include "qvncwindow.h"

QVncWindow::QVncWindow(QWindow *window)
    : QFbWindow(window)
{

}

QImage* QVncWindow::image()
{
    return &m_image;
}
