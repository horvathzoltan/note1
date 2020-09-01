#ifndef GITNOTE_H
#define GITNOTE_H

#include <QModelIndex>
#include <QString>



class GitNote
{
public:
    enum OpenType {DoubleClick, EditButton};

    struct TextFileModel{
        QString name, txt;
        bool isValid(){ return !name.isEmpty(); }
    };

    struct SaveModel{
        TextFileModel txtfile;
        QModelIndex index;
        OpenType type;

    };

    struct SaveModelR{
        TextFileModel txtfile;
        QModelIndex index;
        OpenType type;
    };

   static GitNote::SaveModelR Save(const SaveModel& m);

private:
   static TextFileModel Open(const QModelIndex& index);
};

#endif // GITNOTE_H
