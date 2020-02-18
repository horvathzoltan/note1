#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFileSystemModel>
#include <QMainWindow>
#include "common/logger/log.h"

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
    //QString filename;
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void Load(const QModelIndex &index);
    void Rename(const QModelIndex &index, const QString &fn);
    const QModelIndex getIndex();
    void Save(const QModelIndex &index);
    QString GetHash(const QString &txt);
    static void msg(Errlevels::Levels errlevel, const QString &msg, const QString &loci, const QString &st, void *ui);

private slots:
    void on_fileTreeView_doubleClicked(const QModelIndex &index);
    void on_EditButton_clicked();
    void closeEvent(QCloseEvent *event);
    void on_addDirButton_clicked();
    //typedef void (*zLogGUIfn)(Errlevels::Levels errlevel, const QString &msg, const QString &loci, const QString &st, void *ui);


private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
