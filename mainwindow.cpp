#include "mainwindow.h"
#include "settings.h"
#include "ui_mainwindow.h"

#include <QFileSystemModel>

extern Settings settings;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QFileSystemModel *model = new QFileSystemModel;
    // auto projectDir= QDir(settings.projectPath);
    //auto homeDir = QDir::homePath();
    auto projectDir = QDir(QDir::homePath()).filePath(settings.projectPath);
    if(!QDir(projectDir).exists()){
        QDir::mkpath(projectDir);
    }
    model->setRootPath(projectDir);
    ui->fileTreeView->setModel(model);
}

MainWindow::~MainWindow()
{
    delete ui;
}

