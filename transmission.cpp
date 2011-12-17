#include <cmath>
#include <iostream>

#include "transmission.h"

inline void projectOntoAirlight(tmatrix *image, dtriple airlight,
                                dmatrix *&Ia, dmatrix *&Ir,
                                dmatrix *&h, tmatrix *&naive)
{
    double air_norm = norm(airlight);

    // Allocate the memory
    Ia = new dmatrix(image->w, image->h);
    Ir = new dmatrix(image->w, image->h);
    h = new dmatrix(image->w, image->h);
    naive = new tmatrix(image->w, image->h);

    for (int i=0; i < image->h; i++) {
        for (int j=0; j < image->w; j++) {
            // Extract values from image
            dtriple im_pix = image->pos(i,j);
            double im_norm = norm(im_pix);

            // Calculate Ia
            double dotProd = im_pix.fst * airlight.fst
                            + im_pix.snd * airlight.snd
                            + im_pix.trd * airlight.trd;
            double ia_val = dotProd/air_norm;
            Ia->pos(i,j) = ia_val;

            // Calculate Ir
            double ir_val = sqrt( sqr(im_norm) - sqr(ia_val) );
            Ir->pos(i,j) = ir_val;

            // Calculate h
            h->pos(i,j) = (air_norm - ia_val) / ir_val;

            // Calculate naive dehazed
            double r = im_pix.fst - airlight.fst * ia_val / air_norm;
            double g = im_pix.snd - airlight.snd * ia_val / air_norm;
            double b = im_pix.trd - airlight.trd * ia_val / air_norm;
            naive->pos(i,j) = dtriple(r,g,b);
        }
    }
}

dmatrix *estimateTransmissionSingleAlbedo (tmatrix *image, dtriple airlight)
{
    double normA = norm(airlight);

    // Decompose the image, calculate parameter 'h'
    dmatrix *Ia, *Ir, *h;
    tmatrix *naive;
    projectOntoAirlight(image, airlight, Ia, Ir, h, naive);

    // Estimate the eta
    int length = h->w * h->h;
    double eta = covar(Ia->data, h->data, length) / covar(Ir->data, h->data, length);

    std::cout << "Estimated eta: " << eta << std::endl;

    // Calculate the transmission (commented: shading)
    dmatrix *t = new dmatrix(h->w, h->h);
    //dmatrix *l = new dmatrix(h->w, h->h);
    for (int i=0; i < image->h; i++) {
        for (int j=0; j < image->w; j++) {
            double ia_val = Ia->pos(i,j);
            double ir_val = Ir->pos(i,j);

            double trans = 1 - (ia_val - eta * ir_val) / normA;

            t->pos(i,j) = trans;
            //l->pos(i,j) = irP / tP;
        }
    }

    delete Ia, delete Ir, delete h, delete naive;
    return t;
}

dmatrix *estimateTransmissionMultiAlbedo (tmatrix *image, dtriple airlight)
{
    double normA = norm(airlight);

    // Compute Ia, Ir, h and naive haze-free image
    dmatrix *Ia, *Ir, *h;
    tmatrix *naive;
    projectOntoAirlight(image, airlight, Ia, Ir, h, naive);

    // 2. extract a single color channel, ignoring shading:
    // theta(x) = 1 / tan( Inaive(x).U / Inaive(x).V )
    dmatrix theta(image->w, image->h);
    const double WR = -0.299, WB = -0.114, WG = -0.587,
            CWR = 1 + WR, CWB = 1 + WG;
    for (int i=0; i < image->h; i++) {
        for (int j=0; j < image->w; j++) {
            dtriple pix = naive->pos(i,j);

            double u = pix.fst * WR
                    + pix.snd * WG
                    + pix.trd * CWB;

            double v = pix.fst * CWR
                    + pix.snd * WG
                    + pix.trd * WB;

            theta.pos(i,j) = 1/tan(u / v);
        }
    }

    // calculate the etas per pixel
    dmatrix eta(image->w, image->h);
    for (int i=0; i<image->h; i++) {
        for (int j=0; j<image->w; j++) {
            double cov_a = w_covar(i, j, Ia, h, &theta);
            double cov_r = w_covar(i, j, Ir, h, &theta);
            eta.pos(i,j) = cov_a / cov_r;
        }
    }

    // Calculate the transmission (commented: shading)
    dmatrix *t = new dmatrix(h->w, h->h);
    //dmatrix *l = new dmatrix(h->w, h->h);
    for (int i=0; i < image->h; i++) {
        for (int j=0; j < image->w; j++) {
            double ia_val = Ia->pos(i,j);
            double ir_val = Ir->pos(i,j);
            double eta_val = eta.pos(i,j);

            double trans = 1 - (ia_val - eta_val * ir_val) / normA;
            t->pos(i,j) = trans;

            //l->pos(i,j) = irP / tP;
        }
    }

    // this should still be refined by a markov chain

    delete Ia, delete Ir, delete h, delete naive;
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
