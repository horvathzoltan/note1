#ifndef LSETTINGS_H
#define LSETTINGS_H

#include <QString>
#include "common/helper/settings/isettings.h"
#include "filenamehelper.h"

struct Settings : public com::helper::ISettings
{
    QString projectPath;
   // QString gitUrl, gitUser, gitPasswd;
public:
    Settings();
//    void Save();
//    void Load();
    void parseIni(QMap<QString, QString>) override;
    QMap<QString, QString> toIni() override;
//    void init(const QString &a);
    bool isValid();
};

struct Environment
{
    QString user_at_host = FilenameHelper::user_at_host();
};

#endif // SETTINGS_H
