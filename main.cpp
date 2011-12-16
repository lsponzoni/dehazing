#include <cstring>
#include <cmath>
#include <iostream>

#include <QtGui/QApplication>
#include <QtGui/QImage>

#include <QString>
#include <QStringList>

#include "gui/controller.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Controller control;
    return a.exec();
}
