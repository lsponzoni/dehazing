#include "triple.h"

#define RENORMALIZER 255
#define NORMALIZER (1.0/RENORMALIZER)

dtriple normQColor(QColor color)
{
    double r = color.red() * NORMALIZER;
    double g = color.green() * NORMALIZER;
    double b = color.blue() * NORMALIZER;

    return dtriple(r,g,b);
}

dtriple normQRgb(QRgb color)
{
    double r = qRed(color) * NORMALIZER;
    double g = qGreen(color) * NORMALIZER;
    double b = qBlue(color) * NORMALIZER;

    return dtriple(r,g,b);
}

QRgb dtToQRgb(dtriple color)
{
    double r = color.fst * RENORMALIZER;
    double g = color.snd * RENORMALIZER;
    double b = color.trd * RENORMALIZER;

    return qRgb(r,g,b);
}
