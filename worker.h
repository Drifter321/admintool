#ifndef THREAD_H
#define THREAD_H

#include <QObject>
#include <QThread>
#include <QTableWidgetItem>
#include "rcon.h"

class RulesInfo;
class InfoReply;
class PlayerInfo;
class LogHandler;

class Worker : public QThread
{
    Q_OBJECT

public slots:
    void getServerInfo(QHostAddress *host, quint16 port, QTableWidgetItem *item);
    void getPlayerInfo(QHostAddress *host, quint16 port, QTableWidgetItem *item);
    void getRulesInfo(QHostAddress *host, quint16 port, QTableWidgetItem *item);
    void setupUPnP(LogHandler *);

signals:
    void serverInfoReady(InfoReply *reply, QTableWidgetItem *item);
    void playerInfoReady(QList<PlayerInfo> *, QTableWidgetItem *item);
    void rulesInfoReady(QList<RulesInfo> *, QTableWidgetItem *item);
    void UPnPReady();
};

#endif // THREAD_H
