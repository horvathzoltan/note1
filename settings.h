#ifndef SETTINGS_H
#define SETTINGS_H

#include <QString>
#include "common/helper/settings/isettings.h"


struct Settings : public com::helper::ISettings
{
    QString projectPath;
    QString gitUrl;
public:
    Settings();
//    void Save();
//    void Load();
    void parseIni(QMap<QString, QString>) override;
    QMap<QString, QString> toIni() override;
//    void init(const QString &a);
};

#endif // SETTINGS_H
