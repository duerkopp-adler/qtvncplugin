#include "qrfbpngencoder.h"
#include "qvncclient.h"

#include <qtcpsocket.h>
#include <qendian.h>
#include <qbuffer.h>

#ifdef Q_OS_WIN
#include <Winsock2.h>
#else
#include <arpa/inet.h>
#endif

QRfbPngEncoder::QRfbPngEncoder(QVncClient* s)
    : QRfbEncoder(s)
{

}

void QRfbPngEncoder::write()
{
    QTcpSocket *socket = client->clientSocket();

    QRegion rgn = client->dirtyRegion();
    qCDebug(lcVnc) << "QRfbPngEncoder::write()" << rgn;

    const auto rectsInRegion = rgn.rectCount();

    {
        const char tmp[2] = { 0, 0 }; // msg type, padding
        socket->write(tmp, sizeof(tmp));
    }

    {
        const quint16 count = htons(rectsInRegion);
        socket->write((char *)&count, sizeof(count));
    }

    if (rectsInRegion <= 0)
        return;

   const QImage screenImage = client->server()->screenImage();

    for (const QRect &tileRect: rgn) {
        const QRfbRect rect(tileRect.x(), tileRect.y(),
                            tileRect.width(), tileRect.height());
        rect.write(socket);

        const quint32 encoding = htonl(-260); // png encoding
        socket->write((char *)&encoding, sizeof(encoding));

        quint8 controlByte = 0xa0;
        socket->write((char *)&controlByte, sizeof(controlByte));

        // Convert image rect to PNG byte array
        QByteArray ba;
        QBuffer buffer(&ba);
        buffer.open(QIODevice::WriteOnly);
        screenImage.copy(tileRect).save(&buffer, "PNG");
        int length = ba.length();

        // Encode length of pixel data
        quint8 b = length & 0x7f;
        length >>= 7;
        if (length > 0)
        {
            b |= 0x80;
            socket->putChar(b);
            b = length & 0x7f;
            length >>= 7;
            if (length > 0)
            {
                b |= 0x80;
                socket->putChar(b);
                b = length & 0x7f;
            }
        }
        socket->putChar(b);

        // Write PNG image data
        socket->write(ba);

        if (socket->state() == QAbstractSocket::UnconnectedState)
            break;
    }
    socket->flush();
}
