#include "triple.h"

#define NORMALIZER (1/255.0)

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
