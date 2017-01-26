#include "worker.h"
#include "query.h"
#include "serverinfo.h"
#include <QString>

void Worker::getServerInfo(QHostAddress *host, quint16 port, ServerTableIndexItem *item)
{
    InfoReply *reply = GetInfoReply(*host, port);
    emit serverInfoReady(reply, item);
    this->currentThread()->quit();
}

void Worker::getPlayerInfo(QHostAddress *host, quint16 port, ServerTableIndexItem *item)
{
    QList<PlayerInfo> *list = GetPlayerReply(*host, port);
    emit playerInfoReady(list, item);
    this->currentThread()->quit();
}

void Worker::getRulesInfo(QHostAddress *host, quint16 port, ServerTableIndexItem *item)
{
    QList<RulesInfo> *list = GetRulesReply(*host, port);
    emit rulesInfoReady(list, item);
    this->currentThread()->quit();
}
