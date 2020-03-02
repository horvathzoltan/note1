#ifndef STRINGHELPER2_H
#define STRINGHELPER2_H

#include <QStringList>


#ifdef Q_OS_LINUX
 #define NEWLINE "\n"
#elif defined(Q_OS_WIN)
 #define NEWLINE "\r\n"
#endif

class StringHelper
{
public:
    static QString GetFirstRow(const QString &a);
    static QStringList toStringList(const QString &s);
    static QStringList toStringList(const QString &s, const QRegularExpression &r);
    static QString join(const QList<QChar> &chars, const QChar &s);
    static QStringList toStrigListNl(const QString &s);
};


#endif // STRINGHELPER2_H
