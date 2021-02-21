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
#include <QMessageBox>
#include "common/logger/log.h"
//#include <QCryptographicHash>

#include "common/helper/settings/settingshelper.h"
#include "processhelper.h"
#include "common/helper/string/stringhelper.h"
#include "filesystemmodelhelper.h"
#include "gitnote.h"

extern Settings settings;

const int MainWindow::AUTOSAVE_SEC = 120;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

//    for (int i = 1; i < FileSystemModelHelper::columnCount(); ++i)
//        ui->fileTreeView->hideColumn(i);
    //ui->fileTreeView->setColumnHidden(3, false);
    //ui->fileTreeView->hideColumn(3);
    UpdateEditorState();
    connect(&_autosave_timer, &QTimer::timeout, this, &MainWindow::on_autosave_timer_timeout);
}

MainWindow::~MainWindow()
{
    //FileSystemModelHelper::uninit();
    delete ui;
}

void MainWindow::on_autosave_timer_timeout(){
    setUi(
        GitNote::Save({
            {
                ui->filenameEdit->text(),
                ui->plainTextEdit->toPlainText()
            },
            focusedIndex(),
            GitNote::Timer
                      })
        );
}


void MainWindow::msg(Errlevels::Levels errlevel, const QString &msg, const QString &loci, const QString &st, void *w, int flag)
{
    Q_UNUSED( errlevel )
    Q_UNUSED( loci )
    Q_UNUSED( st )

    if(flag<1)
    {
        reinterpret_cast<MainWindow*>(w)->statusBar()->showMessage(msg, 2000);
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText(msg);
        int r = msgBox.exec();
        //reinterpret_cast<MainWindow*>(w)->statusBar()->showMessage(msg, 2000);
    }
}

/*
* filetree
*/

const QModelIndex MainWindow::NullIndex = QModelIndex();

auto MainWindow::focusedIndex() const -> const QModelIndex{
    auto indexes = ui->fileTreeView->selectionModel()->selectedIndexes();
    if (indexes.isEmpty()) return NullIndex;
    return indexes.first();
}

void MainWindow::updateFileTreeView()
{
    auto m = FileSystemModelHelper::Model();
    auto ix = FileSystemModelHelper::Index();

    ui->fileTreeView->setModel(m);
    ui->fileTreeView->setRootIndex(ix);

    for (int i = 1; i < FileSystemModelHelper::columnCount(); ++i)
    {
        ui->fileTreeView->hideColumn(i);
    }
}

//singleclick
void MainWindow::on_fileTreeView_clicked(const QModelIndex &index)
{
    setUi(GitNote::Info({index}));
}

//doubleclick
void MainWindow::on_fileTreeView_doubleClicked(const QModelIndex &index)
{
    Q_UNUSED(index)
    auto ix = focusedIndex();//FileSystemModelHelper::Index();

    setUi(
        GitNote::Save({
            {
                ui->filenameEdit->text(),
                ui->plainTextEdit->toPlainText()
            },
            ix,
            GitNote::DoubleClick
        })
        );
}


/*
* fájlmentés
*/
void MainWindow::on_EditButton_clicked()
{
    auto ix = focusedIndex();// FileSystemModelHelper::Index();
    setUi(
        GitNote::Save({
            {
                ui->filenameEdit->text(),
                ui->plainTextEdit->toPlainText()
            },
            ix,
            GitNote::EditButton
        })
        );
}

void MainWindow::closeEvent(QCloseEvent *event) {
    Q_UNUSED (event)
    setUi(
        GitNote::Save({
            {
                ui->filenameEdit->text(),
                ui->plainTextEdit->toPlainText()
            },
            focusedIndex(),
            GitNote::Close
        })
        );
}


/*
* új mappa
*/



void MainWindow::on_addDirButton_clicked()
{
    ui->addDirButton->setDisabled(true);
    QCoreApplication::processEvents();
    GitNote::AddDir({this, focusedIndex()});
    ui->addDirButton->setDisabled(false);
}

void MainWindow::on_deleteButton_clicked()
{
    ui->deleteButton->setDisabled(true);
    QCoreApplication::processEvents();

    GitNote::Delete({focusedIndex()});
    ui->fileTreeView->clearSelection();

    ui->deleteButton->setDisabled(false);
}


/*
 * új note
*/

//NewNote
void MainWindow::on_addNoteButton_clicked()
{
    ui->addNoteButton->setDisabled(true);
    QCoreApplication::processEvents();
    GitNote::AddNewNote({this,focusedIndex()});
    ui->addNoteButton->setDisabled(false);
}

void MainWindow::on_SettingsButton_clicked()
{
    setUi(GitNote::Settings({this}));
}

void MainWindow::setUi(GitNote::SettingsModelR m)
{
    if(m.state)
        updateFileTreeView();
}

//AddToRepo
void MainWindow::on_addToRepoButton_clicked()
{
    //ui->addToRepoButton->setDisabled(true);

    GitNote::AddNoteModel m{this,focusedIndex()};
    ui->addToRepoButton->setDisabled(true);
    GitNote::AddNoteModelR a = GitNote::AddNote(m);

    //auto t = FileSystemModelHelper::isDir(a.index);
    setUi(a);
   // ui->addToRepoButton->setEnabled(true);
}

void MainWindow::setUi(GitNote::AddNoteModelR m)
{
    if(m.state)
    {
        setUi(GitNote::InfoModelR {m.giturl, m.index});
    }
}

void MainWindow::on_cloneButton_clicked()
{
    GitNote::Clone({this, focusedIndex()});
}

void MainWindow::updateRepoButton(const QString &giturl, const QModelIndex &index){
    auto nab = !(FileSystemModelHelper::isDir(index)||giturl.isEmpty());

    auto ne = !giturl.contains('|');
    auto dne =  giturl.startsWith('g') && ne;
    auto cne =  giturl.startsWith(QDir::separator()) && ne;

    auto e = (nab&&dne)||(nab&&cne);

    ui->addToRepoButton->setEnabled(e);
}

void MainWindow::updateCloneButton(const QString &giturl, const QModelIndex &index){
//    auto path = FileSystemModelHelper::filePath(index);
//    QFileInfo fi = QFileInfo(path);
//    auto tl = GitHelper::GetToplevel(fi);
//
    auto e =
        giturl.isEmpty() &&
        FileSystemModelHelper::isDir(index);
        ; /*&&
        QDir(FileSystemModelHelper::filePath(index)).isEmpty();*/

    ui->cloneButton->setEnabled(e);
}

void MainWindow::updateGitActionButtonState(const QString &giturl, const QModelIndex &index){
    updateRepoButton(giturl, index);
    updateCloneButton(giturl, index);
}

void MainWindow::setUi(GitNote::InfoModelR m){
    UpdateActionButtonState(m.index);
    updateGitActionButtonState(m.giturl, m.index);
    ui->repolabel->setText(m.giturl);
}

void MainWindow::setUi(GitNote::SaveModelR m)
{
    if(!m.txtfile.isValid()) return;
    
    ui->plainTextEdit->setPlainText(m.txtfile.txt);
    ui->filenameEdit->setText(m.txtfile.name);
    UpdateEditorState();
    if(m.type==GitNote::EditButton) UpdateActionButtonState(m.index);
}


void MainWindow::UpdateEditorState(){
    MainWindow::setEditorState(FileSystemModelHelper::isValid());
}

void MainWindow::setEditorState(bool x){
    ui->filenameEdit->setEnabled(x);
    ui->plainTextEdit->setEnabled(x);
    if(x) _autosave_timer.start(1000*AUTOSAVE_SEC);
    else _autosave_timer.stop();
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
    //ui->deleteButton->setEnabled(x);
}

void MainWindow::on_fileTreeView_expanded(const QModelIndex &index)
{
     GitNote::DirRefresh({index});
}
