#ifndef IMAGEMODEL_H
#define IMAGEMODEL_H

#include <QImage>
#include <QObject>
#include <QPixmap>

class ImageModel : public QObject
{
    Q_OBJECT
public:
    explicit ImageModel(QObject *parent = 0);
    ImageModel(QImage image, QObject *parent = 0);
    ImageModel(QString filename, const char *format = 0, QObject *parent = 0);

    QImage image();
    QPixmap pixmap();

    void load(QString filename, const char *format=NULL);

signals:
    void imageChanged(QImage image);
    void pixmapChanged(QPixmap pixmap);

public slots:
    void setImage(QImage image);
    void setPixmap(QPixmap pixmap);

private:
    QImage myImage;
    QPixmap myPixmap;
};

#endif // IMAGEMODEL_H
