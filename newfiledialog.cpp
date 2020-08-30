#include "newfiledialog.h"
#include "ui_newfiledialog.h"

NewFileDialog::NewFileDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewFileDialog)
{
    ui->setupUi(this);
}

NewFileDialog::~NewFileDialog()
{
    delete ui;
}

void NewFileDialog::setTitle(const QString &title)
{
    setWindowTitle(title);
}

void NewFileDialog::on_NewFileDialog_accepted()
{
    _model.filename = ui->lineEdit->text();
}

bool NewFileDialog::Model::isValid() const
{
    return !filename.isEmpty();
}
