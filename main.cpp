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
    auto fn = FilenameHelper::GetIni();
    com::helper::SettingsHelper::init(fn, &settings);
    com::helper::SettingsHelper::loadSettings();

    QFileSystemModel* fm = new QFileSystemModel();

    FileSystemModelHelper::init(fm);

    auto projectDir =  FileHelper::TakeProjectDir();
    FileSystemModelHelper::setRootPath(projectDir);

     MainWindow w;

   auto cmd = QStringLiteral(R"(git -C "%1" mergetool)").arg("/home/zoli/fejlesztes_notes/notes1/3dplots");
   // auto out = ProcessHelper::Execute(cmd, nullptr);



    w.updateFileTreeView();

    Log::init(MainWindow::msg, false, &w, false);
    w.show();

    qint64 pid;
    QProcess process;
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("LD_LIBRARY_PATH", "/usr/lib"); // workaround - https://bugreports.qt.io/browse/QTBUG-2284
    process.setProcessEnvironment(env);
    process.setWorkingDirectory(QCoreApplication::applicationDirPath());
    process.start("kdiff3");

    process.waitForFinished(-1);
    //QProcess::execute("kdiff3");
//while (kill(pid, 0) == 0) {}
//    QProcess proc;
//    proc.start("kdiff3", QStringList() << "-c" << "ifconfig | grep inet");
//    proc.waitForStarted(-1);
    //proc.write(someData, dataSize);

    auto r = a.exec();
    com::helper::SettingsHelper::saveSettings();

    delete fm;
    return r;
}



