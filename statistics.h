#ifndef STATISTICS_H
#define STATISTICS_H

#include "matrix.h"
#include "triple.h"

double sqr(double x);
double norm(const triple<double,double,double> &x);

/* Simple estimator for the covariance of random varialbes
 * The given arrays contains n samples of the variables.
 */
double covar(double *xs, double *ys, int n);

/* Simple estimator for the expectation of a random variable.
 * The given array contains n samples of the variable.
 */
double expect(double *vals, int n);


/* The size of the neighborhood of a pixel,
 * used on the functions below.
 */
const extern int NEIGHB_SIZE;

/* Robust estimator for the covariance of a region
 * surrounding a pixel, using the arc distance between
 * the "angles" theta of two pixels to calculate
 * the weight.
 */
double w_covar(int center_i, int center_j,
               dmatrix *im_f, dmatrix *im_g,
               dmatrix *theta);

/* Robust estimator for the expectation of a region
 * surrounding a pixel, using the arc distance
 * between the "angles" theta of two pixels to
 * calculate the weight.
 */
double w_expect(int center_i, int center_j,
                dmatrix *im, dmatrix *theta, double Wx);


#endif // STATISTICS_H
