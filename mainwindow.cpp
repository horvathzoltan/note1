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
#include "settingsdialog.h"
#include "common/helper/settings/settingshelper.h"
#include "processhelper.h"
#include "common/helper/string/stringhelper.h"

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

//    auto projectDir = QDir(QDir::homePath()).filePath(settings.projectPath);
//    if(!QDir(projectDir).exists()){
//        QDir(QDir::homePath()).mkpath(projectDir);
//    }

//    model->setRootPath(projectDir);
    setRootPath(settings.projectPath);
    //QModelIndex idx = model->index(projectDir);
//    ui->fileTreeView->setModel(model);
//    ui->fileTreeView->setRootIndex(idx);

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
    if(model->isDir(index)) return;
    Save(model_index);
    Rename(model_index, ui->filenameEdit->text());

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

/*fileTreeView events*/
void MainWindow::on_fileTreeView_clicked(const QModelIndex &index)
{
    UpdateActionButtonState(index);
    auto a = GetRepoURL(index);
    UpdateGitActionButtonState(a, index);

    ui->repolabel->setText(a);    
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

void MainWindow::on_addDirButton_clicked()
{
    auto ix = getIndex();
    //if(!model->isDir(ix)) return;
    auto fn = DisplayNewDirDialog(MSG_ADDNEWDIALOG.arg(DIR));
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
    auto fn = DisplayNewDirDialog(MSG_ADDNEWDIALOG.arg(FILE));
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



QString  MainWindow::DisplayNewDirDialog(const QString& title){
    if(title.isEmpty()) return QString();
    NewFileDialog dialog(this);
    dialog.setTitle(title);
    dialog.exec();
    if(dialog.result()!=QDialog::Accepted) return QString();
    return dialog.filename();
}

void MainWindow::setRootPath(const QString& path){
    //if(settings.projectPath.startsWith(QDir::homePath()))
    //auto projectDir = QDir(QDir::homePath()).filePath(settings.projectPath);
    auto projectDir = FilenameHelper::GetProjectAbsolutePath();
    if(!QDir(projectDir).exists()){
        QDir(QDir::homePath()).mkpath(projectDir);
    }
    model->setRootPath(projectDir);
    QModelIndex idx = model->index(projectDir);
    ui->fileTreeView->setModel(model);
    ui->fileTreeView->setRootIndex(idx);
}

int  MainWindow::DisplaySettingsDialog(const QString& title){
    if(title.isEmpty()) return -1;
    SettingsDialog dialog(this);
    //dialog.setTitle(title);
    dialog.init(&settings);
    //dialog.SetData(settings);
    dialog.exec();
    return dialog.result();
}

void MainWindow::SettingsProcess(int r){
    if(r!=QDialog::Accepted) return;
    if(!settings.isValid()) return;
    auto projectpath = FilenameHelper::GetProjectAbsolutePath();

//    bool isOk = false;
//    if(QDir(projectpath).isEmpty()){
//        //git clone git@github.com:whatever .
//    }
//    else{
//        //git status
//        auto a = ProcessHelper::Execute(QStringLiteral(R"(git -C "%1" status)").arg(projectpath));
//        //if(a.exitCode!=0) return;
//        if(a.stdErr.startsWith(QStringLiteral("fatal: not a git repository"))){
//            //init, addlocal, commit
//            auto c = ProcessHelper::Execute(QStringList{});
//            /*
//             * git init
//git add .
//git commit -m "First commit"
//git remote add origin PATH/TO/REPO
////git remote -v
//git push origin master
//*/
//            zInfo("create git repo");
//            isOk = true;
//        }
//        else if(a.stdOut.startsWith(QStringLiteral("On branch"))){
//            zInfo("existing repo");
//            auto b = ProcessHelper::Execute(QStringLiteral(R"(git -C "%1" remote -v)").arg(projectpath));
//            if(b.stdOut.startsWith(QStringLiteral("origin"))){
//                auto bl= com::helper::StringHelper::toStringList(b.stdOut);
//                QString fetch_url, push_url;
//                foreach (auto b1, bl) {
//                   auto b2 = b1.split(' ');
//                   if(b2.length()<3) continue;
//                   if(b2[2]=="(fetch)") fetch_url = b2[1];
//                   if(b2[2]=="(push)") push_url = b2[1];
//                }
//                if(fetch_url!=push_url){
//                    zInfo("push and fetch urls are differ");
//                }
//                if(fetch_url==settings.gitUrl){
//                    zInfo("repo ok");
//                    isOk = true;
//                }
//                else
//                {
//                    zInfo("repo and settings urls are differ");
//                }
//            }
//            else{ // no remote repo ->git remote add origin PATH/TO/REPO
//                zInfo("no origin");
//                isOk = true;
//            }
//        }
//        else{
//            zInfo("cannot use existing repo");
//        }

//        if(isOk){
//            //- fetch, checkout, push
//        }

//    }
    //-mentjük, frissítjük a fát
    com::helper::SettingsHelper::saveSettings();
    setRootPath(settings.projectPath);
}


void MainWindow::on_SettingsButton_clicked()
{
    auto r = DisplaySettingsDialog(MSG_ADDNEWDIALOG.arg(DIR));
    SettingsProcess(r);
}

/*
 * //git ls-tree --full-tree --name-only -r HEAD
//git ls-files --error-unmatch common.pri
// git  rev-parse --show-toplevel
//git -C /home/zoli/common/test2/ rev-parse --show-toplevel
*/
QString MainWindow::GetRepoURL(const QModelIndex &index){
    auto filepath = model->filePath(index);
    auto fileparent = model->fileInfo(index).absolutePath();

    auto out = ProcessHelper::Execute(QStringLiteral(R"(git -C "%1" rev-parse --show-toplevel)").arg(fileparent));
    if(out.exitCode!=0) return QString();
    if(out.stdOut.isEmpty()) return QString();
    QString rootpath = com::helper::StringHelper::GetFirstRow(out.stdOut);
    if(rootpath.isEmpty()) return QString();

    QString file;
    if(!model->isDir(index)){
        out = ProcessHelper::Execute(QStringLiteral(R"(git -C "%1" ls-files --error-unmatch "%2")").arg(rootpath).arg(filepath));
        if(out.exitCode!=0) return rootpath;
        if(out.stdOut.isEmpty()) return rootpath;
        file = com::helper::StringHelper::GetFirstRow(out.stdOut);
    }

    out = ProcessHelper::Execute(QStringLiteral(R"(git -C "%1" remote -v)").arg(rootpath));
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

void MainWindow::on_addToRepoButton_clicked()
{
    zTrace();
}

void MainWindow::on_cloneButton_clicked()
{
    zTrace();
}

void MainWindow::UpdateGitActionButtonState(const QString &s, const QModelIndex &index){
    if(s.isEmpty())
    { // nem git repo

        if(model->isDir(index))
        {
            ui->addToRepoButton->setEnabled(false);
            if(QDir(model->filePath(index)).isEmpty())  // ha a könyvtár üres, lehet bele klónozni
            {
                ui->cloneButton->setEnabled(true);
            }
            else
            {
                ui->cloneButton->setEnabled(false);
            }
        }
        else // ha fájl, nem lehet bele klónozni
        {
            ui->addToRepoButton->setEnabled(false);
            ui->cloneButton->setEnabled(false);
        }

    }
    else if(s.startsWith(QDir::separator()) || s.startsWith('g'))
    { // van remote
        ui->cloneButton->setEnabled(false);

        if(model->isDir(index))
        {
            ui->addToRepoButton->setEnabled(false);
        }
        else
        {
            if(!s.contains('|'))
            {
                ui->addToRepoButton->setEnabled(true);
            }
            else
            {
                ui->addToRepoButton->setEnabled(false);
            }
        }


    }
    else
    {
        ui->addToRepoButton->setEnabled(false);
        ui->cloneButton->setEnabled(false);
    }
}



