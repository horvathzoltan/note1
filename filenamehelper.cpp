#include "filenamehelper.h"

#include <QFileInfo>


FilenameHelper::FilenameHelper()
{

}

QString FilenameHelper::normalaizedir(QString dir)
{
    auto a = dir.replace('\\', '/');
    return a;
}

//QDir FilenameHelper::GetAbsoluteDir(const QString &oldfn){
//    return QFileInfo(oldfn).absoluteDir();
//}

//QString FilenameHelper::ChangeFile(const QString &oldfn, const QString &fn){
//    return GetAbsoluteDir(oldfn).filePath(fn);
//}


