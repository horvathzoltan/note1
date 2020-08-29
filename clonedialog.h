#ifndef CLONEDIALOG_H
#define CLONEDIALOG_H

#include <QWidget>
#include <QDialog>

#define GET(v, name) const decltype(v)& name() const {return v;}

namespace Ui {
class CloneDialog;
}

class CloneDialog : public QDialog
{
    Q_OBJECT

private:
    QString  _url, _user, _passwd;
public:        
    CloneDialog(QWidget *parent);
    ~CloneDialog();
    bool isValid();
    //const decltype(_url)& url() const {return _url;}
    //const QString& user() const {return _user;}
    //const QString& passwd() const {return _passwd;}
    GET(_url, url)
    GET(_user, user)
    GET(_passwd, passwd)

private:    
    Ui::CloneDialog *ui;
};

#endif // CLONEDIALOG_H
