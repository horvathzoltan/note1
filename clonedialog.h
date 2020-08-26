#ifndef CLONEDIALOG_H
#define CLONEDIALOG_H

#include <QWidget>
#include <QDialog>

namespace Ui {
class CloneDialog;
}

class CloneDialog : public QDialog
{
    Q_OBJECT
public:
    CloneDialog(QWidget *parent);
    ~CloneDialog();
private:
    Ui::CloneDialog *ui;
};

#endif // CLONEDIALOG_H
