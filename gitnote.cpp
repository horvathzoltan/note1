#include "gitnote.h"

#include "common/helper/settings/settingshelper.h"
#include "filesystemmodelhelper.h"
#include "settings.h"
#include "filenamehelper.h"

///
/// \brief menti txt-t, ha kell átnevezi a fájlt a modellen keresztül
///


GitNote::SaveModelR GitNote::Save(const SaveModel& m){
    FileSystemModelHelper::Save(m.txtfile.name, m.txtfile.txt);
    if(m.type==Timer || m.type==Close) return {};
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

extern Settings settings;

void GitNote::SettingsProcess(){
    if(!settings.isValid()) return;
    com::helper::SettingsHelper::saveSettings();
    auto projectDir = FilenameHelper::GetProjectAbsolutePath();
    FileSystemModelHelper::setRootPath(projectDir);
}



CloneDialog::Model  GitNote::DisplayCloneDialog(QMainWindow *main, const QString& title){
    CloneDialog dialog(main);
    dialog.setTitle(title);
    dialog.exec();
    if(dialog.result() != QDialog::Accepted) return CloneDialog::Model();
    return dialog.model();
}
