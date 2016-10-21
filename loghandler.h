#ifndef LOGHANDLER_H
#define LOGHANDLER_H

#include "serverinfo.h"
#include <QUdpSocket>

class LogHandler: public QObject
{
    Q_OBJECT
public:
    LogHandler(MainWindow *);
    ~LogHandler();
    void createBind(quint16);
    bool setupUPnP();
    void removeServer(ServerInfo *);
    void addServer(ServerInfo *);
    QString szPort;
    QHostAddress externalIP;
private slots:
    void socketReadyRead();

private:

    QHostAddress internalIP;
    bool isBound;
    quint16 logPort;

    QUdpSocket *logsocket;
    QList<ServerInfo *> logList;
    MainWindow *pMain;
};

#endif // LOGHANDLER_H
