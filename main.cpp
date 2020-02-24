#include "filenamehelper.h"
#include "mainwindow.h"
#include "settings.h"
#include "common/logger/log.h"
#include <QApplication>
#include "common/helper/settings/settingshelper.h"

Settings settings;

int main(int argc, char *argv[])
{           
    QApplication a(argc, argv);    
    com::helper::SettingsHelper::init(FilenameHelper::GetIni(), &settings);    
    com::helper::SettingsHelper::loadSettings();
    MainWindow w;
    Log::init(MainWindow::msg, false, &w, false);
    w.show();
    auto r = a.exec();
    com::helper::SettingsHelper::saveSettings();
    return r;
}
