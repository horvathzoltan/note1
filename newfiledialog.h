#ifndef NEWFILEDIALOG_H
#define NEWFILEDIALOG_H

#include "common/macrofactory/macro.h"
#include <QDialog>

namespace Ui {
class NewFileDialog;
}

class NewFileDialog : public QDialog
{
    Q_OBJECT
    struct Model{
        QString filename;
        bool isValid() const;
    };
private:
    Ui::NewFileDialog *ui;
    Model _model;
public:
    explicit NewFileDialog(QWidget *parent = nullptr);
    ~NewFileDialog();
    void setTitle(const QString& title);
    GET(_model, model);
private slots:
    void on_NewFileDialog_accepted();
};

#endif // NEWFILEDIALOG_H
