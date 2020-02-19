#ifndef NEWFILEDIALOG_H
#define NEWFILEDIALOG_H

#include <QDialog>

namespace Ui {
class NewFileDialog;
}

class NewFileDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewFileDialog(QWidget *parent = nullptr);
    ~NewFileDialog();
    void setTitle(const QString& title);
    QString filename();
private:
    Ui::NewFileDialog *ui;
};

#endif // NEWFILEDIALOG_H
