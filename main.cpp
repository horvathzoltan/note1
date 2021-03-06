#include <QApplication>
#include <QProcess>

#include "common/logger/log.h"
#include "common/helper/settings/settingshelper.h"

#include "mainwindow.h"
#include "settings.h"
#include "filehelper.h"
#include "filenamehelper.h"
#include "filesystemmodelhelper.h"
#include "processhelper.h"

Settings settings;
Environment environment;

int main(int argc, char *argv[])
{    
    QApplication a(argc, argv);
    /*
     * Note: The use of Q_INIT_RESOURCE() and Q_CLEANUP_RESOURCE() is not necessary when the resource is built as part of the application.
    */
    //Q_INIT_RESOURCE(note1);

    auto fn = FilenameHelper::GetIni();
    com::helper::SettingsHelper::init(fn, &settings);
    com::helper::SettingsHelper::loadSettings();

    auto* fm = new QFileSystemModel();

    FileSystemModelHelper::init(fm);

    auto projectDir =  FileHelper::TakeProjectDir();
    FileSystemModelHelper::setRootPath(projectDir);

     MainWindow w;


//   auto cmd = QStringLiteral(R"(git -C "%1" mergetool)").arg("/home/zoli/fejlesztes_notes/notes1/3dplots");
//   ProcessHelper::Execute(cmd);

    w.updateFileTreeView();

    Log::init(MainWindow::msg, false, &w, false);
    w.show();    

    auto r = QApplication::exec();
    com::helper::SettingsHelper::saveSettings();

    delete fm;
    return r;
}



