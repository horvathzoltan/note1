#include "mainwindow.h"
#include "settings.h"
#include "common/logger/log.h"

#include <QApplication>

Settings settings;

int main(int argc, char *argv[])
{   
    QApplication a(argc, argv);
    MainWindow w;
    Log::init(MainWindow::msg, false, &w, false);
    w.show();
    return a.exec();
}
