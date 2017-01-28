#ifndef SERVERINFO
#define SERVERINFO

#include <QMainWindow>
#include <QHostAddress>
#include <QHash>
#include <QDateTime>
#include "rcon.h"

enum QueryState
{
    QueryRunning,
    QueryFailed,
    QuerySuccess,
};

class PlayerLogInfo
{
public:
    PlayerLogInfo()
    {
        time = QDateTime::currentMSecsSinceEpoch();
        steamID = "";
        userid = 0;
    }

    PlayerLogInfo(quint32 id, QString steam)
    {
        time = QDateTime::currentMSecsSinceEpoch();
        steamID = steam;
        userid = id;
    }

    qint64 time;
    QString steamID;
    quint32 userid;
};



class ServerInfo
{
public:
    ~ServerInfo()
    {
        if(this->rcon != NULL)
            delete this->rcon;
    }

    ServerInfo(QString);
    bool isEqual(ServerInfo *)const;
    void cleanHashTable();
public:
    bool haveInfo;
    qint8 protocol;
    QString tags;
    quint8 vac;
    QString version;
    QString os;
    qint32 appId;
    QString serverName;
    QString currentMap;
    QString nextMap;
    QString ff;
    QString timelimit;
    QStringList mods;
    QString playerCount;
    QString gameName;
    QString type;
    QString serverID;
    QHostAddress host;
    quint16 port;
    quint64 rawServerId;
    QString ipPort;
    QString rconPassword;
    QStringList rconOutput;
    QStringList logOutput;
    QStringList chatOutput;
    bool saveRcon;
    RconQuery *rcon;
    QHash<QString, PlayerLogInfo> logHashTable;
    QList<int> pingList;
    quint16 lastPing;
    quint16 avgPing;
    QImage countryFlag;
    QueryState queryState;
    quint8 currentPlayers;
    quint8 maxPlayers;
};

//Q_DECLARE_METATYPE(ServerInfo)

#endif // SERVERINFO
