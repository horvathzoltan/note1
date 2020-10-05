#include "gitnote.h"

#include "common/logger/log.h"
#include "common/helper/settings/settingshelper.h"
#include "filesystemmodelhelper.h"
#include "settings.h"
#include "filenamehelper.h"
#include "githelper.h"

///
/// \brief menti txt-t, ha kell átnevezi a fájlt a modellen keresztül
///


GitNote::InfoModelR GitNote::Info(const GitNote::InfoModel &m)
{
    auto fileInfo = FileSystemModelHelper::fileInfo(m.index);
    auto giturl = GitHelper::GetRepoURL(fileInfo);

    return {giturl, m.index};
}

// az aktuálisat menti, a megadott néven, nem a megadott fájlt!
// TODO a FileSystemModelHelper aktuálist kell kommitolni!
GitNote::SaveModelR GitNote::Save(const SaveModel& m){
    bool isSaved = FileSystemModelHelper::Save(m.txtfile.name, m.txtfile.txt);
    if(isSaved)
    {
        if(!GitHelper::Commit(m.txtfile.name) || !GitHelper::Push()){
            zInfo("giterr");
        }
    }
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

void GitNote::SettingsProcess()
{
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

void GitNote::Clone(CloneModel m){
    QString path = FileSystemModelHelper::filePath(m.fileindex);

    auto r = GitNote::DisplayCloneDialog(m.w, QStringLiteral("Clone - %1").arg(path));
    if(!r.isValid()) return;
    GitHelper::clone(path, r.url, r.user, r.passwd);
}

const QString GitNote::MSG_ADDNEWDIALOG = QStringLiteral("Add new %1 name:");
const QString GitNote::MSG_ADDNEW =  QStringLiteral("New %1: %2");
const QString GitNote::DIR =  QStringLiteral("dir");
const QString GitNote::MSG_FAILEDTO = QStringLiteral("Failed to %1 %2: %3");
const QString GitNote::CREATE =  QStringLiteral("create");
const QString GitNote::FILE =  QStringLiteral("file");
const QString GitNote::DELETE =  QStringLiteral("delete");


void GitNote::AddDir(AddDirModel m){
    //auto ix = focusedIndex();
    auto mr = GitNote::DisplayNewDirDialog(m.w, MSG_ADDNEWDIALOG.arg(DIR));
    if(mr.filename.isEmpty()) return;
    QModelIndex newix;
    if(FileSystemModelHelper::isDir(m.fileindex))
        newix= FileSystemModelHelper::Mkdir(m.fileindex, mr.filename);
    else{
        auto px = FileSystemModelHelper::parent(m.fileindex);
        newix= FileSystemModelHelper::Mkdir(px, mr.filename);
    }
    if(newix.isValid())
        zInfo(MSG_ADDNEW.arg(DIR).arg(mr.filename))
    else
        zInfo(MSG_FAILEDTO.arg(CREATE).arg(DIR).arg(mr.filename));
}

NewFileDialog::Model GitNote::DisplayNewDirDialog(QMainWindow *main, const QString& title){
    if(title.isEmpty()) return NewFileDialog::Model();
    NewFileDialog dialog(main);
    dialog.setTitle(title);
    dialog.exec();
    if(dialog.result()!=QDialog::Accepted)
        return NewFileDialog::Model();
    return dialog.model();
}

NewFileDialog::Model GitNote::DisplayNewNoteDialog(QMainWindow *main, const QString& title){
    if(title.isEmpty()) return NewFileDialog::Model();
    NewFileDialog dialog(main);
    dialog.setTitle(title);
    dialog.exec();
    if(dialog.result()!=QDialog::Accepted)
        return NewFileDialog::Model();
    return dialog.model();
}


void GitNote::AddNote(AddNoteModel m){
    //auto ix = focusedIndex();

    auto dm = DisplayNewNoteDialog(m.w, GitNote::MSG_ADDNEWDIALOG.arg(GitNote::FILE));
    if(dm.filename.isEmpty()) return;
    //auto oldfn = model->filePath(ix);
    QString newfile;
    if(FileSystemModelHelper::isDir(m.fileindex))
    {
        newfile = QDir(FileSystemModelHelper::filePath(m.fileindex)).filePath(dm.filename);
    }
    else
    {
        newfile = FileSystemModelHelper::fileInfo(m.fileindex).absoluteDir().filePath(dm.filename);
    }

    QFile f(newfile);
#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
    auto isok = f.open(QIODevice::NewOnly| QIODevice::Text);
#else
    auto isok = !QFileInfo::exists(newfile) && f.open(QIODevice::Text);
#endif
    if(isok){
        f.close();
        zInfo(GitNote::MSG_ADDNEW.arg(GitNote::FILE).arg(newfile));
    }
    else{
        zInfo(GitNote::MSG_FAILEDTO.arg(GitNote::CREATE).arg(GitNote::FILE).arg(newfile));
    }
}

GitNote::SettingsModelR GitNote::Settings(SettingsModel m){
    auto r = GitNote::DisplaySettingsDialog(m.w, GitNote::MSG_ADDNEWDIALOG.arg(GitNote::DIR));
    if(r!=QDialog::Accepted) return {false};
    GitNote::SettingsProcess();
    return {true};
}

int GitNote::DisplaySettingsDialog(QMainWindow *main, const QString& title){
    if(title.isEmpty()) return -1;
    SettingsDialog dialog(main);
    dialog.setTitle(title);
    dialog.setModel({&settings});
    dialog.exec();
    return dialog.result();
}

void GitNote::Delete(DeleteModel m){
    //auto ix = focusedIndex();
    if(!FileSystemModelHelper::isValid()) return;
    if(FileSystemModelHelper::Equals(m.fileindex)) return;
    auto fn = FileSystemModelHelper::fileName(m.fileindex);
    if(FileSystemModelHelper::isDir(m.fileindex))
    {
        if(!FileSystemModelHelper::Rmdir(m.fileindex))
            zInfo(GitNote::MSG_FAILEDTO.arg(GitNote::DELETE).arg(GitNote::DIR).arg(fn))
    }
    else
    {
        if(!FileSystemModelHelper::Remove(m.fileindex))
            zInfo(GitNote::MSG_FAILEDTO.arg(GitNote::DELETE).arg(GitNote::FILE).arg(fn))
    }
}
