#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

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
    explicit SettingsDialog(QWidget *parent = nullptr);
    ~SettingsDialog();
    void init(Settings *s);
    void accept() override;
//    void SetData(Settings);
//    Settings GetData();
    void SetData();
    void GetData();
    //  bool eventFilter(QObject *o, QEvent *e) override;

private slots:
    void on_folderPickerButton_clicked();
    void checkGoToParent(const QString &);
    //void checkLineEdit(const QString &text);
private:
    Ui::SettingsDialog *ui;
    Settings *s;
    bool isInited;
    QFileDialog *filedialog;

    //static bool pathFits(const QString&);
};

#endif // SETTINGSDIALOG_H
