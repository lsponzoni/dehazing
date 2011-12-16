#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>

#include "gui/pixmapview.h"
#include "gui/imagemodel.h"

class Controller : public QObject
{
    Q_OBJECT
public:
    explicit Controller(QObject *parent = 0);
    ~Controller();

public slots:
    void open();
    bool open(QString filename);

    void save();
    bool save(QString filename);

private slots:
    void dehazeImage(QImage image);
    void dehazeImage();

private:
    QString runOpenDialog();
    QString runSaveDialog();

    void setupModels();
    void setupUi();

    ImageModel *originalModel, *dehazedModel;
    PixmapView *originalView, *dehazedView;
};

#endif // CONTROLLER_H
