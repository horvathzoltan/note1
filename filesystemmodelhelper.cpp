#include "common/helper/file/filehelper.h"
#include "common/logger/log.h"
#include "filenamehelper.h"
#include "filesystemmodelhelper.h"
#include <QCryptographicHash>

bool FileSystemModelHelper::isInited = false;
QFileSystemModel* FileSystemModelHelper::_model = nullptr;
QModelIndex FileSystemModelHelper::_model_index = QModelIndex();
QString FileSystemModelHelper::_model_hash = QString();

void FileSystemModelHelper::init(QFileSystemModel* m){
    _model = m;
    isInited = true;
    //_model_index = QModelIndex();
    //_model_hash = QString();
}


QModelIndex FileSystemModelHelper::setRootPath(const QString& fn){
    _model_index = _model->setRootPath(fn);
    return _model_index;
}

QModelIndex FileSystemModelHelper::Index(const QString& fn){
    return _model->index(fn);
}

QModelIndex FileSystemModelHelper::Index(){
    return _model_index;
}

//void FileSystemModelHelper::uninit(){
//    delete _model;
//}

QFileSystemModel* FileSystemModelHelper::Model(){
    return _model;
}

bool FileSystemModelHelper::Rename(const QModelIndex &index, const QString& fn){
    if(!index.isValid()) return false;
    if(fn.isEmpty()) return false;
    if(_model->fileName(index)==fn) return false;
    auto newfile = _model->fileInfo(index).absoluteDir().filePath(fn);
    if(QFile::exists(newfile)) return false;
    QFile f(_model->filePath(index));
    return f.rename(newfile);
}

// ha a txt változatlan, le kell menteni az aktuálisat
// ha a neve megváltozott, át kell nevezni a modellben
void FileSystemModelHelper::Save(const QString& fn, const QString& txt){
    if(!_model_index.isValid()) return;
    if(fn.isEmpty()) return;

    QString e;
    auto filepath = _model->filePath(_model_index);

    if(Save(_model_index, txt))
        e = QStringLiteral("Saved: %1").arg(filepath);
    else
        e = QStringLiteral("Unchanged: %1").arg(filepath);
    if(Rename(_model_index, fn))
        e += QStringLiteral(" As: %1").arg(fn);

    zInfo(e);
}

QString FileSystemModelHelper::GetHash(const QString &txt){
    return QString(QCryptographicHash::hash(txt.toUtf8(),QCryptographicHash::Md5).toHex());
}

bool FileSystemModelHelper::Save(const QModelIndex &index, const QString& txt ){
    //if(!index.isValid()) return false;
    auto hash = GetHash(txt);
    if(_model_hash==hash) return false;
    auto filepath = _model->filePath(index);
    com::helper::FileHelper::save(txt, filepath);
    return true;
}

int FileSystemModelHelper::columnCount()
{
    return _model->columnCount();
}

bool FileSystemModelHelper::Equals(const QModelIndex &m)
{
    return _model_index == m;
}

bool FileSystemModelHelper::isDir(const QModelIndex &m)
{
    return _model->isDir(m);
}

bool FileSystemModelHelper::isValid()
{
    return _model_index.isValid();
}

QModelIndex FileSystemModelHelper::Mkdir(const QModelIndex &ix, const QString &fn){
    return _model->mkdir(ix, fn);
}

bool FileSystemModelHelper::Rmdir(const QModelIndex &ix){
    return _model->rmdir(ix);
}

bool FileSystemModelHelper::Remove(const QModelIndex &ix){
    return _model->remove(ix);
}


QString FileSystemModelHelper::filePath(const QModelIndex &ix){
    return _model->filePath(ix);
}


QString FileSystemModelHelper::fileName(const QModelIndex &ix){
    return _model->fileName(ix);
}

QModelIndex FileSystemModelHelper::parent(const QModelIndex &ix){
    return _model->parent(ix);
}

QFileInfo FileSystemModelHelper::fileInfo(const QModelIndex &ix){
    return _model->fileInfo(ix);
}


QString FileSystemModelHelper::Load(const QModelIndex &index)
{
    if(_model->isDir(index)) return"";
 //   Save(_model_index);
  //  Rename(_model_index, fn);// ui->filenameEdit->text()

    auto filepath = _model->filePath(index);
    //auto filename = _model->fileName(index);
    auto txt = com::helper::FileHelper::load(filepath);
    //ui->filenameEdit->setText(filename);
    //ui->plainTextEdit->setPlainText(txt);
    _model_index = index;
    _model_hash = GetHash(txt);
    zInfo(QStringLiteral("Loaded: %1").arg(filepath));
   // UpdateEditorState();
   // UpdateActionButtonState(index);
    return txt;
}
