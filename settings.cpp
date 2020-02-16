#include "settings.h"
#include "filenamehelper.h"

Settings::Settings()
{
    this->projectPath=FilenameHelper::normalaizedir(R"(fejlesztes_notes)");// \notes1
    this->gitUrl = "giturl";
}
