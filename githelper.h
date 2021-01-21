#ifndef GITHELPER_H
#define GITHELPER_H

#include <QFileInfo>
#include <QModelIndex>
#include <QString>



class GitHelper
{
public:
    enum Type:bool{Push=false,Pull=true};
    static QString GetRepoURL(const QFileInfo&);
    static bool isGitRepo(const QFileInfo& fileInfo);
    static bool isTracked(const QFileInfo& fileInfo);
    static bool clone(const QString &path, const QString &url, const QString &user, const QString &passwd);
    static QString GetToplevel(const QFileInfo &fileInfo);
    static bool Commit(const QString &fp, const QString &fn, const QString& desc, QString *s);
    static bool Add(const QString &fp, const QString &fn);
    static bool Rm(const QString &fp, const QString &fn);
    //static bool Push(const QString &fp,  QString *s);
    //static bool Fetch(const QString &fp, QString *s);
    static bool Refresh(const QString &repo_path, const QString& comment, Type t);
    static bool Rename(const QString &fp, const QString &fn1, const QString &fn2);
};

#endif // GITHELPER_H
