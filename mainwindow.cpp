#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <vector>

#include "qcustomplot.h"
#include "core/henon.h"
#include "core/ikeda.h"
#include "core/lorenz.h"

namespace {

// Shows an orbit as an x-vs-y scatter plot. Works for any point type with
// .x/.y members, so 3D orbits are projected onto the x-y plane.
template <class Point>
void plotScatter(QCustomPlot *plot, const std::vector<Point> &orbit)
{
    QList<double> x, y;
    x.reserve(int(orbit.size()));
    y.reserve(int(orbit.size()));
    for (const Point &p : orbit) {
        x.append(p.x);
        y.append(p.y);
    }

    plot->addGraph();
    plot->graph(0)->setData(x, y);
    plot->xAxis->setLabel("x");
    plot->yAxis->setLabel("y");
    plot->graph(0)->setLineStyle(QCPGraph::lsNone);
    plot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 1));
    plot->graph(0)->rescaleAxes();
    plot->replot();
}

} // namespace

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    plotScatter(ui->plot, fnn::ikeda_orbit({0.5, 0.8}, 100000));
    plotScatter(ui->plot2, fnn::henon_orbit({1.2, 1.3}, 10000));
    plotScatter(ui->plot3, fnn::lorenz_orbit({1.2, 1.3, 1.6}, 0.005, 100000));
}

MainWindow::~MainWindow()
{
    delete ui;
}
