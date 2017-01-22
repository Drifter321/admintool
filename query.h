#ifndef INFOQUERY
#define INFOQUERY

#include "serverinfo.h"
#include <QUdpSocket>
#include <QThread>
#include <QTableWidgetItem>

#define QUERY_TIMEOUT 2000

#define A2S_HEADER  0xFFFFFFFF

#define A2S_INFO 0x54
#define A2S_INFO_CHECK 0x49
#define A2S_INFO_STRING "Source Engine Query"

#define A2S_PLAYER 0x55
#define A2S_PLAYER_CHALLENGE_CHECK 0x41
#define A2S_PLAYER_CHECK 0x44

#define A2S_RULES 0x56
#define A2S_RULES_CHALLENGE_CHECK 0x41
#define A2S_RULES_CHECK 0x45

class MainWindow;
class Worker;

class InfoReply
{
public:
    InfoReply(QByteArray, qint64);
    QString hostname;
    QString map;
    QString mod;
    QString gamedesc;
    qint32 appId;
    quint8 players;
    quint8 maxplayers;
    quint8 bots;
    char type;
    char os;
    bool visibility;
    bool vac;
    bool success;
    QString tags;
    QString version;
    qint8 protocol;
    qint64 ping;
    QString serverID;
};

class PlayerInfo
{
public:
    //~PlayerInfo(){delete [] name;}
    QString name;
    qint32 score;
    float time;
};

class RulesInfo
{
public:
    RulesInfo(QString n, QString v){this->name = n; this->value = v;}
   //~RulesInfo(){delete [] name; delete[] value;}
   QString name;
   QString value;
};

class InfoQuery : public QObject
{
    Q_OBJECT
    QThread workerThread;
public:
    InfoQuery(MainWindow *main);
    ~InfoQuery() {
        workerThread.quit();
        workerThread.wait();
    }
signals:
    void query(QHostAddress *, quint16, QTableWidgetItem *item);
};

class PlayerQuery : public QObject
{
    Q_OBJECT
public:
    QThread workerThread;
    Worker *worker;
    PlayerQuery(MainWindow *main);
    ~PlayerQuery();
private:
    MainWindow *pMain;

signals:
    void query(QHostAddress *, quint16, QTableWidgetItem *);
};

class RulesQuery : public QObject
{
    Q_OBJECT
public:
    QThread workerThread;
    Worker *worker;
    RulesQuery(MainWindow *main);
    ~RulesQuery();
private:
    MainWindow *pMain;

signals:
    void query(QHostAddress *, quint16, QTableWidgetItem *);
};

QList<PlayerInfo> *GetPlayerReply(QHostAddress, quint16);
InfoReply *GetInfoReply(QHostAddress, quint16);
QList<RulesInfo> *GetRulesReply(QHostAddress, quint16);
QString SecondsToDisplayTime(float time);
QString GetStringFromStream(QDataStream &stream);
#endif // INFOQUERY

