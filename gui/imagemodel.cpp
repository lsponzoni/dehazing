#include "gui/imagemodel.h"

ImageModel::ImageModel(QObject *parent) :
    QObject(parent)
{
}

ImageModel::ImageModel(QImage image, QObject *parent) :
    QObject(parent)
{
    setImage(image);
}

ImageModel::ImageModel(QString filename, const char *format, QObject *parent) :
    QObject(parent)
{
    load(filename, format);
}

QImage ImageModel::image()
{
    return myImage;
}

QPixmap ImageModel::pixmap()
{
    return myPixmap;
}

void ImageModel::load(QString filename, const char *format)
{
    setImage(QImage(filename, format));
}

void ImageModel::setImage(QImage image)
{
    myImage = image.convertToFormat(QImage::Format_RGB32);
    emit imageChanged(myImage);

    myPixmap = QPixmap::fromImage(image);
    emit pixmapChanged(myPixmap);
}

void ImageModel::setPixmap(QPixmap pixmap)
{
    myImage = pixmap.toImage().convertToFormat(QImage::Format_RGB32);
    emit imageChanged(myImage);

    myPixmap = pixmap;
    emit pixmapChanged(myPixmap);
}
