#include "filenamehelper.h"
#include "mainwindow.h"
#include "mainwindow.h"
#include "newfiledialog.h"
#include "settings.h"
#include "ui_mainwindow.h"
#include "common/helper/file/filehelper.h"
#include <QFileSystemModel>
#include "common/logger/log.h"
#include <QCryptographicHash>

extern Settings settings;

const QString MainWindow::MSG_FAILEDTO = QStringLiteral("Failed to %1 %2: %3");
const QString MainWindow::MSG_ADDNEWDIALOG = QStringLiteral("Add new %1 name:");
const QString MainWindow::MSG_ADDNEW =  QStringLiteral("New %1: %2");
const QString MainWindow::FILE =  QStringLiteral("file");
const QString MainWindow::DIR =  QStringLiteral("dir");
const QString MainWindow::DELETE =  QStringLiteral("delete");
const QString MainWindow::CREATE =  QStringLiteral("create");
const int MainWindow::AUTOSAVE_SEC = 120;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    model = new QFileSystemModel;
    model_index = QModelIndex();
    model_hash = QString();
    // auto projectDir= QDir(settings.projectPath);
    //auto homeDir = QDir::homePath();

    auto projectDir = QDir(QDir::homePath()).filePath(settings.projectPath);
    if(!QDir(projectDir).exists()){
        QDir(QDir::homePath()).mkpath(projectDir);
    }

    model->setRootPath(projectDir);
    QModelIndex idx = model->index(projectDir);
    ui->fileTreeView->setModel(model);
    ui->fileTreeView->setRootIndex(idx);

    for (int i = 1; i < model->columnCount(); ++i)
        ui->fileTreeView->hideColumn(i);

    UpdateEditorState();

    //QTimer *timer = new QTimer(this);
    connect(&autosave_timer, &QTimer::timeout, this, &MainWindow::on_timerupdate);

}

void MainWindow::on_timerupdate(){
    Save();
}

MainWindow::~MainWindow()
{
    delete model;
    delete ui;
}

void MainWindow::msg(Errlevels::Levels errlevel, const QString &msg, const QString &loci, const QString &st, void *w)
{
    reinterpret_cast<MainWindow*>(w)->statusBar()->showMessage(msg, 2000);
}



void MainWindow::Rename(const QModelIndex &index, const QString& fn){
    if(!index.isValid()) return;
    if(fn.isEmpty()) return;
    if(model->fileName(index)==fn) return;   
    auto newfile = model->fileInfo(index).absoluteDir().filePath(fn);
    if(QFile::exists(newfile)) return;
    QFile f(model->filePath(index));
    f.rename(newfile);
    //zInfo(QStringLiteral("rename: %1->%2 %3").arg(oldfn).arg(fn).arg(isok));
}

void MainWindow::Save(){
    Save(model_index);
    Rename(model_index, ui->filenameEdit->text());
}

void MainWindow::Save(const QModelIndex &index){
    if(!index.isValid()) return;
    auto txt = ui->plainTextEdit->toPlainText();
    auto hash = GetHash(txt);
    if(model_hash==hash) return;
    auto filepath = model->filePath(index);
    com::helper::FileHelper::save(txt, filepath);
    zInfo(QStringLiteral("Saved: %1").arg(filepath));
}

void MainWindow::Load(const QModelIndex &index)
{
    Save(model_index);
    Rename(model_index, ui->filenameEdit->text());

    if(model->isDir(index)) return;
    auto filepath = model->filePath(index);
    auto filename = model->fileName(index);
    auto txt = com::helper::FileHelper::load(filepath);
    ui->filenameEdit->setText(filename);
    ui->plainTextEdit->setPlainText(txt);
    model_index = index;
    model_hash = GetHash(txt);
    zInfo(QStringLiteral("Loaded: %1").arg(filename));
    UpdateEditorState();
    UpdateActionButtonState(index);
} 

QString MainWindow::GetHash(const QString &txt){
    return QString(QCryptographicHash::hash(txt.toUtf8(),QCryptographicHash::Md5).toHex());
}

void MainWindow::on_fileTreeView_doubleClicked(const QModelIndex &index)
{
    Load(index);
}

void MainWindow::on_EditButton_clicked()
{
    auto index = getIndex();
    if(!index.isValid()) return;
    if(model_index==index) return;
    Load(index);
}

const QModelIndex MainWindow::getIndex(){
    auto indexes = ui->fileTreeView->selectionModel()->selectedIndexes();
    if (indexes.isEmpty()) return QModelIndex();
    return indexes.at(0);
}



void MainWindow::closeEvent(QCloseEvent *event) {   
    Save();
}

QString  MainWindow::NewDirDialog(const QString& title){
    if(title.isEmpty()) return QString();
    NewFileDialog dialog(this);
    dialog.setTitle(title);
    dialog.exec();
    if(dialog.result()!=QDialog::Accepted) return QString();
    return dialog.filename();
}

void MainWindow::on_addDirButton_clicked()
{
    auto ix = getIndex();
    //if(!model->isDir(ix)) return;
    auto fn = NewDirDialog(MSG_ADDNEWDIALOG.arg(DIR));
    if(fn.isEmpty()) return;
    QModelIndex newix;
    if(model->isDir(ix))
            newix= model->mkdir(ix, fn);
    else{
        auto px = model->parent(ix);
        newix= model->mkdir(px, fn);
    }
    if(newix.isValid())
        zInfo(MSG_ADDNEW.arg(DIR).arg(fn))
    else
        zInfo(MSG_FAILEDTO.arg(CREATE).arg(DIR).arg(fn));

}


void MainWindow::on_deleteButton_clicked()
{
    auto ix = getIndex();
    if(!model_index.isValid()) return;
    if(model_index==ix) return;
    auto fn = model->fileName(ix);
    if(model->isDir(ix))
    {
        if(!model->rmdir(ix))
            zInfo(MSG_FAILEDTO.arg(DELETE).arg(DIR).arg(fn))
    }
    else
    {
        if(!model->remove(ix))
            zInfo(MSG_FAILEDTO.arg(DELETE).arg(FILE).arg(fn))
    }
}


void MainWindow::on_addNoteButton_clicked()
{
    auto ix = getIndex();
    auto fn = NewDirDialog(MSG_ADDNEWDIALOG.arg(FILE));
    if(fn.isEmpty()) return;
    //auto oldfn = model->filePath(ix);
    QString newfile;
    if(model->isDir(ix))
    {
         newfile = QDir(model->filePath(ix)).filePath(fn);
    }
    else
    {
         newfile = model->fileInfo(ix).absoluteDir().filePath(fn);
    }

    QFile f(newfile);
    if(f.open(QIODevice::NewOnly| QIODevice::Text)){
        f.close();
        zInfo(MSG_ADDNEW.arg(FILE).arg(newfile));
    }
    else{
        zInfo(MSG_FAILEDTO.arg(CREATE).arg(FILE).arg(newfile));

    }
}

void MainWindow::UpdateEditorState(){
    if(model_index.isValid()){
        MainWindow::setEditorState(true);
    }else{
        MainWindow::setEditorState(false);
    }
}

void MainWindow::setEditorState(bool x){
    ui->filenameEdit->setEnabled(x);
    ui->plainTextEdit->setEnabled(x);
    if(x) autosave_timer.start(1000*AUTOSAVE_SEC);
    else autosave_timer.stop();
}


void MainWindow::UpdateActionButtonState(const QModelIndex &index){
    if(model_index.isValid()){
        if(index==model_index){
            setActionButtonState(false);
        }
        else
        {
            setActionButtonState(true);
        }
    }
    else
    {
        setActionButtonState(true);
    }
}

void MainWindow::setActionButtonState(bool x){
    ui->EditButton->setEnabled(x);
    ui->deleteButton->setEnabled(x);
}

void MainWindow::on_fileTreeView_clicked(const QModelIndex &index)
{
    UpdateActionButtonState(index);
}
