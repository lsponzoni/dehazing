#include <cstring>
#include <cmath>
#include <iostream>

#include <QtCore/QCoreApplication>
#include <QtGui/QImage>

#include <QString>
#include <QStringList>

#include "matrix.h"
#include "triple.h"
#include "transmission.h"


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QString inFile = QCoreApplication::arguments()[1];
    QString outFile = "dehazed" + inFile;

    QImage image(inFile);
    QImage dehazed = image;
    tmatrix *im = normQImage(image);

    dtriple airlight(0.8, 0.8, 0.9);
    dmatrix *t = estimateTransmission(im, airlight);

    dmToQImage(t).save("trans" + inFile);

    std::cout << "Dehazed image " << inFile.toStdString() << std::endl;
    std::cout << "Saving to " << outFile.toStdString() << std::endl;

    dehazed.save(outFile);

    return 0;//a.exec();
}
