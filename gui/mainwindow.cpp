#include <QColorDialog>
#include <QFileDialog>
#include <QPushButton>
#include <QVBoxLayout>

#include "transmission.h"
#include "mainwindow.h"

const QColor MainWindow::defaultAirlightColor = QColor(255,255,255);

MainWindow::MainWindow(QWidget *parent) :
    QWidget(parent),
    airlightColor(defaultAirlightColor)
{
    setupModels();
    setupViews();
    setupUi();
}

MainWindow::~MainWindow()
{
    delete originalView;
    delete dehazedView;
}

void MainWindow::open()
{
    QString filename = runOpenDialog();
    open(filename);
}

bool MainWindow::open(QString filename)
{
    if ( !filename.isNull() ) {
        originalModel->load(filename);
        return true;

    } else {
        return false;
    }
}

void MainWindow::save()
{
    QString filename = runSaveDialog();
    save(filename);
}

bool MainWindow::save(QString filename)
{
    if (!filename.isNull()) {
        dehazedModel->image().save(filename);
        return true;

    } else {
        return false;
    }
}

void MainWindow::chooseColor()
{
    airlightColor = QColorDialog::getColor(airlightColor, this);
    dehazeImage();
}

void MainWindow::showEvent(QShowEvent *e)
{
    QWidget::showEvent(e);
    originalView->show();
    dehazedView->show();
}

void MainWindow::dehazeImage(QImage image)
{
    tmatrix *im = normQImage(image);
    dtriple a = normQColor(airlightColor);

    dmatrix *transm = estimateTransmissionSingleAlbedo(im, a);
    tmatrix *dehazed = removeAirlight(im, transm, a);

    dehazedModel->setImage(tmToQImage(dehazed));

    delete im;
    delete transm;
    delete dehazed;
}

void MainWindow::dehazeImage()
{
    dehazeImage(originalModel->image());
}

QString MainWindow::runOpenDialog()
{
    return QFileDialog::getOpenFileName(originalView,
                                        tr("Open Image"),
                                        QString(),
                                        tr("Image File (*.jpg *.png *.bmp); Any File")
                                       );
}

QString MainWindow::runSaveDialog()
{
    return QFileDialog::getSaveFileName(dehazedView,
                                        tr("Save image"),
                                        QString(),
                                        tr("Image File (*.jpg *.png *.bmp); Any File")
                                       );
}

void MainWindow::setupModels()
{
    originalModel = new ImageModel(this);
    connect(originalModel, SIGNAL(imageChanged(QImage)),
            this, SLOT(dehazeImage(QImage)));

    dehazedModel = new ImageModel(this);
}

void MainWindow::setupViews()
{
    originalView = new PixmapView();
    originalView->setGeometry(originalView->x(),
                              originalView->y(),
                              256, 256);
    originalView->setWindowTitle(tr("Original Image"));
    connect(originalModel, SIGNAL(pixmapChanged(QPixmap)),
            originalView, SLOT(setPixmap(QPixmap)));

    setGeometry(originalView->x() + originalView->width() + 20,
                originalView->y(),
                128, 128);

    dehazedView = new PixmapView();
    dehazedView->setWindowTitle(tr("Dehazed Image"));
    dehazedView->setGeometry(x() + width() + 20, y(),
                             256, 256);
    connect(dehazedModel, SIGNAL(pixmapChanged(QPixmap)),
            dehazedView, SLOT(setPixmap(QPixmap)));
}

void MainWindow::setupUi()
{
    QPushButton *openButton = new QPushButton(tr("Open"), this);
    connect(openButton, SIGNAL(clicked()),
            this, SLOT(open()));

    QPushButton *saveButton = new QPushButton(tr("Save"), this);
    connect(saveButton, SIGNAL(clicked()),
            this, SLOT(save()));

    QPushButton *pickButton = new QPushButton(tr("Pick Airlight Color"), this);
    connect(pickButton, SIGNAL(clicked()),
            this, SLOT(chooseColor()));

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(openButton);
    layout->addWidget(saveButton);
    layout->addWidget(pickButton);

    setLayout(layout);
}
