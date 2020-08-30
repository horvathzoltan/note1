#ifndef FSMILEHELPER_H
#define FSMILEHELPER_H

#include <QFileSystemModel>
#include <QModelIndex>

class FileSystemModelHelper
{
private:
    static QFileSystemModel* _model;
    static QModelIndex _model_index; // ami meg van nyitva
    static QString _model_hash;
    static bool isInited;
public:
    static void init(QFileSystemModel* m);
    //static void uninit();
    static QString Load(const QModelIndex &index);
    static bool Rename(const QModelIndex &index, const QString &fn);
    static bool Save(const QModelIndex &index,  const QString& txt);
    static void Save(const QString& fn,  const QString& txt);
    static int columnCount();
    static bool Equals(const QModelIndex&);
    static bool isDir(const QModelIndex &m);
    static bool isValid();
    static QModelIndex Mkdir(const QModelIndex &ix, const QString &fn);
    static QString filePath(const QModelIndex &ix);
    static QString fileName(const QModelIndex &ix);
    static QFileInfo fileInfo(const QModelIndex &ix);
    static QModelIndex parent(const QModelIndex &ix);
    static bool Rmdir(const QModelIndex &ix);
    static bool Remove(const QModelIndex &ix);
    static QModelIndex setRootPath(const QString& fn);
    static QModelIndex Index(const QString& fn);
    static QModelIndex Index();
    static QFileSystemModel* Model();
    static QString GetHash(const QString &txt);

};

#endif // FILEHELPER_H
