#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "c3ddataseries.h"
#include "c2ddataseries.h"

//#include <QwPlot3D/
//#include <QtDataVisualization>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    virtual ~MainWindow();

private:
    Ui::MainWindow *ui;
    C3DDataSeries *data;
    C2DDataSeries *dataH;
    C3DDataSeries *dataL;
};

#endif // MAINWINDOW_H
