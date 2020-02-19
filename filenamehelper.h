#ifndef FILENAMEHELPER_H
#define FILENAMEHELPER_H
#include <QString>
#include <QDir>

class FilenameHelper
{
public:
    FilenameHelper();
    static QString normalaizedir(QString dir);
//    static QDir GetAbsoluteDir(const QString &oldfn);
//    static QString ChangeFile(const QString &oldfn, const QString &fn);
};

#endif // FILENAMEHELPER_H
