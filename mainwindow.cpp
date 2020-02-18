#include "mainwindow.h"
#include "settings.h"
#include "ui_mainwindow.h"
#include "common/helper/file/filehelper.h"
#include <QFileSystemModel>
#include "common/logger/log.h"
#include <QCryptographicHash>

extern Settings settings;

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
}


MainWindow::~MainWindow()
{
    delete model;
    delete ui;
}

void MainWindow::Rename(const QModelIndex &index, const QString& fn){
    if(!index.isValid()) return;
    if(fn.isEmpty()) return;
    if(model->fileName(index)==fn) return;   
    auto oldfn = model->filePath(index);
    //auto path = QDir().absoluteFilePath(oldfn);
    QFileInfo fi(oldfn);
    auto dir = fi.absoluteDir();
    auto newfile = dir.filePath(fn);
    if(QFile::exists(newfile)) return;
    QFile f(oldfn);   
    //auto isok =
    f.rename(newfile);
    //zInfo(QStringLiteral("rename: %1->%2 %3").arg(oldfn).arg(fn).arg(isok));
}

void MainWindow::Save(const QModelIndex &index){
    if(!index.isValid()) return;
    auto txt = ui->plainTextEdit->toPlainText();
    auto hash = GetHash(txt);
    if(model_hash==hash) return;
    auto filepath = model->filePath(index);
    com::helper::FileHelper::save(txt, filepath);
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
    model_hash = GetHash(txt);//QString(QCryptographicHash::hash(txt.toUtf8(),QCryptographicHash::Md5).toHex());
    //statusBar()->showMessage("Loaded", 2000);
    zInfo("Loaded");
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
    Load(index);
}

const QModelIndex MainWindow::getIndex(){
    auto indexes = ui->fileTreeView->selectionModel()->selectedIndexes();
    if (indexes.isEmpty()) return QModelIndex();
    return indexes.at(0);
}



void MainWindow::closeEvent(QCloseEvent *event) {
    auto index = getIndex();
    if(!index.isValid()) return;
    Save(model_index);
    Rename(model_index, ui->filenameEdit->text());
}



void MainWindow::on_addDirButton_clicked()
{
    auto ix = getIndex();
    model->mkdir(ix, "makesz");

}

void MainWindow::msg(Errlevels::Levels errlevel, const QString &msg, const QString &loci, const QString &st, void *w)
{
    reinterpret_cast<MainWindow*>(w)->statusBar()->showMessage(msg, 2000);
}
