#ifndef FILENAMEHELPER_H
#define FILENAMEHELPER_H
#include <QString>
#include <QDir>

class FilenameHelper
{
public:
    FilenameHelper();
    static QString normalaizedir(QString dir);
    static QString GetIni();
//    static QDir GetAbsoluteDir(const QString &oldfn);
    //    static QString ChangeFile(const QString &oldfn, const QString &fn);
    static QString applicationName();
};

#endif // FILENAMEHELPER_H
