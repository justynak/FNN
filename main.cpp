#include "mainwindow.h"
#include <QApplication>

#include <qdebug.h>
#include "c2ddataseries.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    //C2DDataSeries series;
    //QVector2D p = QVector2D(0.0, 1.0);
    //series.AttachPoint(p);



    return a.exec();
}
