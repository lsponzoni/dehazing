#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QWidget>

#include "gui/pixmapview.h"
#include "gui/imagemodel.h"

class MainWindow : public QWidget
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void open();
    bool open(QString filename);

    void save();
    bool save(QString filename);

    void chooseColor();

protected:
    void showEvent(QShowEvent *);

private slots:
    void dehazeImage(QImage image);
    void dehazeImage();

private:
    QString runOpenDialog();
    QString runSaveDialog();

    void setupModels();
    void setupViews();
    void setupUi();

    static const QColor defaultAirlightColor;
    QColor airlightColor;

    ImageModel *originalModel, *dehazedModel;
    PixmapView *originalView, *dehazedView;
};

#endif // CONTROLLER_H
