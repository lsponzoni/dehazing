#ifndef PIXMAPPROXY_H
#define PIXMAPPROXY_H

#include <QLabel>
#include <QPixmap>
#include <QScrollArea>
#include <QWidget>

class PixmapView : public QWidget
{
    Q_OBJECT
public:
    explicit PixmapView(QWidget *parent = 0);
    PixmapView(QPixmap pixmap, QWidget *parent = 0);

public slots:
    void setPixmap(QPixmap pixmap);

private:
    QLabel *imageLabel;

    void setupUi(QPixmap initial);
};

#endif // PIXMAPPROXY_H
