#ifndef PROCESSHELPER_H
#define PROCESSHELPER_H

#include <QString>



class ProcessHelper
{
    struct Out
    {
    public:
        QString stdout;
        QString stderr;

        QString ToString();
    };
public:
    static const QString SEPARATOR;

    static QString Execute(const QString& cmd);    
    static QString Execute(const QStringList &cmds);
    static Out Execute2(const QString &cmd);
};

#endif // PROCESSHELPER_H
