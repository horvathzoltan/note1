#include "processhelper.h"
#include "filenamehelper.h"
#include <QProcess>

const QString ProcessHelper::SEPARATOR = NEWLINE+QStringLiteral("stderr")+NEWLINE;

QString ProcessHelper::Out::ToString(){
    if(stderr.isEmpty()) return stdout;
    return stdout+SEPARATOR+stderr;
}

ProcessHelper::Out ProcessHelper::Execute2(const QString& cmd){
    QProcess process;
    process.start(cmd);
    process.waitForFinished(-1); // will wait forever until finished
    return {process.readAllStandardOutput(), process.readAllStandardError()};
}

QString ProcessHelper::Execute(const QString& cmd){
    return Execute2(cmd).ToString();
}

QString ProcessHelper::Execute(const QStringList& cmds){
    Out e;

    foreach(auto cmd, cmds){
        auto r = Execute2(cmd);
        e.stdout += r.stdout;
        e.stderr += r.stderr;
    }
    return e.ToString();
}
