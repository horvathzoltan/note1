#include "filenamehelper.h"

#include <QFileInfo>
#include <QApplication>

FilenameHelper::FilenameHelper()
{

}

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

//QDir FilenameHelper::GetAbsoluteDir(const QString &oldfn){
//    return QFileInfo(oldfn).absoluteDir();
//}

//QString FilenameHelper::ChangeFile(const QString &oldfn, const QString &fn){
//    return GetAbsoluteDir(oldfn).filePath(fn);
//}


