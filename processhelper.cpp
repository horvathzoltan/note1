#include "processhelper.h"

#include "common/helper/string/stringhelper.h"
#include <QCoreApplication>
#include <QProcess>

const QString ProcessHelper::SEPARATOR = NEWLINE+QStringLiteral("stderr")+NEWLINE;

QString ProcessHelper::Output::ToString(){
    QString e;

    if(!stdOut.isEmpty())
    {
        if(!e.isEmpty()) e+=SEPARATOR;
        e+=stdOut;
    }
    if(!stdErr.isEmpty())
    {
        if(!e.isEmpty()) e+=SEPARATOR;
        e+=stdErr;
    }
    if(!e.isEmpty()) e+=SEPARATOR;
    e+=QStringLiteral("exitCode: %1").arg(exitCode);
    return e;
}

ProcessHelper::Output ProcessHelper::Execute(const QString& cmd){
    qint64 pid;
    QProcess process;
    static QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("LD_LIBRARY_PATH", "/usr/lib"); // workaround - https://bugreports.qt.io/browse/QTBUG-2284
    process.setProcessEnvironment(env);
    static auto path = QCoreApplication::applicationDirPath();
    process.setWorkingDirectory(path);

    process.start(cmd);
    process.waitForFinished(-1); // will wait forever until finished
    return {process.readAllStandardOutput(), process.readAllStandardError(), process.exitCode()};
}

//QString ProcessHelper::Execute(const QString& cmd){
//    return Execute2(cmd).ToString();
//}

QString ProcessHelper::Execute(const QStringList& cmds){
    Output e;

    foreach(auto cmd, cmds){
        auto r = Execute(cmd);
        e.stdOut += r.stdOut;
        e.stdErr += r.stdErr;
        e.exitCode = r.exitCode;
    }
    return e.ToString();
}
