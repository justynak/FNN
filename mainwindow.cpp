#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "qcustomplot.h"
#include "cikedagenerator.h"
#include "chenongenerator.h"
#include "clorentzgenerator.h"
#include "c3ddataseries.h"

#include <mgl2/qmathgl.h>
#include <mgl2/plot.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    CIkedaGenerator* gen = new CIkedaGenerator(1.0, 0.9, 0.1, 8.0, QVector3D(0.5, 0.8, 0), 100000);
    gen->Solve();

    qDebug() << "Done\n";

    data = gen->GetData();
    QList<double>x = data->GetXLine();
    QList<double>y = data->GetYLine();
    QList<double>z = data->GetZLine();

    ui->plot->addGraph();
    ui->plot->graph(0)->setData(x, y);

    ui->plot->xAxis->setLabel("x");
    ui->plot->yAxis->setLabel("y");

    ui->plot->graph(0)->setLineStyle(QCPGraph::lsNone);
    ui->plot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 1));
    ui->plot->graph(0)->rescaleAxes();



    CHenonGenerator* genH = new CHenonGenerator();
    genH->Solve();
    dataH = genH->GetData();
    QList<double>xH = dataH->GetXLine();
    QList<double>yH = dataH->GetYLine();

    ui->plot2->addGraph();
    ui->plot2->graph(0)->setData(xH, yH);

    ui->plot2->xAxis->setLabel("x");
    ui->plot2->yAxis->setLabel("y");

    ui->plot2->graph(0)->setLineStyle(QCPGraph::lsNone);
    ui->plot2->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 1));
    ui->plot2->graph(0)->rescaleAxes();

    CLorentzGenerator *genL = new CLorentzGenerator();
    genL->Solve();
    dataL = genL->GetData();
    QList<double>xL = dataL->GetXLine();
    QList<double>yL = dataL->GetYLine();

    ui->plot3->addGraph();
    ui->plot3->graph(0)->setData(xL, yL);

    ui->plot3->xAxis->setLabel("x");
    ui->plot3->yAxis->setLabel("y");

    ui->plot3->graph(0)->setLineStyle(QCPGraph::lsNone);
    ui->plot3->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 1));
    ui->plot3->graph(0)->rescaleAxes();

    ui->plot->replot();
    ui->plot2->replot();
    ui->plot3->replot();

    delete genH;
    delete genL;
    delete gen;
}


MainWindow::~MainWindow()
{
    delete ui;
    delete data;
    delete dataH;
    delete dataL;
}
