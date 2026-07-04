#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    virtual ~MainWindow();

    // Renders every tab to <dir>/tab<N>.png; used for headless verification.
    bool saveScreenshots(const QString &dir);

private:
    void setupAttractorsTab();
    void setupLogisticTab();
    void setupTakensTab();
    void updateLogisticPlot(int sliderValue);

    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
