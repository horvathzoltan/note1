#include <QApplication>

#include "common/logger/log.h"
#include "common/helper/settings/settingshelper.h"

#include "mainwindow.h"
#include "settings.h"
#include "filehelper.h"
#include "filenamehelper.h"
#include "filesystemmodelhelper.h"

Settings settings;
Environment environment;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    auto fn = FilenameHelper::GetIni();
    com::helper::SettingsHelper::init(fn, &settings);
    com::helper::SettingsHelper::loadSettings();

    QFileSystemModel* fm = new QFileSystemModel();

    FileSystemModelHelper::init(fm);

    auto projectDir =  FileHelper::TakeProjectDir();
    FileSystemModelHelper::setRootPath(projectDir);

    MainWindow w;
    w.updateFileTreeView();

    Log::init(MainWindow::msg, false, &w, false);
    w.show();
    auto r = a.exec();
    com::helper::SettingsHelper::saveSettings();

    delete fm;
    return r;
}



