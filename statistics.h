#ifndef STATISTICS_H
#define STATISTICS_H

#include "matrix.h"
#include "triple.h"

double sqr(double x);
double norm(const triple<double,double,double> &x);

/* Simple estimator for the expectation of a random variable.
 * The given array contains n samples of the variable.
 */
double expect(double *vals, int n);

/* Simple estimator for the covariance of random varialbes
 * The given arrays contains n samples of the variables.
 */
double covar(double *xs, double *ys, int n);


double corr(int i, int j, dmatrix *fs, dmatrix *gs, dmatrix *o);

double expectation(int i, int j, dmatrix *fs, dmatrix *o, double Wx);


#endif // STATISTICS_H
