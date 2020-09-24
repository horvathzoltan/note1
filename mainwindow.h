#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFileSystemModel>
#include <QMainWindow>
#include "common/logger/log.h"
#include "clonedialog.h"
#include "gitnote.h"
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE




class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    static const QModelIndex NullIndex;
    static const int AUTOSAVE_SEC;

    //QFileSystemModel *_model;
    //QModelIndex _model_index;
    //QString _model_hash;

    QTimer _autosave_timer;
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    const QModelIndex focusedIndex() const;

    static void msg(Errlevels::Levels errlevel, const QString &msg, const QString &loci, const QString &st, void *ui);

    QString DisplayNewDirDialog(const QString &title);

    void SettingsProcess();
    void UpdateEditorState();
    void UpdateActionButtonState(const QModelIndex &index);
    void setActionButtonState(bool x);
    void setEditorState(bool x);
    void updateFileTreeView();
    //QString GetRepoURL(const QModelIndex &index);
    bool AddRepoValidation(const QModelIndex &index);
    void updateGitActionButtonState(const QString &s, const QModelIndex &index);
    void Open(const QModelIndex& index);
    //void Save();
    void updateRepoButton(const QString &giturl, const QModelIndex &index);
    void updateCloneButton(const QString &giturl, const QModelIndex &index);    
    void setUi(GitNote::SaveModelR fm);
    void setUi(GitNote::InfoModelR m);
    //CloneDialog::Model DisplayCloneDialog(const QString &title);
    void setUi(GitNote::SettingsModelR m);
private slots:
    void on_fileTreeView_doubleClicked(const QModelIndex &index);
    void on_EditButton_clicked();
    void closeEvent(QCloseEvent *event);
    void on_addDirButton_clicked();    
    void on_deleteButton_clicked();
    void on_addNoteButton_clicked();
    void on_fileTreeView_clicked(const QModelIndex &index);
    void on_autosave_timer_timeout();
    void on_SettingsButton_clicked();

    void on_addToRepoButton_clicked();
    void on_cloneButton_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
