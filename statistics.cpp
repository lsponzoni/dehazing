#include <cmath>
#include "statistics.h"

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

