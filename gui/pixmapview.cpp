#include <QVBoxLayout>
#include "gui/pixmapview.h"

PixmapView::PixmapView(QWidget *parent) :
    QWidget(parent)
{
    setupUi(QPixmap());
}

PixmapView::PixmapView(QPixmap pixmap, QWidget *parent) :
    QWidget(parent)
{
    setupUi(pixmap);
}

void PixmapView::setPixmap(QPixmap pixmap)
{
    imageLabel->setPixmap(pixmap);
    imageLabel->setFixedSize(pixmap.width(), pixmap.height());
}

void PixmapView::setupUi(QPixmap initial)
{
    imageLabel = new QLabel;
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel->setScaledContents(true);
    setPixmap(initial);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(imageLabel);

    setLayout(mainLayout);
}
