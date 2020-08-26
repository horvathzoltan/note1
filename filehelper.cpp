//#include <QModelIndex>
#include "./filehelper.h"
#include "filenamehelper.h"

QString FileHelper::TakeProjectDir(){
    auto projectDir = FilenameHelper::GetProjectAbsolutePath();
    if(!QDir(projectDir).exists()){
        QDir(QDir::homePath()).mkpath(projectDir);
    }

    return projectDir;
}
