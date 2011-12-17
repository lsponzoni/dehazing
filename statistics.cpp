#include <cmath>
#include "statistics.h"

double pound(int i, int j, dmatrix*o);
void set_block(int &starti, int &startj,
               int &end_I, int &end_J,
               int i, int j, int h, int w);
double w(double ox, double oy, double deviation);
double dist(double o1, double o2);

double sqr(double x)
{
    return x * x;
}

double norm(const triple<double, double, double> &x) {
    return sqrt( sqr(x.fst) + sqr(x.snd) + sqr(x.trd) );
}

double expect(double *vals, int n)
{
    double sum = 0;
    for (int i=0; i < n; i++) {
        sum += vals[i];
    }

    return sum / n;
}

double covar(double *xs, double *ys, int n)
{
    double ex = expect(xs, n);
    double ey = expect(ys, n);

    double sum = 0;
    for (int i=0; i < n; i++) {
        sum += (xs[i] - ex) * (ys[i] - ey);
    }

    return sum / n;
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
        for(b; b < max_J; b++){
            if(a != i && b != j){
                double k = fs->pos(a,b) - ef;
                double d = gs->pos(a,b) - eg;

                sum +=  k * d * w(o->pos(i,j),o->pos(a, b), DEVIATION);
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
        for(b; b < max_J; b++){
            if(a != i && b != j){
                double k = fs-> pos(i,j);

                sum +=  k * w(o->pos(i,j),o->pos(a, b),DEVIATION);
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
        for(b; b < max_J; b++){
            if(a != i && b != j){
                sum +=  w(o->pos(i,j),o->pos(a, b),DEVIATION);
            }
        }
    }
    return sum;
}

void set_block(int &starti,int &startj, int &end_I, int &end_J, int i,int j,int h,int w){
    const int DIMENSION = 24;

    starti = i - i % DIMENSION;
    startj = j - j % DIMENSION;

    end_I = h;
    end_J = w;

    if(starti + DIMENSION < end_I){
        end_I = starti + DIMENSION;
    }

    if(startj + DIMENSION < end_J){
        end_J = startj + DIMENSION;
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
