#ifndef GITNOTE_H
#define GITNOTE_H

#include <QMainWindow>
#include <QModelIndex>
#include <QString>

#include "clonedialog.h"
#include "newfiledialog.h"
#include "settingsdialog.h"


class GitNote
{
public:
    static const QString MSG_ADDNEW;
    static const QString MSG_ADDNEWDIALOG;
    static const QString DIR;
    static const QString MSG_FAILEDTO;
    static const QString CREATE;
    static const QString FILE;
    static const QString DELETE;


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

    struct InfoModel{  static const QString FILE;
        static const QString DELETE;
        static const int AUTOSAVE_SEC;
        static const QModelIndex NullIndex;
        const QModelIndex& index;
    };

    struct InfoModelR{
        QString giturl;
        const QModelIndex& index;
    };

   static GitNote::InfoModelR Info(const InfoModel& m);

   static GitNote::SaveModelR Save(const SaveModel& m, QObject *parent);

   static void SettingsProcess();   

   struct CloneModel{
       QMainWindow*w;
       const QModelIndex& fileindex;
   };

   static void Clone(CloneModel m);

   struct AddDirModel{
       QMainWindow*w;
       const QModelIndex& fileindex;
   };
   static void AddDir(AddDirModel m);

   struct SettingsModel{
       QMainWindow *w;
   };

   struct SettingsModelR{
       bool state;
   };

   static SettingsModelR Settings(SettingsModel m);

   struct AddNoteModel{
       QMainWindow*w;
       const QModelIndex& fileindex;
   };
   static void AddNewNote(AddNoteModel m);

   struct DeleteModel{
       const QModelIndex& fileindex;
   };

   static void Delete(DeleteModel m);

   struct AddNoteModelR{
       bool state;
       QString giturl;
       const QModelIndex& index;
   };
   static AddNoteModelR AddNote(AddNoteModel m);
private:    
    static TextFileModel Open(const QModelIndex& index);
    static CloneDialog::Model DisplayCloneDialog(QMainWindow *w, const QString& title);
    static NewFileDialog::Model DisplayNewDirDialog(QMainWindow *w, const QString& title);
    static NewFileDialog::Model DisplayNewNoteDialog(QMainWindow *w, const QString& title);
    static int DisplaySettingsDialog(QMainWindow *main, const QString& title);
};



#endif // GITNOTE_H
