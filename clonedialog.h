#ifndef CLONEDIALOG_H
#define CLONEDIALOG_H

#include "common/macrofactory/macro.h"

#include <QWidget>
#include <QDialog>


namespace Ui {
class CloneDialog;
}

class CloneDialog : public QDialog
{
    Q_OBJECT

public:
    struct Model{
        public:
            QString url, user, passwd;
            bool isValid() const;
    };

private:
    Ui::CloneDialog *ui;
    Model _model;
public:        
    CloneDialog(QWidget *parent);
    ~CloneDialog();    
    GET(_model, model)
    SET(_model, setModel)
//    /#define GET(v, name) const decltype(v)& name() const {return v;}
    //decltype(_model)& model() {return _model;}
    //void setModel(const decltype(_model)& m) {_model = m;}

    void setTitle(QString a);
private slots:
    void on_CloneDialog_accepted();
};

#endif // CLONEDIALOG_H
