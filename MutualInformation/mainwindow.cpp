#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>

#include "logisticmapgenerator.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    data = QList<qreal>();

    LogisticMapGenerator generator;
    generator.SetData(&data);
    generator.Solve();

    for(int i=0; i<data.size(); ++i)
        sampleCount.append((qreal)i);

    ui->plot->addGraph();
    ui->plot->graph(0)->setData(sampleCount, data);

    ui->plot->graph(0)->setLineStyle(QCPGraph::lsNone);
    ui->plot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 1));
    ui->plot->xAxis->setLabel("x[n]");
    ui->plot->yAxis->setLabel("n");

    ui->plot->rescaleAxes();
    ui->plot->yAxis->setRange(0.0, 1.0);

    ui->plot->replot();

    connect(ui->rValueSlider, SIGNAL(valueChanged(int)), this, SLOT(on_rValueSlider_valueChanged(int)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_rValueSlider_valueChanged(int value)
{
    qreal r = (qreal)value * 0.01;
    ui->rValueLabel->setText(tr("R = %1").arg(r));

    LogisticMapGenerator generator;
    generator.SetR(r);
    generator.SetData(&data);
    generator.Solve();

    ui->plot->graph(0)->setData(sampleCount, data);

    ui->plot->graph(0)->setLineStyle(QCPGraph::lsNone);
    ui->plot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 1));
    ui->plot->xAxis->setLabel("x[n]");
    ui->plot->yAxis->setLabel("n");
    //ui->plot->rescaleAxes();

    ui->plot->replot();
}
