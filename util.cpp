#include "util.h"
#include <QCoreApplication>
#include <QString>

char *BuildPath(const char *file)
{
#ifdef WIN32
    return (QString("./%1").arg(file)).toUtf8().data();
#elif __APPLE__
    return (QString("%1/%2").arg(QCoreApplication::applicationDirPath(), file)).toUtf8().data();
#endif
}
