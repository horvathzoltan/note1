#include "settings.h"
#include "filenamehelper.h"
#include "common/helper/ini/inihelper.h"
#include "common/helper/file/filehelper.h"
#include "common/macrofactory/macro.h"

Settings::Settings()
{
    this->projectPath=FilenameHelper::normalaizedir(R"(fejlesztes_notes)");// \notes1
    this->gitUrl = "giturl";
}

//void Settings::init(const QString &a){
//    com::helper::SettingsHelper::init(a, this);
//}

//void Settings::Save(){
//    com::helper::SettingsHelper::saveSettings();
//}

//void Settings::Load()
//{
//    com::helper::SettingsHelper::loadSettings();
//}

QMap<QString, QString> Settings::toIni(){
    QMap<QString, QString> m;
    m.insert(nameof(gitUrl), gitUrl);
    m.insert(nameof(projectPath), projectPath);
    return m;
}

void Settings::parseIni(QMap<QString, QString> m){
    gitUrl=m.value(nameof(gitUrl));
    projectPath = FilenameHelper::normalaizedir(m.value(nameof(projectPath)));
}
