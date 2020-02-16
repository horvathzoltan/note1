#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFileSystemModel>
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    QFileSystemModel *model;
    //QString filename;
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void Load(const QModelIndex &index);
    void Rename(const QModelIndex &index, const QString &fn);
    QModelIndex *getIndex();
private slots:
    void on_fileTreeView_doubleClicked(const QModelIndex &index);
    void on_EditButton_clicked();
    void closeEvent(QCloseEvent *event);
private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
