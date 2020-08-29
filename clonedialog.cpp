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
//    isInited=false;
//    filedialog=nullptr;
}

CloneDialog::~CloneDialog()
{
    delete ui;
}

bool CloneDialog::isValid()
{
    return !(_url.isEmpty() || _passwd.isEmpty() || _user.isEmpty());
}
