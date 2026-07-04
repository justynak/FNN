#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    // --screenshot <dir>: render every tab to PNG and exit (headless check)
    const QStringList args = a.arguments();
    const int i = args.indexOf("--screenshot");
    if (i != -1 && i + 1 < args.size())
        return w.saveScreenshots(args[i + 1]) ? 0 : 1;

    return a.exec();
}
