#include "worker.h"
#include "query.h"
#include "serverinfo.h"
#include <QString>

void Worker::getServerInfo(ServerInfo *info, QTableWidgetItem *item)
{
    InfoReply *reply = GetInfoReply(info);
    emit serverInfoReady(info, reply, item);
    this->currentThread()->quit();
}

void Worker::getPlayerInfo(ServerInfo *info, QTableWidgetItem *item)
{
    QList<PlayerInfo> *list = GetPlayerReply(info);
    emit playerInfoReady(list, item);
    this->currentThread()->quit();
}

void Worker::getRulesInfo(ServerInfo *info, QTableWidgetItem *item)
{
    QList<RulesInfo> *list = GetRulesReply(info);
    emit rulesInfoReady(list, item);
    this->currentThread()->quit();
}
