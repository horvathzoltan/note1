#include "githelper.h"
#include "processhelper.h"
#include "common/helper/string/stringhelper.h"
#include "filesystemmodelhelper.h"
#include "common/logger/log.h"


/*
 * //git ls-tree --full-tree --name-only -r HEAD
//git ls-files --error-unmatch common.pri
// git  rev-parse --show-toplevel
//git -C /home/zoli/common/test2/ rev-parse --show-toplevel
*/
QString GitHelper::GetRepoURL(const QFileInfo& fileInfo)
{
   auto filepath = fileInfo.absoluteFilePath();
   auto fileparent = fileInfo.absolutePath();

    auto out = ProcessHelper::Execute(QStringLiteral(R"(git -C "%1" rev-parse --show-toplevel)").arg(fileparent));
    if(out.exitCode!=0) return QString();
    if(out.stdOut.isEmpty()) return QString();
    QString rootpath = com::helper::StringHelper::GetFirstRow(out.stdOut);
    if(rootpath.isEmpty()) return QString();

    QString file;

    //if(!FileSystemModelHelper::isDir(index))
    if(!fileInfo.isDir())
    {
        out = ProcessHelper::Execute(QStringLiteral(R"(git -C "%1" ls-files --error-unmatch "%2")").arg(rootpath).arg(filepath));
        if(out.exitCode!=0) return rootpath;
        if(out.stdOut.isEmpty()) return rootpath;
        file = com::helper::StringHelper::GetFirstRow(out.stdOut);
    }

    out = ProcessHelper::Execute(QStringLiteral(R"(git -C "%1" remote -v)").arg(rootpath));
    if(out.exitCode==0){
        auto bl= com::helper::StringHelper::toStringList(out.stdOut);
        static QRegularExpression r1(com::helper::StringHelper::join({'\t', ' '}, '|'));

        QString fetch_url, push_url;
        foreach (auto b1, bl) {
            auto b2 = com::helper::StringHelper::toStringList(b1, r1);
            //auto b2 = b1.split(' ');
            if(b2.length()<3) continue;
            if(b2[2]=="(fetch)") fetch_url = b2[1];
            if(b2[2]=="(push)") push_url = b2[1];
        }
        if(!fetch_url.isEmpty())
        {
            if(file.isEmpty())
                return fetch_url;
            return fetch_url+'|'+file;
        }
    }
    if(file.isEmpty())
        return rootpath;
    return rootpath+'|'+file;
}

bool GitHelper::clone(const QString& path, const QString& url, const QString& user, const QString& passwd){
    zTrace()
    return false;
}
