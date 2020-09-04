#ifndef GITNOTE_H
#define GITNOTE_H

#include <QMainWindow>
#include <QModelIndex>
#include <QString>

#include "clonedialog.h"

class GitNote
{
public:
    enum OpenType {DoubleClick, EditButton, Timer, Close};

    struct TextFileModel{
        QString name, txt;
        bool isValid(){ return !name.isEmpty(); }
    };

    struct SaveModel{
        TextFileModel txtfile;
        QModelIndex index;
        OpenType type;

        bool isValid(){ return !txtfile.isValid(); }
    };

    struct SaveModelR{
        TextFileModel txtfile;
        QModelIndex index;
        OpenType type;
    };

   static GitNote::SaveModelR Save(const SaveModel& m);

   static void SettingsProcess();

   static CloneDialog::Model DisplayCloneDialog(QMainWindow *w, const QString& title);
private:
   static TextFileModel Open(const QModelIndex& index);
};

#endif // GITNOTE_H
