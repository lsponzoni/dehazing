#include <cmath>
#include "statistics.h"

double sqr(double x)
{
    return x * x;
}

double norm(const triple<double, double, double> &x) {
    return sqrt( sqr(x.fst) + sqr(x.snd) + sqr(x.trd) );
}

double avg(double *vals, int n)
{
    double sum = 0;
    for (int i=0; i < n; i++) {
        sum += vals[i];
    }

    return sum / n;
}

double cov(double *xs, double *ys, int n)
{
    double ex = avg(xs, n);
    double ey = avg(ys, n);

    double sum = 0;
    for (int i=0; i < n; i++) {
        sum += (xs[i] - ex) * (ys[i] - ey);
    }

    return sum / n;
}
