#ifndef MATRIX_H
#define MATRIX_H

#include <QImage>
#include "triple.h"

template<class T>
/*
 * A simple matrx class, wrapper around an array.
 *
 * The array itself is on the field 'data',
 * and the dimensions are 'w', 'h'.
 *
 * The method 'pos' provides a convenient way
 * to get an set elements.
 */
struct matrix
{
    T *data;
    int w, h;

    // Creates an empty matrix
    matrix() :
        data(NULL),
        w(0),
        h(0)
    {
    }

    // Creates a matrix copying the given data.
    matrix(T *data, int w, int h) :
        data(new T[w*h]),
        w(w),
        h(h)
    {
        setData(data);
    }

    // Creates an empty matrix of given dimensions
    matrix(int w, int h) :
        data(new T[w*h]),
        w(w),
        h(h)
    {
    }

    // Copies a matrix by copying the data
    matrix(const matrix &other) :
        data(new T[w*h]),
        w(other.w),
        h(other.h)
    {
        setData(other.data);
    }

    // Deallocates memory when deleting a matrix
    ~matrix()
    {
        delete data;
    }

    inline T &pos(int i, int j)
    {
        return data[i*w + j];
    }


private:
    // Copies the given data to our data
    inline void setData(T *data)
    {
        memcpy(this->data, data, w*h*sizeof(T));
    }
};

// Common matrix types
typedef matrix<double> dmatrix;
typedef matrix<dtriple> tmatrix;

// Conversion to/from qimages
tmatrix *normQImage(QImage im);
QImage dmToQImage(dmatrix *m);
QImage tmToQImage(tmatrix *m);

#endif // MATRIX_H
