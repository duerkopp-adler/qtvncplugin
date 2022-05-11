#include "qvncopenglcontext.h"
#include "qvncscreen.h"

#include <qdebug.h>
#include <qpa/qplatformsurface.h>
#include <qpa/qplatformscreen.h>
#include <qopenglcontext.h>
#include <qscreen.h>
#include <qcoreapplication.h>
#include <GL/osmesa.h>

QT_BEGIN_NAMESPACE

class QVncOpenGLContextData
{
public:
    QSurfaceFormat format;
    OSMesaContext mesaContext = nullptr;
    QImage surfaceImage;
    void (*glFinish) () = nullptr;
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

    d->glFinish = OSMesaGetProcAddress("glFinish");
}

QVncOpenGLContext::~QVncOpenGLContext()
{
    OSMesaDestroyContext(d->mesaContext);
}

bool QVncOpenGLContext::makeCurrent(QPlatformSurface* surface)
{
    QSize size = surface->surface()->size();
    const QScreen *screen = context()->screen();
    d->surfaceImage = QImage(size,  screen->handle()->format());
    return OSMesaMakeCurrent(d->mesaContext, d->surfaceImage.bits(), GL_UNSIGNED_BYTE, size.width(), size.height());
}

void QVncOpenGLContext::doneCurrent()
{

}

void QVncOpenGLContext::swapBuffers(QPlatformSurface* surface)
{
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

QImage QVncOpenGLContext::image() const
{
    if (d->surfaceImage.isNull())
    {
        return QImage();
    }

    d->glFinish();

    const QSize screenSize = context()->screen()->size();
    QImage screenImage(d->surfaceImage.mirrored(false, true).copy(0, 0, screenSize.width(), screenSize.height()));
    return screenImage;
}

bool QVncOpenGLContext::isSharing() const
{
    return false;
}

bool QVncOpenGLContext::isValid() const
{
    return d->mesaContext != nullptr;
}
