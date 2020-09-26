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
    filedialog=nullptr;
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::setTitle(const QString &title)
{
    setWindowTitle(title);
}


void SettingsDialog::SetUI(){
    ui->projectPathEdit->setText(_model.s->projectPath);
}


void SettingsDialog::on_SettingsDialog_accepted()
{
    _model.s->projectPath = FilenameHelper::normalaizedir(ui->projectPathEdit->text());
}

void SettingsDialog::on_folderPickerButton_clicked()
{
    filedialog= new QFileDialog(this, "Select Firmware Version");
    //QFileDialog dlg(this, "Select Firmware Version");
    filedialog->setDirectory(QDir::home());
    filedialog->setFileMode(QFileDialog::DirectoryOnly);
    filedialog->setOption(QFileDialog::ReadOnly, true);
    filedialog->setOption(QFileDialog::HideNameFilterDetails, true);
    filedialog->setViewMode(QFileDialog::List);
    filedialog->setAcceptMode(QFileDialog::AcceptOpen);
    checkGoToParent(filedialog->directory().path());

    connect(filedialog, SIGNAL(directoryEntered(const QString &)), this, SLOT(checkGoToParent(const QString &)));
    auto fe = filedialog->findChild<QLineEdit *>("fileNameEdit");
    fe->setEnabled(false);

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

