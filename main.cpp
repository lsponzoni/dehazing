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

// template
double corr(int i, int j, dmatrix *fs, dmatrix *gs, dmatrix *o);
double expectation(int i, int j, dmatrix *fs, dmatrix *o, double Wx);
double pound(int i, int j, dmatrix *o);
void set_block(int &starti,int &startj, int &end_I, int &end_J, int i,int j,int h,int w);
double w(double ox, double oy, double deviation);
double dist(double o1,double o2);

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

    dmatrix n(image->w, image->h);
    for (int i=0; i<image->h; i++) {
        for (int j=0; j<image->w; j++) {
            n.pos(i,j) = corr(i, j, &Ia, &h, &o)/ corr(i, j, &Ir, &h, &o);
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
// 4. robust estimator for Correlation
// 4.1 w(x,y) = exp( - dist( O(x), O(y))^2/ deviation ^ 2) <could make a map here for better performance>
// 4.2 Wx = Sum(w(x,y))
// 4.3 Ex(f) = 1/Wx*(sum f(y) * w(x,y) )
// 4.4 Correlation = (sum (f(y) - E(x,y,f))(g(y) - E(x,y,g) * w(x,y))/Wx
// 4.5 Substitute those equations where they where used on constant albedo to have an aproximation of t
// 4.6
// 4.7
// 4.8
//******************************************
// Multi albedo
// TODO i got problems thinking on acessing the positions through pointers to data
// so i'm passing the matrix's and positions
//
// const int DIMENSION = 24;
const double DEVIATION = 1/8;
const double PI = 3.14;

double corr(int i, int j, dmatrix *fs, dmatrix *gs, dmatrix *o){
    double Wx = pound(i, j, o);

    double ef = expectation(i, j, fs, o, Wx) ;
    double eg = expectation(i, j, gs, o, Wx) ;

    int a,b, max_I, max_J;
    set_block(a,b, max_I, max_J, i, j, o->h, o->w);

    double sum = 0;
    for(a; a < max_I; a++){
        for(int c = b; c < max_J; c++){
            if(a != i && c != j){
                double k = fs->pos(a,c) - ef;
                double d = gs->pos(a,c) - eg;

                sum +=  k * d * w(o->pos(i,j),o->pos(a, c), DEVIATION);
            }
        }
    }

    return sum/ Wx;
}

double expectation(int i, int j, dmatrix *fs, dmatrix *o, double Wx){

    int a,b, max_I, max_J;
    set_block(a , b, max_I, max_J, i, j, o->h, o->w);

    // for each pixel in neighborhood of say 24 x 24
    double sum = 0;

    for(a; a < max_I; a++){
        for(int c = b; c < max_J; c++){
            if(a != i && c != j){
                double k = fs-> pos(i,j);

                sum +=  k * w(o->pos(i,j),o->pos(a, c),DEVIATION);
            }
        }
    }
    return sum/ Wx;
}

double pound(int i, int j, dmatrix *o){

    int a, b, max_I, max_J;
    set_block(a, b, max_I, max_J, i, j, o->h, o->w);

    double sum = 0;
    for(a; a < max_I; a++){
        for(int c = b; c < max_J; c++){
            if(a != i && c != j){
                sum +=  w(o->pos(i,j),o->pos(a, c),DEVIATION);
            }
        }
    }
    return sum;
}

void set_block(int &starti,int &startj, int &end_I, int &end_J, int i,int j,int h,int w){
    const int DIMENSION = 24;

    starti = i - DIMENSION/2;
    startj = j - DIMENSION/2;

    end_I = h;
    end_J = w;

    if(i + DIMENSION/2 < end_I){
        end_I = i + DIMENSION/2;
    }

    if(j + DIMENSION/2 < end_J){
        end_J = j + DIMENSION/2;
    }
}

// 4.1 w(x,y) = exp( - dist( O(x), O(y))^2/ deviation ^ 2)
double w(double ox, double oy, double deviation){
    double d = (dist( ox, oy ) /  deviation);
    return exp( - d * d);
}

// 3. map pixel chroma into angles dist(o1,o2) = min |o1 - o2|, 2pi - |o1 - o2|
double dist(double o1,double o2){
    double a1 = fabs(o1 - o2);
    double a2 = 2*PI - a1;

    if (a1 > a2)
        return a2;
    else
        return a1;
}


//
//
//
