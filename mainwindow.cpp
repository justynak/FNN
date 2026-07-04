#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <vector>

#include "qcustomplot.h"
#include "core/fnn.h"
#include "core/henon.h"
#include "core/ikeda.h"
#include "core/lorenz.h"
#include "core/mutual_information.h"

namespace {

void addTitle(QCustomPlot *plot, const QString &title)
{
    plot->plotLayout()->insertRow(0);
    plot->plotLayout()->addElement(0, 0, new QCPPlotTitle(plot, title));
}

void scatterPlot(QCustomPlot *plot, const QList<double> &x, const QList<double> &y,
                 const QString &title, const QString &xLabel, const QString &yLabel)
{
    plot->addGraph();
    plot->graph(0)->setData(x, y);
    plot->graph(0)->setLineStyle(QCPGraph::lsNone);
    plot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 1));
    plot->xAxis->setLabel(xLabel);
    plot->yAxis->setLabel(yLabel);
    addTitle(plot, title);
    plot->rescaleAxes();
    plot->replot();
}

// x-vs-y scatter of an orbit; 3D orbits are projected onto the x-y plane.
template <class Point>
void scatterOrbit(QCustomPlot *plot, const std::vector<Point> &orbit,
                  const QString &title)
{
    QList<double> x, y;
    x.reserve(int(orbit.size()));
    y.reserve(int(orbit.size()));
    for (const Point &p : orbit) {
        x.append(p.x);
        y.append(p.y);
    }
    scatterPlot(plot, x, y, title, "x", "y");
}

// Line-plus-marker plot of a curve sampled at integer x starting at firstX,
// with a dashed vertical marker at markerX.
void curvePlot(QCustomPlot *plot, const std::vector<double> &values, int firstX,
               int markerX, const QString &title, const QString &xLabel,
               const QString &yLabel)
{
    QList<double> x, y;
    x.reserve(int(values.size()));
    y.reserve(int(values.size()));
    for (size_t i = 0; i < values.size(); ++i) {
        x.append(firstX + int(i));
        y.append(values[i]);
    }

    plot->addGraph();
    plot->graph(0)->setData(x, y);
    plot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 5));
    plot->xAxis->setLabel(xLabel);
    plot->yAxis->setLabel(yLabel);
    addTitle(plot, title);

    QCPItemStraightLine *marker = new QCPItemStraightLine(plot);
    plot->addItem(marker);
    marker->point1->setCoords(markerX, 0.0);
    marker->point2->setCoords(markerX, 1.0);
    marker->setPen(QPen(Qt::red, 1, Qt::DashLine));

    plot->rescaleAxes();
    plot->yAxis->setRangeLower(0.0);
    plot->replot();
}

} // namespace

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupAttractorsTab();
    setupTakensTab();
}

void MainWindow::setupAttractorsTab()
{
    scatterOrbit(ui->ikedaPlot, fnn::ikeda_orbit({0.5, 0.8}, 100000),
                 "Ikeda map");
    scatterOrbit(ui->henonPlot, fnn::henon_orbit({1.2, 1.3}, 10000),
                 "Hénon map");
    scatterOrbit(ui->lorenzPlot, fnn::lorenz_orbit({1.2, 1.3, 1.6}, 0.005, 100000),
                 "Lorenz system");
}

void MainWindow::setupTakensTab()
{
    // The whole pipeline on a single scalar coordinate: Lorenz x is all
    // Takens needs to rebuild the attractor.
    const auto orbit = fnn::lorenz_orbit({1.2, 1.3, 1.6}, 0.01, 6500);
    std::vector<double> signal;
    QList<double> trueX, trueZ;
    for (size_t i = 500; i < orbit.size(); ++i) { // drop the transient
        signal.push_back(orbit[i].x);
        trueX.append(orbit[i].x);
        trueZ.append(orbit[i].z);
    }

    const auto ami = fnn::ami_curve(signal, 40, 16);
    int tau = fnn::first_local_minimum(ami);
    if (tau < 1)
        tau = 10; // no minimum found; fall back to something plottable

    // FNN needs fewer points to converge than the scatter plots need to
    // look dense; a shorter signal keeps the O(N^2) search off the
    // startup path.
    const std::vector<double> fnnSignal(signal.begin(), signal.begin() + 2500);
    const auto fnnFractions = fnn::fnn_curve(fnnSignal, 5, tau);
    int m = int(fnnFractions.size());
    for (size_t d = 0; d < fnnFractions.size(); ++d)
        if (fnnFractions[d] < 0.02) {
            m = int(d) + 1;
            break;
        }

    scatterPlot(ui->truePlot, trueX, trueZ,
                "Lorenz attractor", "x", "z");

    QList<double> reconX, reconY;
    for (size_t i = 0; i + tau < signal.size(); ++i) {
        reconX.append(signal[i]);
        reconY.append(signal[i + tau]);
    }
    scatterPlot(ui->reconPlot, reconX, reconY,
                QString("Reconstructed from x alone (τ = %1)").arg(tau),
                "x(t)", QString("x(t + %1)").arg(tau));

    curvePlot(ui->amiPlot, ami, 0, tau,
              "Average mutual information", "lag [samples]", "AMI [bits]");
    curvePlot(ui->fnnPlot, fnnFractions, 1, m,
              "False nearest neighbors", "embedding dimension",
              "false-neighbor fraction");
    ui->fnnPlot->xAxis->setAutoTickStep(false);
    ui->fnnPlot->xAxis->setTickStep(1.0); // dimensions are integers
    ui->fnnPlot->replot();

    ui->statusBar->showMessage(
        QString("τ = %1 samples (first AMI minimum) · m = %2 (first dimension "
                "with < 2%% false neighbors)").arg(tau).arg(m));
}

bool MainWindow::saveScreenshots(const QString &dir)
{
    bool ok = true;
    for (int tab = 0; tab < ui->tabs->count(); ++tab) {
        ui->tabs->setCurrentIndex(tab);
        QCoreApplication::processEvents();
        const QString file = QString("%1/tab%2.png").arg(dir).arg(tab);
        ok = grab().save(file) && ok;
    }
    return ok;
}

MainWindow::~MainWindow()
{
    delete ui;
}
