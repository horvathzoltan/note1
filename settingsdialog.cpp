#include "filenamehelper.h"
#include "settingsdialog.h"
//#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "common/logger/log.h"

#include <QFileDialog>
#include <QToolButton>
#include <QCompleter>
#include <QAbstractItemView>
#include <QKeyEvent>

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    isInited=false;
    filedialog=nullptr;
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::init(Settings *s)
{
    this->s = s;
    isInited = true;
    SetData();
}

void SettingsDialog::setTitle(const QString &title)
{
    setWindowTitle(title);
}

//void SettingsDialog::SetData(Settings s){
//    if(!isInited) return;
//    ui->projectPathEdit->setText(s.projectPath);
//    ui->gitUrlEdit->setText(s.gitUrl);
//    ui->gitUserEdit->setText(s.gitUser);
//    ui->gitPasswdEdit->setText(s.gitPasswd);
//}

//Settings SettingsDialog::GetData(){
//    if(!isInited) return Settings();

//    Settings s;
//    s.projectPath = FilenameHelper::normalaizedir(ui->projectPathEdit->text());
//    s.gitUrl = ui->gitUrlEdit->text();
//    // teszt ha nem elérhető
//    s.gitUser = ui->gitUserEdit->text();
//    s.gitPasswd = ui->gitPasswdEdit->text();
//    // teszt ha ok

//    return s;
//}

void SettingsDialog::SetData(){
    if(!isInited) return;
    ui->projectPathEdit->setText(s->projectPath);
//    ui->gitUrlEdit->setText(s->gitUrl);
//    ui->gitUserEdit->setText(s->gitUser);
//    ui->gitPasswdEdit->setText(s->gitPasswd);
}

void SettingsDialog::GetData(){
    if(!isInited) return;
    s->projectPath = FilenameHelper::normalaizedir(ui->projectPathEdit->text());
//    s->gitUrl = ui->gitUrlEdit->text();
//    // teszt ha nem elérhető
//    s->gitUser = ui->gitUserEdit->text();
//    s->gitPasswd = ui->gitPasswdEdit->text();
    // teszt ha ok
}

void SettingsDialog::accept()
{
    if(!isInited) return;
    GetData();
    zInfo("Settings accepted");
    QDialog::accept();
}

void SettingsDialog::on_folderPickerButton_clicked()
{//filedialog
//    QString dir = QFileDialog::getExistingDirectory(
//        this, "Pick a dir", QDir::homePath(),
//        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    filedialog= new QFileDialog(this, "Select Firmware Version");
    //QFileDialog dlg(this, "Select Firmware Version");
    filedialog->setDirectory(QDir::home());
    filedialog->setFileMode(QFileDialog::DirectoryOnly);
    filedialog->setOption(QFileDialog::ReadOnly, true);
    filedialog->setOption(QFileDialog::HideNameFilterDetails, true);
    filedialog->setViewMode(QFileDialog::List);
    filedialog->setAcceptMode(QFileDialog::AcceptOpen);
    checkGoToParent(filedialog->directory().path());

    //directoryEntered(const QString& directory)
    //connect(&dlg, SIGNAL(directoryEntered(QString)), this, SLOT(checkGoToParent()));
    connect(filedialog, SIGNAL(directoryEntered(const QString &)), this, SLOT(checkGoToParent(const QString &)));
    auto fe = filedialog->findChild<QLineEdit *>("fileNameEdit");
    fe->setEnabled(false);
//    connect(fe, SIGNAL(textChanged(const QString &)), this, SLOT(checkLineEdit(const QString &)));

//    fe->installEventFilter(this);
//    fe->completer()->popup()->installEventFilter(this);

    filedialog->exec();
    QString dir = filedialog->selectedFiles().first();
    delete(filedialog);
    if(dir.startsWith(QDir::homePath()))
       dir= dir.mid(QDir::homePath().length()+1);
    ui->projectPathEdit->setText(dir);

}
// https://stackoverflow.com/questions/12169878/limit-directory-traversal-in-qfiledialog

void SettingsDialog::checkGoToParent(const QString &a)
{
    if(filedialog==nullptr) return;
    auto b = filedialog->findChild<QToolButton *>("toParentButton");

    //b->setEnabled(a!=QDir::homePath());
    if(!a.startsWith(QDir::homePath())){
        filedialog->setDirectory(QDir::homePath());
        b->setEnabled(false);
    }
    else if(a==QDir::homePath()){
        b->setEnabled(false);
    }
    else{
        b->setEnabled(true);
    }
    //zInfo(QStringLiteral("dir: %1").arg(a));
}

//void SettingsDialog::checkLineEdit(const QString &text)
//{
//zInfo(QStringLiteral("checkLineEdit: %1").arg(text));
//    if(filedialog==nullptr) return;

//    QAbstractButton *btn = findChild<QDialogButtonBox *>("buttonBox")->buttons().first();
//    QString path = QDir::cleanPath(filedialog->directory().absolutePath());
//    if(!text.startsWith(QDir::separator())) path += QDir::separator();
//    path += text;
//    bool a = QDir(text).isAbsolute();

//    //btn->setEnabled(btn->isEnabled() && ((!a && pathFits(path)) || (a && pathFits(text))));
////    if(a){
////        if(pathFits(text)){
////            zInfo("1");
////            btn->setEnabled(true);
////        }
////        else{
////            zInfo("2");
////            btn->setEnabled(false);
////        }
////    }
////    else{
////        if(pathFits(path)){
////            zInfo("3");
////            btn->setEnabled(true);
////        }
////        else{
////            zInfo("4");
////            btn->setEnabled(false);
////        }
////    }
//}

//bool SettingsDialog::pathFits(const QString &p)
//{
//    //zInfo(QStringLiteral("dir: %1").arg(p));
//    if(!QDir(p).exists(p)) return false;
//    if(p==QDir::homePath()) return false;
//    if(p.startsWith(QDir::homePath())) return true;
//    return false;

//}

//bool SettingsDialog::eventFilter(QObject *o, QEvent *e)
//{
//    zInfo("aaaa");
//    if (e->type() != QEvent::KeyPress)
//        return false;
//    int key = static_cast<QKeyEvent *>(e)->key();
//    if (o->objectName() == "listView" || o->objectName() == "treeView")
//    {
//        return (Qt::Key_Backspace == key && !pathFits(filedialog->directory().absolutePath()));
//    }
//    else
//    {
//        if (Qt::Key_Return != key && Qt::Key_Enter != key)
//            return false;
//        QString text = findChild<QLineEdit *>("fileNameEdit")->text();
//        QString path = QDir::cleanPath(filedialog->directory().absolutePath() + (text.startsWith("/") ? "" : "/") + text);
//        bool a = QDir(text).isAbsolute();
//        return !((!a && pathFits(path)) || (a && pathFits(text)));
//    }
//}

