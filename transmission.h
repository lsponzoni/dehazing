#ifndef TRANSMISSION_H
#define TRANSMISSION_H

#include "matrix.h"
#include "triple.h"
#include "statistics.h"

dmatrix *estimateTransmissionSingleAlbedo(tmatrix *image, dtriple airlight);
dmatrix *estimateTransmissionMultiAlbedo(tmatrix *image, dtriple airlight);

tmatrix *removeAirlight(tmatrix *image, dmatrix *transmission, dtriple airlight);

#endif // TRANSMISSION_H
