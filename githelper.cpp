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
    //auto fn = fileInfo.fileName();

    auto cmd = QStringLiteral(R"(git -C "%1" rev-parse --show-toplevel)").arg(fileparent);
    //auto cmd = QStringLiteral(R"(git -C "%1" ls-files --error-unmatch "%2")").arg(fileparent).arg(fn);

    auto out = ProcessHelper::Execute(cmd);
    if(out.exitCode!=0) return QString();
    if(out.stdOut.isEmpty()) return QString();
    return com::helper::StringHelper::GetFirstRow(out.stdOut);
}

bool GitHelper::isGitRepo(const QFileInfo& fileInfo){
    return !GitHelper::GetToplevel(fileInfo).isEmpty();
    //return GitHelper::GetRepoURL(fileInfo).startsWith('g');
    //return GitHelper::GetRepoURL(fileInfo);
}

auto GitHelper::isTracked(const QFileInfo& fileInfo) -> bool{
    auto fn = fileInfo.fileName();
    auto fileparent = (fileInfo.isDir())?fileInfo.absoluteFilePath():fileInfo.absolutePath();

    auto cmd = QStringLiteral(R"(git -C "%1" ls-files --error-unmatch "%2")").arg(fileparent).arg(fn);
    auto out = ProcessHelper::Execute(cmd);
    return !out.exitCode;
}

// git add work1.h
bool GitHelper::Add(const QString &fp, const QString &fn)
{
    //QString comment = "add_"+environment.user_at_host+'_'+QDateTime::currentDateTimeUtc().toString();
    auto cmd = QStringLiteral(R"(git -C "%1" add "%2")").arg(fp).arg(fn);
    auto out = ProcessHelper::Execute(cmd);
    if(!out.exitCode) return true;
    zError2(out.ToString(),2);
    //if(!out.stdErr.isEmpty()) return false;
    return false;
}

bool GitHelper::Rm(const QString &fp, const QString &fn)
{
    //QString comment = "add_"+environment.user_at_host+'_'+QDateTime::currentDateTimeUtc().toString();
    auto cmd = QStringLiteral(R"(git -C "%1" rm "%2")").arg(fp).arg(fn);
    auto out = ProcessHelper::Execute(cmd);
    if(!out.exitCode) return true;
    zError2(out.ToString(),2);
    //if(!out.stdErr.isEmpty()) return false;
    return false;
}

bool GitHelper::Rename(const QString &fp, const QString &fn1, const QString &fn2)
{
    //QString comment = "add_"+environment.user_at_host+'_'+QDateTime::currentDateTimeUtc().toString();
    auto cmd = QStringLiteral(R"(git -C "%1" mv "%2" "%3")").arg(fp).arg(fn1).arg(fn2);
    auto out = ProcessHelper::Execute(cmd);
    if(!out.exitCode) return true;
    zError2(out.ToString(),2);
    //if(!out.stdErr.isEmpty()) return false;
    return false;
}

// git commit work1.h -m "valami2"
bool GitHelper::Commit(const QString &repo_path, const QString &file_name, const QString& desc, QString *err)
{    
    QString comment = desc+"_"+environment.user_at_host+'_'+QDateTime::currentDateTimeUtc().toString();
    auto cmd = QStringLiteral(R"(git -C "%1" commit -a -m "%2")").arg(repo_path).arg(comment);
    if(!file_name.isEmpty()) cmd+= QStringLiteral(R"( -o "%1")").arg(file_name);
    auto out = ProcessHelper::Execute(cmd);
    if(!out.exitCode) return true;
    if(out.stdOut.contains("Your branch is up to date")) return true;//1:Your branch is up to date

    //if(out.stdErr.isEmpty()) return true;
    //if(err) *err = out.ToString();
    zError2(out.ToString(),2);
    return false;
}

/*
git commit -a -m "b" // ha push akkor kell
git fetch
git merge
git mergetool // ha nincs conflict nem kell
git commit -a -m "ab"
git push // ha push akkor kell
*/

//Your branch is up to date

auto GitHelper::Refresh(const QString &repo_path, const QString& comment, Type type) ->bool
{
    bool isok;
    if(type == Push){ // push akko rkell commitolni - pullnál nincs változás
        QString err;
        isok = Commit(repo_path, QString(), comment, &err);

        if(!isok){            
            zError2(err,2);
            return false;
        }
    }

    auto cmd = QStringLiteral(R"(git -C "%1" fetch)").arg(repo_path);
    auto out = ProcessHelper::Execute(cmd);
    if(out.exitCode){
        zError2(out.ToString(),2);
        return false;
    }

    cmd = QStringLiteral(R"(git -C "%1" merge)").arg(repo_path);
    out = ProcessHelper::Execute(cmd);

   // bool isChanged = false;

    if(out.exitCode) //1:Conflict 1:Aborting
    {
        if(out.stdOut.contains(QStringLiteral("CONFLICT")))
        {
            cmd = QStringLiteral(R"(git -C "%1" mergetool)").arg(repo_path);
            out = ProcessHelper::Execute(cmd);
            if(out.exitCode)
            {
                zError2(out.ToString(),2);
                return false;
            }
            //else // változott, sikeres merge, de az eredeti megszakadt
            //{
                cmd = QStringLiteral(R"(git -C "%1" -c core.editor=true merge --continue)").arg(repo_path);
                out = ProcessHelper::Execute(cmd);
                if(out.exitCode)
                {
                    zError2(out.ToString(),2);
                    return false;
                }
            //}
        }
        else if(out.stdErr.endsWith("not something we can merge\n")) {}
        else// egyéb hiba van
        {

            zError2(out.ToString(),2);
            return false;
        }
    }
//    else // 0:Already up to date. //0:1 file changed
//    {
//        if(!out.stdOut.contains("Already up to date.")) // valami változás volt
//        {
//            isChanged = true;
//        }

//    }

//    if(isChanged) // ha változott
//    {
//        cmd = QStringLiteral(R"(git -C "%1" -c core.editor=true merge --continue)").arg(repo_path);
//        out = ProcessHelper::Execute(cmd);
////        QString err;
////        isok = Commit(repo_path, QString(), "merge-"+comment, &err);
////        if(!isok){
////            zError2(err,2);
////            return false;
////        }
//    }

    if(type == Push){ // ha push fel is küldjük
        auto cmd = QStringLiteral( R"(git -C "%1" push)").arg(repo_path);
        auto out = ProcessHelper::Execute(cmd);
        if(out.exitCode){
            zError2(out.ToString(),2);
            return false;
        }
    }

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
    //if(!isGitRepo(fileInfo)) return "";
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
    {
        auto cmd = QStringLiteral(R"(git -C "%1" ls-files --error-unmatch "%2")").arg(rootpath).arg(filepath);
        out = ProcessHelper::Execute(cmd);
        if(out.exitCode!=0) return filepath;
        if(out.stdOut.isEmpty()) return filepath;
        file = filepath.mid(rootpath.length()+1);
    }
    else
    {
        auto cmd = QStringLiteral(R"(git -C "%1" ls-files --error-unmatch "%2")").arg(rootpath).arg(filepath);
        out = ProcessHelper::Execute(cmd);
        if(out.exitCode!=0) return filepath;
        if(out.stdOut.isEmpty()) return filepath;
        file = filepath.mid(rootpath.length()+1);
    }

    auto cmd= QStringLiteral(R"(git -C "%1" remote -v)").arg(rootpath);
    out = ProcessHelper::Execute(cmd);
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


