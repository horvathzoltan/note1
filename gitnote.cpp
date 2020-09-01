#include "gitnote.h"

#include "filesystemmodelhelper.h"

///
/// \brief menti txt-t, ha kell átnevezi a fájlt a modellen keresztül
///

GitNote::SaveModelR GitNote::Save(const SaveModel& m){
    FileSystemModelHelper::Save(m.txtfile.name, m.txtfile.txt);
    auto mr = GitNote::Open(m.index);
    return {mr, m.index, m.type};
}

///
/// \brief megnyitja indexet
///
/// megnyitja az indexet
/// -ha valós
/// -ha file
/// -ha nincsen megnyitva
///

GitNote::TextFileModel GitNote::Open(const QModelIndex& index)
{
    if(!index.isValid()) return {};
    if(FileSystemModelHelper::isDir(index)) return{};
    if(FileSystemModelHelper::Equals(index)) return{};
    auto filename = FileSystemModelHelper::fileName(index);
    auto txt2 = FileSystemModelHelper::Load(index);
    return {filename, txt2};
}
