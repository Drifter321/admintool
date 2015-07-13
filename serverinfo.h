#ifndef SERVERINFO
#define SERVERINFO

#include <QMainWindow>
#include <QHostAddress>
#include "rcon.h"

struct ServerInfo
{
public:
    ~ServerInfo()
    {
        if(this->rcon != NULL)
            delete this->rcon;
    }

    ServerInfo(QString);
    bool isEqual(ServerInfo) const;
    qint8 protocol;
    qint32 appId;
    QHostAddress host;
    int port;
    QString ipPort;
    bool isValid;
    QString rconPassword;
    QString rconOutput;
    bool saveRcon;
    RconQuery *rcon;
};

//Q_DECLARE_METATYPE(ServerInfo)

#endif // SERVERINFO

