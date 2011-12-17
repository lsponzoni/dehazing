#include <cstring>
#include <cmath>
#include <iostream>

#include <QtGui/QApplication>
#include <QtGui/QImage>

#include <QString>
#include <QStringList>

#include "gui/mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow win;
    win.show();

    return app.exec();
}
