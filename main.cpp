#include <cstring>
#include <cmath>
#include <iostream>

#include <QtCore/QCoreApplication>
#include <QtGui/QImage>

#include <QString>
#include <QStringList>

#include "matrix.h"
#include "triple.h"
#include "transmission.h"


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QString inFile = QCoreApplication::arguments()[1];
    QString outFile = "dehazed" + inFile;

    QImage image(inFile);
    if (image.isNull()) {
        std::cout << "Couldn't read image " << inFile.toStdString() << std::endl;
        return 0;
    }

    tmatrix *im = normQImage(image);

    dtriple airlight(0.8, 0.8, 0.9);
    dmatrix *t = estimateTransmission(im, airlight);

    dmToQImage(t).save("trans" + inFile);

    tmatrix *dhz = removeAirlight(im, t, airlight);
    QImage dehazed = tmToQImage(dhz);

    std::cout << "Dehazed image " << inFile.toStdString() << std::endl;
    std::cout << "Saving to " << outFile.toStdString() << std::endl;

    dehazed.save(outFile);

    return 0;//a.exec();
}

//*************************************************************************
// After better redo
// Multi albedo methods
// TODO refactor code
//*************************************************************************
// both y and x are pixels
//
dmatrix *estimateTransmissionMultiAlbedo (tmatrix *image, dtriple airlight)
{
    double normA = norm(airlight);

    const double WR = -0.299, WB = -0.114, WG = -0.587, CWR = 1 + WR, CWB = 1 + WG;

    // 1. compute naive haze free In = I - A*Ia/ normA
    tmatrix In(image->w, image->h);
    dmatrix Ia(image->w, image->h);
    for (int i=0; i<image->h; i++) {
        for (int j=0; j<image->w; j++) {
            dtriple imP = image->pos(i,j);

            double dotProd = imP.fst * airlight.fst
                            + imP.snd * airlight.snd
                            + imP.trd * airlight.trd;

            // Calculate Ia
            double Ia_value = dotProd / normA;
            Ia.pos(i,j) = Ia_value;

            // Calculate In
            double r = imP.fst - airlight.fst * Ia_value / normA;
            double g = imP.snd - airlight.snd * Ia_value / normA;
            double b = imP.trd - airlight.trd * Ia_value / normA;

            In.pos(i,j) = dtriple(r,g,b);
        }
    }

    // 2. extract a single color channel o(x) = 1/tan( (Inaive .* U)/Inaive .* V)
    // ignoring shading
    dmatrix o(image->w, image->h);
    for (int i=0; i<image->h; i++) {
        for (int j=0; j<image->w; j++) {
            dtriple imP = In.pos(i,j);

            double u = imP.fst * WR
                      + imP.snd * WG
                      + imP.trd * CWB;

            double b = imP.fst * CWR
                      + imP.snd * WG
                      + imP.trd * WB;

            o.pos(i,j) = 1/tan(u / b );
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
    // TODO the methods
    dmatrix n(image->w, image->h);
    for (int i=0; i<image->h; i++) {
        for (int j=0; j<image->w; j++) {
            n.pos(i,j) = w_covar(i, j, &Ia, &h, &o)/ w_covar(i, j, &Ir, &h, &o);
        }
    }

    // Calculate the transmission (commented: shading)
    dmatrix *t = new dmatrix(h.w, h.h);
    //dmatrix *l = new dmatrix(h.w, h.h);
    for (int i=0; i<image->h; i++) {
        for (int j=0; j<image->w; j++) {
            double iaP = Ia.pos(i,j);
            double irP = Ir.pos(i,j);
            double nP = n.pos(i,j);

            double tP = 1 - (iaP - nP * irP) / normA;

            t->pos(i,j) = tP;
            //l->pos(i,j) = irP / tP;
        }
    }

    // this should still be refined by a markov chain
    return t;
}

