#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "qcustomplot.h"
#include "cikedagenerator.h"
#include "c3ddataseries.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    CIkedaGenerator* gen = new CIkedaGenerator(1.0, 0.9, 0.1, 8.0, QVector3D(0.5, 0.8, 0), 1000);
    gen->Solve();

    qDebug() << "Done\n";

    data = gen->GetData();
    QList<double>x = data->GetXLine();
    QList<double>y = data->GetYLine();

    ui->plot->addGraph();
    ui->plot->graph(0)->setData(x, y);

    ui->plot->xAxis->setLabel("x");
    ui->plot->yAxis->setLabel("y");

    ui->plot->graph(0)->setLineStyle(QCPGraph::lsNone);
    ui->plot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 1));
    ui->plot->graph(0)->rescaleAxes();

    ui->plot->replot();
}


MainWindow::~MainWindow()
{
    delete ui;
    delete data;
}
