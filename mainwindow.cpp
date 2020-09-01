#include "clonedialog.h"
#include "filenamehelper.h"
#include "filesystemmodelhelper.h"
#include "githelper.h"
#include "mainwindow.h"
#include "mainwindow.h"
#include "newfiledialog.h"
#include "settings.h"
#include "ui_mainwindow.h"
//#include "common/helper/file/filehelper.h"
#include <QFileSystemModel>
#include "common/logger/log.h"
//#include <QCryptographicHash>
#include "settingsdialog.h"
#include "common/helper/settings/settingshelper.h"
#include "processhelper.h"
#include "common/helper/string/stringhelper.h"
#include "filesystemmodelhelper.h"
#include "gitnote.h"

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

    for (int i = 1; i < FileSystemModelHelper::columnCount(); ++i)
        ui->fileTreeView->hideColumn(i);
    UpdateEditorState();
    connect(&_autosave_timer, &QTimer::timeout, this, &MainWindow::on_autosave_timer_timeout);
}

void MainWindow::on_autosave_timer_timeout(){
    auto fn = ui->filenameEdit->text();
    auto txt = ui->plainTextEdit->toPlainText();
    FileSystemModelHelper::Save(fn, txt);
}

MainWindow::~MainWindow()
{
    //FileSystemModelHelper::uninit();
    delete ui;
}

void MainWindow::msg(Errlevels::Levels errlevel, const QString &msg, const QString &loci, const QString &st, void *w)
{
    Q_UNUSED( errlevel )
    Q_UNUSED( loci )
    Q_UNUSED( st )

    reinterpret_cast<MainWindow*>(w)->statusBar()->showMessage(msg, 2000);
}


/*fileTreeView events*/
void MainWindow::on_fileTreeView_clicked(const QModelIndex &index)
{
    UpdateActionButtonState(index);
    auto fileInfo = FileSystemModelHelper::fileInfo(index);

    auto giturl = GitHelper::GetRepoURL(fileInfo);
    updateGitActionButtonState(giturl, index);

    ui->repolabel->setText(giturl);
}

void MainWindow::on_fileTreeView_doubleClicked(const QModelIndex &index)
{
    Q_UNUSED(index)

    setUi(
        GitNote::Save({
            {
                ui->filenameEdit->text(),
                ui->plainTextEdit->toPlainText()
            },
            focusedIndex(),
            GitNote::DoubleClick
        })
        );
}

void MainWindow::on_EditButton_clicked()
{
    setUi(
        GitNote::Save({
            {
                ui->filenameEdit->text(),
                ui->plainTextEdit->toPlainText()
            },
            focusedIndex(),
            GitNote::EditButton
        })
        );
}

void MainWindow::setUi(GitNote::SaveModelR m)
{
    if(!m.txtfile.isValid()) return;

    ui->plainTextEdit->setPlainText(m.txtfile.txt);
    ui->filenameEdit->setText(m.txtfile.name);
    UpdateEditorState();
    if(m.type==GitNote::EditButton) UpdateActionButtonState(m.index);
}

const QModelIndex MainWindow::NullIndex = QModelIndex();

const QModelIndex MainWindow::focusedIndex() const{
    auto indexes = ui->fileTreeView->selectionModel()->selectedIndexes();
    if (indexes.isEmpty()) return NullIndex;
    //auto a = indexes[0];//.at(0);
    return indexes[0];
}

void MainWindow::closeEvent(QCloseEvent *event) {
    Q_UNUSED (event)
    auto fn = ui->filenameEdit->text();
    auto txt = ui->plainTextEdit->toPlainText();
    FileSystemModelHelper::Save(fn, txt);
}

void MainWindow::on_addDirButton_clicked()
{
    auto ix = focusedIndex();
    auto fn = DisplayNewDirDialog(MSG_ADDNEWDIALOG.arg(DIR));
    if(fn.isEmpty()) return;
    QModelIndex newix;
    if(FileSystemModelHelper::isDir(ix))
        newix= FileSystemModelHelper::Mkdir(ix, fn);
    else{
        auto px = FileSystemModelHelper::parent(ix);
        newix= FileSystemModelHelper::Mkdir(px, fn);
    }
    if(newix.isValid())
        zInfo(MSG_ADDNEW.arg(DIR).arg(fn))
    else
        zInfo(MSG_FAILEDTO.arg(CREATE).arg(DIR).arg(fn));

}


void MainWindow::on_deleteButton_clicked()
{
    auto ix = focusedIndex();
    if(!FileSystemModelHelper::isValid()) return;
    if(FileSystemModelHelper::Equals(ix)) return;
    auto fn = FileSystemModelHelper::fileName(ix);
    if(FileSystemModelHelper::isDir(ix))
    {
        if(!FileSystemModelHelper::Rmdir(ix))
            zInfo(MSG_FAILEDTO.arg(DELETE).arg(DIR).arg(fn))
    }
    else
    {
        if(!FileSystemModelHelper::Remove(ix))
            zInfo(MSG_FAILEDTO.arg(DELETE).arg(FILE).arg(fn))
    }
}


void MainWindow::on_addNoteButton_clicked()
{
    auto ix = focusedIndex();
    auto fn = DisplayNewDirDialog(MSG_ADDNEWDIALOG.arg(FILE));
    if(fn.isEmpty()) return;
    //auto oldfn = model->filePath(ix);
    QString newfile;
    if(FileSystemModelHelper::isDir(ix))
    {
        newfile = QDir(FileSystemModelHelper::filePath(ix)).filePath(fn);
    }
    else
    {
        newfile = FileSystemModelHelper::fileInfo(ix).absoluteDir().filePath(fn);
    }

    QFile f(newfile);
#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
    auto isok = f.open(QIODevice::NewOnly| QIODevice::Text);
#else
    auto isok = !QFileInfo::exists(newfile) && f.open(QIODevice::Text);
#endif
    if(isok){
        f.close();
        zInfo(MSG_ADDNEW.arg(FILE).arg(newfile));
    }
    else{
        zInfo(MSG_FAILEDTO.arg(CREATE).arg(FILE).arg(newfile));

    }
}

void MainWindow::UpdateEditorState(){
    if(FileSystemModelHelper::isValid()){
        MainWindow::setEditorState(true);
    }else{
        MainWindow::setEditorState(false);
    }
}

void MainWindow::setEditorState(bool x){
    ui->filenameEdit->setEnabled(x);
    ui->plainTextEdit->setEnabled(x);
    if(x) _autosave_timer.start(1000*AUTOSAVE_SEC);
    else _autosave_timer.stop();
}

void MainWindow::updateFileTreeView()
{
    auto m = FileSystemModelHelper::Model();
    auto ix = FileSystemModelHelper::Index();

    ui->fileTreeView->setModel(m);
    ui->fileTreeView->setRootIndex(ix);
}

void MainWindow::UpdateActionButtonState(const QModelIndex &index){
    auto a =
        FileSystemModelHelper::isValid() &&
        !(
            FileSystemModelHelper::Equals(index) ||
            FileSystemModelHelper::isDir(index)
            );

    setActionButtonState(a);
}


///
/// \brief a szerkesztő actionok buttonjait engedélyezi/tiltja
/// \param x
///
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
    auto m = dialog.model();
    if(!m.isValid()) return QString();

    return m.filename;
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

///
/// \brief Feldolgozza a settingst
///

void MainWindow::SettingsProcess(){
    if(!settings.isValid()) return;
    com::helper::SettingsHelper::saveSettings();
    auto projectDir = FilenameHelper::GetProjectAbsolutePath();
    FileSystemModelHelper::setRootPath(projectDir);
    updateFileTreeView();
}


void MainWindow::on_SettingsButton_clicked()
{
    auto r = DisplaySettingsDialog(MSG_ADDNEWDIALOG.arg(DIR));
    if(r!=QDialog::Accepted) return;
    SettingsProcess();
}



void MainWindow::on_addToRepoButton_clicked()
{
    zTrace()
}

void MainWindow::on_cloneButton_clicked()
{
    // ha még nem git könyvtár
    //zTrace();
    auto fileindex = focusedIndex();
    QString path = FileSystemModelHelper::filePath(fileindex);

    CloneDialog dialog(this);
    dialog.setTitle(QStringLiteral("Clone - %1").arg(path));
    dialog.exec();

    if(dialog.result() != QDialog::Accepted) return;
    auto m = dialog.model();
    if(!m.isValid()) return;

    GitHelper::clone(path, m.url, m.user, m.passwd);
}

void MainWindow::updateRepoButton(const QString &giturl, const QModelIndex &index){
    auto nab = !FileSystemModelHelper::isDir(index)&&
             !giturl.isEmpty();

    auto ne = !giturl.contains('|');
    auto dne =  giturl.startsWith('g') && ne;
    auto cne =  giturl.startsWith(QDir::separator()) && ne;

    auto e = (nab&&dne)||(nab&&cne);

    ui->addToRepoButton->setEnabled(e);
}


void MainWindow::updateCloneButton(const QString &giturl, const QModelIndex &index){
    auto e =
        giturl.isEmpty() &&
        FileSystemModelHelper::isDir(index) &&
        QDir(FileSystemModelHelper::filePath(index)).isEmpty();

    ui->cloneButton->setEnabled(e);
}

void MainWindow::updateGitActionButtonState(const QString &giturl, const QModelIndex &index){
    updateRepoButton(giturl, index);
    updateCloneButton(giturl, index);
}

///
/// \brief menti txt-t, ha kell átnevezi a fájlt a modellen keresztül
///

//void MainWindow::Save(){
//    auto fn =  ui->filenameEdit->text();
//    auto txt = ui->plainTextEdit->toPlainText();
//    FileSystemModelHelper::Save(fn, txt);
//}

///
/// \brief megnyitja indexet
///
/// megnyitja az indexet
/// -ha valós
/// -ha file
/// -ha nincsen megnyitva
///

void MainWindow::Open(const QModelIndex& index)
{
    if(!index.isValid()) return;
    if(FileSystemModelHelper::isDir(index)) return;
    if(FileSystemModelHelper::Equals(index)) return;
    auto filename = FileSystemModelHelper::fileName(index);
    auto txt2 = FileSystemModelHelper::Load(index);
    ui->plainTextEdit->setPlainText(txt2);
    ui->filenameEdit->setText(filename);
    UpdateEditorState();
}



