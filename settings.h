#ifndef SETTINGS_H
#define SETTINGS_H

#include <QString>



struct Settings
{
    QString projectPath;
    QString gitUrl;
public:
    Settings();
};

#endif // SETTINGS_H
