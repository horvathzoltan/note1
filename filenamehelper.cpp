#include "filenamehelper.h"
#include "processhelper.h"
#include "settings.h"

#include <QFileInfo>
#include <QApplication>
#include <QSysInfo>

extern Settings settings;

QString FilenameHelper::normalaizedir(QString dir)
{
    auto a = dir.replace('\\', '/');
    return a;
}

QString FilenameHelper::applicationName(){
    return QDir(QApplication::applicationDirPath()).filePath(QApplication::applicationName());
}

QString FilenameHelper::GetIni()
{
    return applicationName()+".ini";
}

QString FilenameHelper::GetProjectAbsolutePath()
{
    auto r = QDir::home().filePath(settings.projectPath);
    return r;
}

QString FilenameHelper::user()
{
    auto l = QString(qgetenv("USER"));
    if (!l.isEmpty()) return l;
    l = qgetenv("USERNAME");
    if (!l.isEmpty()) return l;
#ifdef Q_OS_UNIX
    auto cmd = "whoami";
    auto r = ProcessHelper::Execute(cmd);
    if(!r.stdErr.isEmpty()) return QString();
    l = r.stdOut.trimmed();
#endif
    return l;
}

QString FilenameHelper::host(){ return QSysInfo::machineHostName(); }

QString FilenameHelper::user_at_host(){
    auto u = FilenameHelper::user();
    auto h = FilenameHelper::host();
    return (u.isEmpty())?h:u+'@'+h;
}

//QString FilenameHelper::ChangeFile(const QString &oldfn, const QString &fn){
//    return GetAbsoluteDir(oldfn).filePath(fn);
//}
//   2
// abcdef
