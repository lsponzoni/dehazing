#include <cstring>
#include <cmath>
#include <iostream>

#include <QtCore/QCoreApplication>
#include <QtGui/QImage>

#include <QString>
#include <QStringList>

#include "matrix.h"
#include "triple.h"
#include "statistics.h"

typedef matrix<double> dmatrix;
typedef matrix<dtriple> tmatrix;

triple<double, dmatrix*, dmatrix*> estimate (dtriple airlight, tmatrix *image)
{
    double normA = norm(airlight);

    // Calculate Ia
    dmatrix Ia(image->w, image->h);
    for (int i=0; i<image->h; i++) {
        for (int j=0; j<image->w; j++) {
            dtriple imP = image->pos(i,j);

            double dotProd = imP.fst * airlight.fst
                            + imP.snd * airlight.snd
                            + imP.trd * airlight.trd;

            Ia.pos(i,j) = dotProd/normA;
        }
    }

    // Calculate Ir
    dmatrix Ir(image->w, image->h);
    for (int i=0; i<image->h; i++) {
        for (int j=0; j<image->w; j++) {
            double imNorm = norm(image->pos(i,j));
            double iaP = Ia.pos(i,j);
            Ir.pos(i,j) = sqrt( sqr(imNorm) - sqr(iaP) );
        }
    }

    // Calculate h
    dmatrix h(image->w, image->h);
    for (int i=0; i<image->h; i++) {
        for (int j=0; j<image->w; j++) {
            double iaP = Ia.pos(i,j);
            double irP = Ir.pos(i,j);
            h.pos(i,j) = (normA - iaP) / irP;
        }
    }

    // Estimate the eta
    int length = h.w * h.h;
    double eta = cov(Ia.data, h.data, length) / cov(Ir.data, h.data, length);

    // Calculate the transmission and shading
    dmatrix *t = new dmatrix(h.w, h.h);
    dmatrix *l = new dmatrix(h.w, h.h);
    for (int i=0; i<image->h; i++) {
        for (int j=0; j<image->w; j++) {
            double iaP = Ia.pos(i,j);
            double irP = Ir.pos(i,j);

            double tP = 1 - (iaP - eta * irP) / normA;

            t->pos(i,j) = tP;
            l->pos(i,j) = irP / tP;
        }
    }

    return triple<double, dmatrix*, dmatrix*>(eta, t, l);
}

tmatrix *normalizedQImage(QImage im)
{
#define NORMALIZER (1/255.0)
    tmatrix *norm = new tmatrix(im.width(), im.height());

    for (int x=0; x<norm->w; x++) {
        for (int y=0; y<norm->h; y++) {
            QRgb iP = im.pixel(x,y);
            double r = qRed(iP) * NORMALIZER;
            double g = qGreen(iP) * NORMALIZER;
            double b = qBlue(iP) * NORMALIZER;
            norm->pos(y,x) = dtriple(r,g,b);
        }
    }

    return norm;
}

QImage dmatrixToQImage(dmatrix *m)
{
#define RENORMALIZER 255
    QImage im(m->w, m->h, QImage::Format_RGB32);

    for (int x=0; x<m->w; x++) {
        for (int y=0; y<m->h; y++) {
            double v = m->pos(y,x) * RENORMALIZER;
            im.setPixel(x, y, qRgb(v,v,v));
        }
    }

    return im;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QString inFile = QCoreApplication::arguments()[1];
    QString outFile = "dehazed" + inFile;

    QImage image(inFile);
    QImage dehazed = image;
    tmatrix *im = normalizedQImage(image);

    dtriple airlight(0.8, 0.8, 0.9);
    triple<double, dmatrix*, dmatrix*> est = estimate(airlight, im);
    double eta = est.fst;
    dmatrix *t = est.snd;
    dmatrix *f = est.trd;

    std::cout << "Estimated eta: " << eta << std::endl;
    dmatrixToQImage(t).save("trans" + inFile);
    dmatrixToQImage(f).save("shad" + inFile);

    std::cout << "Dehazed image " << inFile.toStdString() << std::endl;
    std::cout << "Saving to " << outFile.toStdString() << std::endl;

    dehazed.save(outFile);

    return 0;//a.exec();
}
