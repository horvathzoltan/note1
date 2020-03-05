#include "settings.h"
#include "filenamehelper.h"
#include "common/helper/ini/inihelper.h"
#include "common/helper/file/filehelper.h"
#include "common/macrofactory/macro.h"

Settings::Settings()
{
    this->projectPath=FilenameHelper::normalaizedir(R"(fejlesztes_notes)");// \notes1
//    this->gitUrl = "https://github.com/note55/note1.git";
//    this->gitUser = "horvath.zoltan.mobil3@gmail.com";
//    this->gitPasswd = "asERxc5678asERxc56";
}

QMap<QString, QString> Settings::toIni(){
    QMap<QString, QString> m;
    //m.insert(nameof(gitUrl), gitUrl);
    m.insert(nameof(projectPath), projectPath);
    return m;
}

bool Settings::isValid()
{
    if(this->projectPath.isEmpty()) return false;
    return true;
}

void Settings::parseIni(QMap<QString, QString> m){
    //gitUrl=m.value(nameof(gitUrl));
    projectPath = FilenameHelper::normalaizedir(m.value(nameof(projectPath)));
}
