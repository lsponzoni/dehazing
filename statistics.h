#ifndef STATISTICS_H
#define STATISTICS_H

#include "triple.h"

double sqr(double x);
double norm(const triple<double,double,double> &x);

double avg(double *vals, int n);
double cov(double *xs, double *ys, int n);

#endif // STATISTICS_H
