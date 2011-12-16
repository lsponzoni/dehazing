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

dmatrix *estimateTransmission(tmatrix *image, dtriple airlight);
tmatrix *removeAirlight(tmatrix *image, dmatrix *transmission, dtriple airlight);

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QString inFile = QCoreApplication::arguments()[1];
    QString outFile = "dehazed" + inFile;

    QImage image(inFile);
    QImage dehazed = image;
    tmatrix *im = normQImage(image);

    dtriple airlight(0.8, 0.8, 0.9);
    dmatrix *t = estimateTransmission(im, airlight);

    dmToQImage(t).save("trans" + inFile);

    std::cout << "Dehazed image " << inFile.toStdString() << std::endl;
    std::cout << "Saving to " << outFile.toStdString() << std::endl;

    dehazed.save(outFile);

    return 0;//a.exec();
}

tmatrix *removeAirlight(tmatrix *image, dmatrix *transmission, dtriple airlight)
{
    tmatrix *result = new tmatrix(image->w, image->h);

    double aR = airlight.fst;
    double aG = airlight.snd;
    double aB = airlight.trd;

    for (int i=0; i < image->w; i++) {
        for (int j=0; j < image->h; j++) {
            dtriple imP = image->pos(i,j);
            double t = transmission->pos(i,j);

            double r = (imP.fst - (1-t)*aR) / t;
            double g = (imP.snd - (1-t)*aG) / t;
            double b = (imP.trd - (1-t)*aB) / t;

            result->pos(i,j) = dtriple(r,g,b);
        }
    }

    return result;
}

dmatrix *estimateTransmission (tmatrix *image, dtriple airlight)
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

    std::cout << "Estimated eta: " << eta << std::endl;

    // Calculate the transmission (commented: shading)
    dmatrix *t = new dmatrix(h.w, h.h);
    //dmatrix *l = new dmatrix(h.w, h.h);
    for (int i=0; i<image->h; i++) {
        for (int j=0; j<image->w; j++) {
            double iaP = Ia.pos(i,j);
            double irP = Ir.pos(i,j);

            double tP = 1 - (iaP - eta * irP) / normA;

            t->pos(i,j) = tP;
            //l->pos(i,j) = irP / tP;
        }
    }

    return t;
}
