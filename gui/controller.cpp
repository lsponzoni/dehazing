#include <QFileDialog>

#include "controller.h"

Controller::Controller(QObject *parent) :
    QObject(parent)
{
    setupModels();
    setupUi();
}

Controller::~Controller()
{
    delete originalView;
    delete dehazedView;
}

void Controller::open()
{
    QString filename = runOpenDialog();
    open(filename);
}

bool Controller::open(QString filename)
{
    if ( !filename.isNull() ) {
        originalModel->load(filename);
        return true;

    } else {
        return false;
    }
}

void Controller::save()
{
    QString filename = runSaveDialog();
    save(filename);
}

bool Controller::save(QString filename)
{
    if (!filename.isNull()) {
        dehazedModel->image().save(filename);
        return true;

    } else {
        return false;
    }
}

void Controller::dehazeImage(QImage image)
{
    // TODO
}

void Controller::dehazeImage()
{
    dehazeImage(originalModel->image());
}

QString Controller::runOpenDialog()
{
    return QFileDialog::getOpenFileName(originalView,
                                        tr("Open Image"),
                                        QString(),
                                        tr("Image File (*.jpg *.png)")
                                       );
}

QString Controller::runSaveDialog()
{
    return QFileDialog::getSaveFileName(dehazedView,
                                        tr("Save image"),
                                        QString(),
                                        tr("Image File (*.jpg *.png)")
                                       );
}

void Controller::setupModels()
{
    originalModel = new ImageModel(this);
    connect(originalModel, SIGNAL(imageChanged(QImage)),
            this, SLOT(dehazeImage(QImage)));

    dehazedModel = new ImageModel(this);
}

void Controller::setupUi()
{
    originalView = new PixmapView();
    originalView->setGeometry(originalView->x(),
                              originalView->y(),
                              256, 256);
    originalView->setWindowTitle(tr("Original Image"));
    originalView->show();
    connect(originalModel, SIGNAL(pixmapChanged(QPixmap)),
            originalView, SLOT(setPixmap(QPixmap)));

    dehazedView = new PixmapView();
    dehazedView->setWindowTitle(tr("Dehazed Image"));
    dehazedView->setGeometry(originalView->x() + 270,
                             dehazedView->y(),
                             256, 256);
    dehazedView->show();
    connect(dehazedModel, SIGNAL(pixmapChanged(QPixmap)),
            originalView, SLOT(setPixmap(QPixmap)));
}
