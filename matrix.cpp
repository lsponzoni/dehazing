#include "matrix.h"

#define RENORMALIZER 255
#define NORMALIZER (1/RENORMALIZER)

tmatrix *normQImage(QImage im)
{
    if (im.isNull()) {
        return NULL;
    }

    tmatrix *norm = new tmatrix(im.width(), im.height());

    for (int x=0; x<norm->w; x++) {
        for (int y=0; y<norm->h; y++) {
            norm->pos(y,x) = normQRgb(im.pixel(x,y));
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
            dtriple mp = m->pos(y,x);

            double r = mp.fst * RENORMALIZER;
            double g = mp.snd * RENORMALIZER;
            double b = mp.trd * RENORMALIZER;

            im.setPixel(x, y, qRgb(r,g,b));
        }
    }

    return im;
}
