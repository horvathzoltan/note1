#ifndef GITHELPER_H
#define GITHELPER_H

#include <QFileInfo>
#include <QModelIndex>
#include <QString>



class GitHelper
{
public:
    static QString GetRepoURL(const QFileInfo&);
    static bool isGitRepo(const QFileInfo& fileInfo);
    static bool isTracked(const QFileInfo& fileInfo);
    static bool clone(const QString &path, const QString &url, const QString &user, const QString &passwd);
    static QString GetToplevel(const QFileInfo &fileInfo);
    static bool Commit(const QString &fp, const QString &fn, const QString& desc);
    static bool Add(const QString &fp, const QString &fn);
    static bool Rm(const QString &fp, const QString &fn);
    static bool Push(const QString &fp, QObject *parent);
};

#endif // GITHELPER_H
