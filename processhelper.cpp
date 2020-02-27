#include "processhelper.h"
#include "filenamehelper.h"
#include <QProcess>


QString ProcessHelper::Execute(const QString& cmd){
    QProcess process;
    process.start(cmd);
    process.waitForFinished(-1); // will wait forever until finished

    QString stdout = process.readAllStandardOutput();
    QString stderr = process.readAllStandardError();
    if(stderr.isEmpty()) return stdout;
    return stdout+NEWLINE+"stderr"+NEWLINE+stderr;
}
