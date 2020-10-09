#include "githelper.h"
#include "processhelper.h"
#include "common/helper/string/stringhelper.h"
#include "filesystemmodelhelper.h"
#include "common/logger/log.h"
#include <QDateTime>
#include "filenamehelper.h"
#include "settings.h"

extern Environment environment;

QString GitHelper::GetToplevel(const QFileInfo& fileInfo)
{

    auto fileparent = (fileInfo.isDir())?fileInfo.absoluteFilePath():fileInfo.absolutePath();

    auto out = ProcessHelper::Execute(QStringLiteral(R"(git -C "%1" rev-parse --show-toplevel)").arg(fileparent));
    if(out.exitCode!=0) return QString();
    if(out.stdOut.isEmpty()) return QString();
    return com::helper::StringHelper::GetFirstRow(out.stdOut);
}

bool GitHelper::isGitRepo(const QFileInfo& fileInfo){
    return !GitHelper::GetToplevel(fileInfo).isEmpty();
}

// git commit work1.h -m "valami2"
bool GitHelper::Commit(const QString &fp, const QString &fn)
{    
    QString comment = "update_"+environment.user_at_host+'_'+QDateTime::currentDateTimeUtc().toString();
    auto cmd = QStringLiteral(R"(git -C "%1" commit -m "%2" -o "%3")").arg(fp).arg(comment).arg(fn);
    auto out = ProcessHelper::Execute(cmd);
    if(out.exitCode!=0) return false;
    if(out.stdOut.isEmpty()) return false;
    if(!out.stdErr.isEmpty()) return false;
    return true;
}

// git push
bool GitHelper::Push(const QString &fp)
{
    auto cmd = QStringLiteral(R"(git -C "%1" push)").arg(fp);
    auto out = ProcessHelper::Execute(cmd);
    if(out.exitCode!=0) return false;
    if(out.stdOut.isEmpty()) return false;
    if(!out.stdErr.isEmpty()) return false;
    return true;
}
/*
 * //git ls-tree --full-tree --name-only -r HEAD
//git ls-files --error-unmatch common.pri
// git  rev-parse --show-toplevel
//git -C /home/zoli/common/test2/ rev-parse --show-toplevel
*/
///
/// \brief A repo URL-jét adja a a fileinfo alapján
/// \param fileInfo
/// \return
///
QString GitHelper::GetRepoURL(const QFileInfo& fileInfo)
{
   auto filepath = fileInfo.absoluteFilePath();
   //auto fileparent = fileInfo.absolutePath();

    ProcessHelper::Output out;
    //auto out = ProcessHelper::Execute(QStringLiteral(R"(git -C "%1" rev-parse --show-toplevel)").arg(fileparent));
    //if(out.exitCode!=0) return QString();
    //if(out.stdOut.isEmpty()) return QString();
    QString rootpath = GetToplevel(fileInfo);//com::helper::StringHelper::GetFirstRow(out.stdOut);
    if(rootpath.isEmpty()) return QString();    

    QString file;

    //if(!FileSystemModelHelper::isDir(index))
    if(!fileInfo.isDir())
    {         out = ProcessHelper::Execute(QStringLiteral(R"(git -C "%1" ls-files --error-unmatch "%2")").arg(rootpath).arg(filepath));
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

/*
git@github.com:horvathzoltan/3dplots.git - 3dplots könyvtár van-e
git -C /home/zoli/cmn2/a clone git@github.com:horvathzoltan/3dplots.git
*/
bool GitHelper::clone(const QString& path, const QString& url, const QString& user, const QString& passwd){
    //zTrace()    
    static auto commandTmp = QStringLiteral(R"(git -C %1 clone %2)");
    auto command = commandTmp.arg(path).arg(url);
    //zInfo(command);
    auto out = ProcessHelper::Execute(command);
    return false;

}


