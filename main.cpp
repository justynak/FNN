#include "mainwindow.h"
#include <QApplication>

#include "c2ddataseries.h"
int main(int argc, char *argv[])
{
    //QApplication a(argc, argv);
    //MainWindow w;
    //w.show();

    C2DDataSeries series;
    QVector2D p = QVector2D(0.0, 1.0);
    series.AttachPoint(p);

    return 0;//a.exec();
}
