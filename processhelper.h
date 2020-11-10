#ifndef PROCESSHELPER1_H
#define PROCESSHELPER1_H

#include <QObject>
#include <QString>

class ProcessHelper
{
public:
    struct Output
    {
    public:
        QString stdOut;
        QString stdErr;
        int exitCode;
        QString ToString();
    };
public:
    static const QString SEPARATOR;

    //static QString Execute(const QString& cmd);
    static QString Execute(const QStringList &cmds);
    static Output Execute(const QString &cmd);
};

#endif // PROCESSHELPER_H
