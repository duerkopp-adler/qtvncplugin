#include "qvncopenglcontext.h"
#include "qvncscreen.h"

#include <qdebug.h>
#include <qpa/qplatformsurface.h>
#include <qpa/qplatformscreen.h>
#include <qopenglcontext.h>
#include <qscreen.h>
#include <qvncwindow.h>
#include <qcoreapplication.h>
#include <qopenglfunctions.h>
#include <GL/osmesa.h>

QT_BEGIN_NAMESPACE

class QVncOpenGLContextData
{
public:
    QSurfaceFormat format;
    OSMesaContext mesaContext = nullptr;
};

QVncOpenGLContext::QVncOpenGLContext(const QSurfaceFormat& format)
    : d(new QVncOpenGLContextData)
{
    d->format.setDepthBufferSize(format.depthBufferSize());
    d->format.setStencilBufferSize(format.stencilBufferSize());
    d->format.setRedBufferSize(8);
    d->format.setGreenBufferSize(8);
    d->format.setBlueBufferSize(8);
    d->format.setVersion(3, 2);
    d->format.setRenderableType(QSurfaceFormat::RenderableType::OpenGL);
    d->format.setProfile(QSurfaceFormat::OpenGLContextProfile::CoreProfile);

    const int attribs[] = {
       OSMESA_FORMAT,                 OSMESA_RGBA,
       OSMESA_DEPTH_BITS,             format.depthBufferSize(),
       OSMESA_STENCIL_BITS,           format.stencilBufferSize(),
       OSMESA_ACCUM_BITS,             0,
       OSMESA_PROFILE,                OSMESA_CORE_PROFILE,
       OSMESA_CONTEXT_MAJOR_VERSION,  format.majorVersion(),
       OSMESA_CONTEXT_MINOR_VERSION,  format.minorVersion(),
       0
    };
    d->mesaContext = OSMesaCreateContextAttribs(attribs, NULL);
}

QVncOpenGLContext::~QVncOpenGLContext()
{
    OSMesaDestroyContext(d->mesaContext);
}

bool QVncOpenGLContext::makeCurrent(QPlatformSurface* surface)
{
    QSize size = surface->surface()->size();
    QVncWindow *window = static_cast<QVncWindow*>(surface);
    QImage *image = window->image();
    *image = QImage(size, context()->screen()->handle()->format());
    bool ok = OSMesaMakeCurrent(d->mesaContext, image->bits(), GL_UNSIGNED_BYTE, size.width(), size.height());
    return ok;
}

void QVncOpenGLContext::doneCurrent()
{

}

void QVncOpenGLContext::swapBuffers(QPlatformSurface* surface)
{
    context()->functions()->glFinish();
    QVncScreen *screen = static_cast<QVncScreen*>(surface->screen());
    screen->setDirty(screen->geometry());
    QEvent request(QEvent::UpdateRequest);
    QCoreApplication::sendEvent(screen, &request);
}

QFunctionPointer QVncOpenGLContext::getProcAddress(const char* procName)
{
    return reinterpret_cast<QFunctionPointer>(OSMesaGetProcAddress(procName));
}

QSurfaceFormat QVncOpenGLContext::format() const
{
    return d->format;
}

bool QVncOpenGLContext::isSharing() const
{
    return false;
}

bool QVncOpenGLContext::isValid() const
{
    return d->mesaContext != nullptr;
}
