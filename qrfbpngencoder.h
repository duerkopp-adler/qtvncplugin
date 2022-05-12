#ifndef QRFBPNGENCODER_H
#define QRFBPNGENCODER_H

#include "qvnc_p.h"

class QRfbPngEncoder : public QRfbEncoder
{
public:
    QRfbPngEncoder(QVncClient *s);

public:
    void write();
};

#endif // QRFBPNGENCODER_H
