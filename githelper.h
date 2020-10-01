#ifndef GITHELPER_H
#define GITHELPER_H

#include <QFileInfo>
#include <QModelIndex>
#include <QString>



class GitHelper
{
public:
    static QString GetRepoURL(const QFileInfo&);
    static bool clone(const QString &path, const QString &url, const QString &user, const QString &passwd);
    static QString GetToplevel(const QFileInfo &fileInfo);
};

#endif // GITHELPER_H
