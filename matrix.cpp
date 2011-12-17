#include "matrix.h"

#define RENORMALIZER 255
#define NORMALIZER (1.0/RENORMALIZER)

tmatrix *normQImage(QImage im)
{
    if (im.isNull()) {
        return NULL;
    }

    tmatrix *norm = new tmatrix(im.width(), im.height());

    for (int x=0; x<norm->w; x++) {
        for (int y=0; y<norm->h; y++) {
            dtriple pix = normQRgb(im.pixel(x,y));
            norm->pos(y,x) = pix;
        }
    }

    return norm;
}

QImage dmToQImage(dmatrix *m)
{
    QImage im(m->w, m->h, QImage::Format_RGB32);

    for (int x=0; x<m->w; x++) {
        for (int y=0; y<m->h; y++) {
            double v = m->pos(y,x) * RENORMALIZER;
            im.setPixel(x, y, qRgb(v,v,v));
        }
    }

    return im;
}

QImage tmToQImage(tmatrix *m)
{
    QImage im(m->w, m->h, QImage::Format_RGB32);

    for (int x=0; x<m->w; x++) {
        for (int y=0; y<m->h; y++) {
            QRgb pix = dtToQRgb(m->pos(y,x));
            im.setPixel(x, y, pix);
        }
    }

    return im;
}
