#include <cmath>
#include <iostream>

#include "transmission.h"

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
    double eta = covar(Ia.data, h.data, length) / covar(Ir.data, h.data, length);

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
