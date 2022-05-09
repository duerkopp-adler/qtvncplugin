#include "qvncopenglcontext.h"

#include <qdebug.h>
#include <qpa/qplatformsurface.h>
#include <GL/osmesa.h>

QT_BEGIN_NAMESPACE

class QVncOpenGLContextData
{
public:
    QSurfaceFormat format;
    OSMesaContext mesaContext = nullptr;
    GLubyte* buffer = nullptr;
};

QVncOpenGLContext::QVncOpenGLContext(const QSurfaceFormat& format)
    : d(new QVncOpenGLContextData)
{
    d->format = format;

    static const int attribs[] = {
       OSMESA_FORMAT,                 OSMESA_RGBA,
       OSMESA_DEPTH_BITS,             24,
       OSMESA_STENCIL_BITS,           8,
       OSMESA_ACCUM_BITS,             0,
       OSMESA_PROFILE,                OSMESA_CORE_PROFILE,
       OSMESA_CONTEXT_MAJOR_VERSION,  4,
       OSMESA_CONTEXT_MINOR_VERSION,  5,
       0
    };
    d->mesaContext = OSMesaCreateContextAttribs(attribs, NULL);
}

QVncOpenGLContext::~QVncOpenGLContext()
{
    OSMesaDestroyContext(d->mesaContext);
    if (d->buffer)
    {
        free(d->buffer);
    }
}

bool QVncOpenGLContext::makeCurrent(QPlatformSurface* surface)
{
    QSize size = surface->surface()->size();

    d->buffer = (GLubyte*) malloc(size.width() * size.height() * 4 * sizeof(GLubyte));
    return OSMesaMakeCurrent(d->mesaContext, d->buffer, GL_UNSIGNED_BYTE, size.width(), size.height());
}

void QVncOpenGLContext::doneCurrent()
{
    free(d->buffer);
    d->buffer = nullptr;
}

void QVncOpenGLContext::swapBuffers(QPlatformSurface* surface)
{
    Q_UNUSED(surface)
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
