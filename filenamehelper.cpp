#include "filenamehelper.h"
#include "settings.h"

#include <QFileInfo>
#include <QApplication>

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

QString FilenameHelper::GetProjectAbsolutePath(){
    //return QDir(QDir::homePath()).filePath(settings.projectPath);
    auto r = QDir::home().filePath(settings.projectPath);
    return r;//QDir(QDir::homePath()).filePath(settings.projectPath);
}

//QString FilenameHelper::ChangeFile(const QString &oldfn, const QString &fn){
//    return GetAbsoluteDir(oldfn).filePath(fn);
//}
//   2
// abcdef
QString FilenameHelper::GetFirstRow(const QString & a){
    int ix = a.indexOf('\n');
    if(ix==-1) return a;
    if(ix>0&&a[ix-1]=='\r') ix--;
    return a.left(ix);
}

QStringList FilenameHelper::toStringList(const QString &s){
    return s.split(QRegExp(QStringLiteral("\\t|\\ ")), QString::SkipEmptyParts);
}

