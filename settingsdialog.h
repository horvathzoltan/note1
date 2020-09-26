#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include "common/macrofactory/macro.h"

#include <QDialog>
#include <QFileDialog>
#include <settings.h>

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT
public:
    struct Model{
    public:
        Settings *s;
    };

public:
    Model _model;
    explicit SettingsDialog(QWidget *parent = nullptr);
    ~SettingsDialog();

    void SetUI();

    GET(_model, model)
    SET_UI(_model, setModel, SetUI)

    void setTitle(const QString &title);
private slots:
    void on_SettingsDialog_accepted();
    void on_folderPickerButton_clicked();
    void checkGoToParent(const QString &);
private:

    Ui::SettingsDialog *ui;
    QFileDialog *filedialog;
};

#endif // SETTINGSDIALOG_H
