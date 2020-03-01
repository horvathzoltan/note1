#ifndef FILENAMEHELPER_H
#define FILENAMEHELPER_H
#include <QString>
#include <QDir>

#ifdef Q_OS_LINUX
 #define NEWLINE "\n"
#elif defined(Q_OS_WIN)
 #define NEWLINE "\r\n"
#endif

class FilenameHelper
{
public:
    static QString normalaizedir(QString dir);
    static QString GetIni();
    static QString GetProjectAbsolutePath();
    //    static QString ChangeFile(const QString &oldfn, const QString &fn);
    static QString applicationName();
    static QString GetFirstRow(const QString &a);
    static QStringList toStringList(const QString &s);
};

#endif // FILENAMEHELPER_H
