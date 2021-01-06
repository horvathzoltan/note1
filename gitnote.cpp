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
// majd pedig megnyitja az aktuálisat
// ha git-követett, akkor fetcheli
GitNote::SaveModelR GitNote::Save(const SaveModel& m)
{
    static const QString FN = "Update";
    bool isSaved = FileSystemModelHelper::Save(m.txtfile.name, m.txtfile.txt);
    if(isSaved)
    {
        auto ix = FileSystemModelHelper::Index();
        auto fi = FileSystemModelHelper::fileInfo(ix);
        if(GitHelper::isTracked(fi))
        {
            auto pix = FileSystemModelHelper::parent(ix);
            auto fp = FileSystemModelHelper::filePath(pix);

//            QString e;
//            bool is_comm_ok = GitHelper::Commit(fp, m.txtfile.name, FN, &e);
//            if(!is_comm_ok)
//            {
//                zError2("commit err\n\n"+e, 2);
//                goto end;
//            }
            //bool is_push_ok = GitHelper::Push(fp, &e);
            bool is_push_ok = GitHelper::Refresh(fp, FN, GitHelper::Push);
//            if(!is_comm_ok || !is_push_ok)
//            {
//                zError2("push err\n\n"+e, 2);
//            }
        }
    }
    end:
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

    //auto filepath = FileSystemModelHelper::parent(index);
//    auto ix = FileSystemModelHelper::Index();
    auto pix = FileSystemModelHelper::parent(index);
    auto fp = FileSystemModelHelper::filePath(pix);
    auto fi = FileSystemModelHelper::fileInfo(index);
    if(GitHelper::isTracked(fi))
    {
        static const QString FN = "Fetch";
        GitHelper::Refresh(fp, FN, GitHelper::Pull);
    }
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
/*
    if(isSaved)
    {
        auto ix = FileSystemModelHelper::Index();
        auto fi = FileSystemModelHelper::fileInfo(ix);
        if(GitHelper::isGitRepo(fi))
        {
            auto pix = FileSystemModelHelper::parent(ix);
            auto fp = FileSystemModelHelper::filePath(pix);

if(!GitHelper::Commit(fp, m.txtfile.name) || !GitHelper::Push(fp))
{
    zInfo("giterr");
}
}
}
*/
/*
    if(isSaved)
    {
        auto ix = FileSystemModelHelper::Index();
        auto fi = FileSystemModelHelper::fileInfo(ix);
        if(GitHelper::isGitRepo(fi))
        {
            auto pix = FileSystemModelHelper::parent(ix);
            auto fp = FileSystemModelHelper::filePath(pix);

if(!GitHelper::Commit(fp, m.txtfile.name) || !GitHelper::Push(fp))
{
    zInfo("giterr");
}
}
}
*/

GitNote::AddNoteModelR GitNote::AddNote(AddNoteModel m){
    static QString FN = "Add";

    AddNoteModelR rm{false, QString(), m.fileindex};

    if(!m.fileindex.isValid()) return rm;
    //auto fi = FileSystemModelHelper::fileInfo(m.fileindex);

    auto pix = FileSystemModelHelper::parent(m.fileindex); // szülő mappa
    auto pfi = FileSystemModelHelper::fileInfo(pix);
    auto is_gitrepo = GitHelper::isGitRepo(pfi);
    auto fn = FileSystemModelHelper::fileName(m.fileindex);

    //auto fn = fi2.fileName();
    if(is_gitrepo)
    {
        QString e;
        auto fp = FileSystemModelHelper::filePath(pix);
        auto isok = GitHelper::Add(fp, fn);
        if(isok)
        {
            isok = GitHelper::Refresh(fp, FN, GitHelper::Type::Push);
//        auto isok_commit = GitHelper::Commit(fp, QString(), FN, &e);
//        auto isok_push = GitHelper::Push(fp, &e);

//        if(!isok_rm || !isok_commit || !isok_push)
//        {
//            zInfo("giterr");
//            return rm;
//        }
//        else
            if(isok)
            {
                auto fileInfo = FileSystemModelHelper::fileInfo(m.fileindex);
                rm.giturl=GitHelper::GetRepoURL(fileInfo);
            }
        }
    }

    rm.state=true;
    return rm;
}

// a model a fókuszált indexet tartalmazza
void GitNote::AddNewNote(AddNoteModel m){
    static QString FN = "Add";
    if(!m.fileindex.isValid()) return;

    auto dm = DisplayNewNoteDialog(m.w, GitNote::MSG_ADDNEWDIALOG.arg(GitNote::FILE));
    if(dm.filename.isEmpty()) return;

    QString newfile;
    auto fi = FileSystemModelHelper::fileInfo(m.fileindex);
    if(FileSystemModelHelper::isDir(m.fileindex)) // ha dirre mutat, akkor bele
    {
        newfile = QDir(FileSystemModelHelper::filePath(m.fileindex)).filePath(dm.filename);
    }
    else // ha fájlra, akkor mellé - a szülő lesz a születendő szüleje is
    {
        newfile = FileSystemModelHelper::fileInfo(m.fileindex).absoluteDir().filePath(dm.filename);
    }

    QFile f(newfile);
#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
    auto isok = f.open(QIODevice::NewOnly| QIODevice::Text);
#else
    auto isok = (!QFileInfo::exists(newfile)) && f.open(QIODevice::ReadWrite|QIODevice::Text);
#endif    

    if(isok)
    {
        f.close();
        zInfo(GitNote::MSG_ADDNEW.arg(GitNote::FILE).arg(newfile));
    }
    else{
        zInfo(GitNote::MSG_FAILEDTO.arg(GitNote::CREATE).arg(GitNote::FILE).arg(newfile));
    }

    auto pix = FileSystemModelHelper::parent(m.fileindex); // szülő mappa
    auto fi2 = FileSystemModelHelper::fileInfo(pix);
    auto is_gitrepo = GitHelper::isGitRepo(fi2);

    if(is_gitrepo){
        QString e;
        auto fp = FileSystemModelHelper::filePath(pix);
        auto isok_add = GitHelper::Add(fp, newfile);

        //auto isok_commit = GitHelper::Commit(fp, newfile, FN, &e);
        //auto isok_push = GitHelper::Push(fp, &e);

        //if(!isok_add || !isok_commit || !isok_push)
        if(isok_add){
            isok = GitHelper::Refresh(fp, FN, GitHelper::Type::Push);
        }
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
    static QString FN = "Remove";
    //auto ix = focusedIndex();
    if(!FileSystemModelHelper::isValid()) return;
    if(FileSystemModelHelper::Equals(m.fileindex)) return;

    //auto newname = FileSystemModelHelper::filePath(newix);
    auto pix = FileSystemModelHelper::parent(m.fileindex); // szülő mappa
    auto fi2 = FileSystemModelHelper::fileInfo(pix);
    auto is_gitrepo = GitHelper::isGitRepo(fi2);
    auto fn = FileSystemModelHelper::fileName(m.fileindex);

    //auto fn = fi2.fileName();
    if(is_gitrepo)
    {
        QString e;
        auto fp = FileSystemModelHelper::filePath(pix);
        auto isok_rm = GitHelper::Rm(fp, fn);

        if(isok_rm)
        {
            bool isok = GitHelper::Refresh(fp, FN, GitHelper::Type::Push);
        }
//        auto isok_commit = GitHelper::Commit(fp, QString(), FN, &e);
//        auto isok_push = GitHelper::Push(fp, &e);

//        if(!isok_rm || !isok_commit || !isok_push)
//        {
//            zInfo("giterr");
//        }
    }
    // TODO git delete

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




    /*
git rm file1.txt
git commit -m "remove file1.txt"
    */
}
