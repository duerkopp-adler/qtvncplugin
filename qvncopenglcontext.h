#ifndef QVncOpenGLContext_H
#define QVncOpenGLContext_H

#include <qscopedpointer.h>
#include <qpa/qplatformopenglcontext.h>

QT_BEGIN_NAMESPACE

class QVncOpenGLContextData;

class QVncOpenGLContext : public QPlatformOpenGLContext
{
public:
    QVncOpenGLContext(const QSurfaceFormat& format);
    virtual ~QVncOpenGLContext();

    bool makeCurrent(QPlatformSurface *surface) override;
    void doneCurrent() override;
    void swapBuffers(QPlatformSurface *surface) override;
    QFunctionPointer getProcAddress(const char *procName) override;

    QSurfaceFormat format() const override;
    bool isSharing() const override;
    bool isValid() const override;

private:
    QScopedPointer<QVncOpenGLContextData> d;
};

#endif // QVncOpenGLContext_H

