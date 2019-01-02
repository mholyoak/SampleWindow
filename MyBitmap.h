#ifndef CMYBITMAP_H
#define CMYBITMAP_H

#include <QPixmap>
#include <QByteArray>

class CMyBitmap
{
public:
    CMyBitmap(const QString yuvFileName, uint32_t imageWidth, uint32_t imageHeight);

    QPixmap _pixmap;
private:
    QByteArray _data;
};

#endif // CMYBITMAP_H
