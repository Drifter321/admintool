#ifndef THREAD_H
#define THREAD_H

#include <QObject>
#include <QThread>
#include "rcon.h"
#include "customitems.h"

class RulesInfo;
class InfoReply;
class PlayerInfo;
class LogHandler;

class Worker : public QThread
{
    Q_OBJECT

public slots:
    void getServerInfo(QHostAddress *host, quint16 port, ServerTableIndexItem *item);
    void getPlayerInfo(QHostAddress *host, quint16 port, ServerTableIndexItem *item);
    void getRulesInfo(QHostAddress *host, quint16 port, ServerTableIndexItem *item);
    void setupUPnP(LogHandler *);

signals:
    void serverInfoReady(InfoReply *reply, ServerTableIndexItem *item);
    void playerInfoReady(QList<PlayerInfo> *, ServerTableIndexItem *item);
    void rulesInfoReady(QList<RulesInfo> *, ServerTableIndexItem *item);
    void UPnPReady();
};

#endif // THREAD_H
