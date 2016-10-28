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
    bool isEqual(ServerInfo *)const;
    bool isEqual(ServerInfo) const;
    bool haveInfo;
    qint8 protocol;
    QString tags;
    quint8 vac;
    QString version;
    QString os;
    qint32 appId;
    QString serverName;
    QString currentMap;
    QString playerCount;
    QString gameName;
    QString type;
    QHostAddress host;
    quint16 port;
    QString ipPort;
    bool isValid;
    QString rconPassword;
    QStringList rconOutput;
    QStringList logOutput;
    QStringList chatOutput;
    bool saveRcon;
    RconQuery *rcon;
};

//Q_DECLARE_METATYPE(ServerInfo)

#endif // SERVERINFO
