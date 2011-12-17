#include <cmath>
#include "statistics.h"

// Sum the weights of the pixels in a neighborhood
double sum_weights(int center_i, int center_j, dmatrix *theta);

// 4.1 w(x,y) = exp( - dist( O(x), O(y))^2/ deviation ^ 2)
double weight(double theta_x, double theta_y);

// Arc distance between two angles (in RADIANS)
double arc_dist(double alpha, double beta);

// Obtain the boundaries of a pixel neighborhood
inline void get_boundaries(int center_i, int center_j,
                           int width, int height,
                           int &init_i, int &init_j,
                           int &end_i, int &end_j);

const int NEIGHB_SIZE = 24;
const double DEVIATION_SQ = (1.0/(8*8));
const double PI = atan(1.0) * 4.0;

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

double w_covar(int center_i, int center_j,
               dmatrix *im_f, dmatrix *im_g,
               dmatrix *theta)
{
    // Obtain the sum of all weights
    double Wx = sum_weights(center_i, center_j, theta);

    // Obtain the expectations on the same region
    double Ef = w_expect(center_i, center_j, im_f, theta, Wx);
    double Eg = w_expect(center_i, center_j, im_g, theta, Wx);

    // Obtain the boundaries
    int init_i, init_j, end_i, end_j;
    get_boundaries(center_i, center_j, im_f->w, im_f->h,
                   init_i, init_j, end_i, end_j);

    // Constant values, calculate only once
    double center_theta = theta->pos(center_i, center_j);

    // Sum over all the pixels in the region
    double sum = 0;
    for (int i=init_i; i < end_i; i++) {
        for (int j=init_j; j < end_j; j++) {

            double f_part = im_f->pos(i, j) - Ef;
            double g_part = im_g->pos(i, j) - Eg;
            double thet   = theta->pos(i, j);

            sum +=  f_part * g_part * weight(thet, center_theta);
        }
    }

    return sum/ Wx;
}

double w_expect(int center_i, int center_j,
                dmatrix *image, dmatrix *theta, double Wx)
{
    // Obtain the boundaries
    int init_i, init_j, end_i, end_j;
    get_boundaries(center_i, center_j, image->w, image->h,
                   init_i, init_j, end_i, end_j);

    // Constant values, calculate only once
    double center_theta = theta->pos(center_i, center_j);

    // Sum over all pixels in the region
    double sum = 0;
    for (int i=init_i; i < end_i; i++) {
        for (int j=init_j; j < end_j; j++) {

            double pix = image->pos(i, j);
            double curr_theta = theta->pos(i, j);

            sum +=  pix * weight(curr_theta, center_theta);
        }
    }

    return sum / Wx;
}

double sum_weights(int center_i, int center_j, dmatrix *theta)
{
    // Obtain the boundaries
    int init_i, init_j, end_i, end_j;
    get_boundaries(center_i, center_j, theta->w, theta->h,
                   init_i, init_j, end_i, end_j);

    // Constant values, calculate only once
    double center_theta = theta->pos(center_i, center_j);

    // Sum over all pixels in the region
    double sum = 0;
    for(int i = init_i; i < end_i; i++) {
        for(int j = init_j; j < end_j; j++) {

            double curr_theta = theta->pos(i, j);
            sum += weight(curr_theta, center_theta);
        }
    }

    return sum;
}

// Changes made
// 1. It doesn't make any sense to consider a thorus in this application
// 2. Clamping square seems to make more sense
// 3. init_i + neight_size in a ciclic situation will actually do a loop
// 4. No wrap_i and j in the functions calling this
void get_boundaries(int center_i, int center_j,
                    int width, int height,
                    int &init_i, int &init_j,
                    int &end_i, int &end_j)
{
    init_i = center_i - NEIGHB_SIZE/2;
    init_j = center_j - NEIGHB_SIZE/2;

    end_i  = init_i + NEIGHB_SIZE;
    end_j  = init_j + NEIGHB_SIZE;

    // Clamp the boundaries to figure dimension
    if (init_i < 0) init_i = 0;
    if (init_j < 0) init_j = 0;

    if (end_i > height) end_i = height;
    if (end_j > width) end_j = width;

}

// 4.1 w(x,y) = exp( - dist( O(x), O(y))^2/ deviation ^ 2)
double weight(double theta_x, double theta_y){
    double dist = arc_dist(theta_x, theta_y);
    double frac = -(dist * dist) / DEVIATION_SQ;
    return exp(frac);
}

double arc_dist(double alpha, double beta){
    double dist1 = fabs(alpha - beta);
    double dist2 = 2*PI - dist1;

    return std::min(dist1, dist2);
}
