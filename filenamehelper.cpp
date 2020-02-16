#include "filenamehelper.h"


FilenameHelper::FilenameHelper()
{

}

QString FilenameHelper::normalaizedir(QString dir)
{
    auto a = dir.replace('\\', '/');
    return a;
}
