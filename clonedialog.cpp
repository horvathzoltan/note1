#include "clonedialog.h"
#include "filenamehelper.h"
//#include "settingsdialog.h"
#include "ui_clonedialog.h"
#include "common/logger/log.h"

//#include <QFileDialog>
//#include <QToolButton>
//#include <QCompleter>
//#include <QAbstractItemView>
//#include <QKeyEvent>

CloneDialog::CloneDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CloneDialog)
{
    ui->setupUi(this);
}

CloneDialog::~CloneDialog()
{
    delete ui;
}

void CloneDialog::setTitle(QString a){
    setWindowTitle(a);
}


bool CloneDialog::Model::isValid() const
{
    return !(url.isEmpty() || passwd.isEmpty() || user.isEmpty());
}

void CloneDialog::on_CloneDialog_accepted()
{
    _model.url = ui->gitUrlEdit->text();
    _model.user = ui->gitUserEdit->text();
    _model.passwd = ui->gitPasswdEdit->text();
}
