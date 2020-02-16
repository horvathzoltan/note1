#include "mainwindow.h"
#include "settings.h"
#include "ui_mainwindow.h"
#include "common/helper/file/filehelper.h"
#include <QFileSystemModel>

extern Settings settings;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    model = new QFileSystemModel;
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
    if(fn.isEmpty()) return;
    if(model->fileName(index)==fn) return;
    QFile(model->filePath(index)).rename(fn);
}

void MainWindow::Load(const QModelIndex &index)
{
    Rename(index, ui->filenameEdit->text());

    if(model->isDir(index)) return;
    auto filepath = model->filePath(index);
    auto filename = model->fileName(index);
    auto txt = com::helper::FileHelper::load(filepath);
    ui->filenameEdit->setText(filename);
    ui->plainTextEdit->setPlainText(txt);
}

void MainWindow::on_fileTreeView_doubleClicked(const QModelIndex &index)
{
    Load(index);
}

void MainWindow::on_EditButton_clicked()
{
    auto index = getIndex();
    if(index == nullptr) return;
    Load(*index);
}

QModelIndex* MainWindow::getIndex(){
    auto indexes = ui->fileTreeView->selectionModel()->selectedIndexes();
    if (indexes.isEmpty()) return nullptr;
    return &indexes[0];
}

void MainWindow::closeEvent(QCloseEvent *event) {
    auto index = getIndex();
    if(index==nullptr) return;

    Rename(*index, ui->filenameEdit->text());
}
