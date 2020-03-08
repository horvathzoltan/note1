#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFileSystemModel>
#include <QMainWindow>
#include "common/logger/log.h"
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    QFileSystemModel *model;
    QModelIndex model_index;
    QString model_hash;
    static const QString MSG_ADDNEW;
    static const QString MSG_ADDNEWDIALOG;
    static const QString MSG_FAILEDTO;
    static const QString FILE;
    static const QString DIR;
    static const QString DELETE;
    static const QString CREATE;
    static const int AUTOSAVE_SEC;
    QTimer autosave_timer;
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void Load(const QModelIndex &index);
    void Rename(const QModelIndex &index, const QString &fn);
    const QModelIndex getIndex();
    void Save(const QModelIndex &index);
    QString GetHash(const QString &txt);
    static void msg(Errlevels::Levels errlevel, const QString &msg, const QString &loci, const QString &st, void *ui);

    QString DisplayNewDirDialog(const QString &title);
    int DisplaySettingsDialog(const QString& title);
    void SettingsProcess(int);
    void UpdateEditorState();
    void UpdateActionButtonState(const QModelIndex &index);
    void setActionButtonState(bool x);
    void setEditorState(bool x);
    void Save();
    void setRootPath(const QString &path);
    QString GetRepoURL(const QModelIndex &index);
    bool AddRepoValidation(const QModelIndex &index);
    void UpdateGitActionButtonState(const QString &s, const QModelIndex &index);
private slots:
    void on_fileTreeView_doubleClicked(const QModelIndex &index);
    void on_EditButton_clicked();
    void closeEvent(QCloseEvent *event);
    void on_addDirButton_clicked();    
    void on_deleteButton_clicked();
    void on_addNoteButton_clicked();
    void on_fileTreeView_clicked(const QModelIndex &index);
    void on_timerupdate();
    void on_SettingsButton_clicked();

    void on_addToRepoButton_clicked();
    void on_cloneButton_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
