#ifndef RCON_H
#define RCON_H

#include <QMainWindow>
#include <QTableWidgetItem>
#include <QTcpSocket>

#define RCON_AUTH 3
#define RCON_AUTH_RESPONSE 2
#define RCON_EXEC_COMMAND 2
#define RCON_EXEC_RESPONSE 0

enum QueuedCommandType
{
    GetLogCommand, //Runs command without history, adds a log listener.
    ConsoleCommand,//Runs commands with history
    ContextCommand //Runs commands without history
};

class QueuedCommand
{
public:
    QueuedCommand(QString cmd, QueuedCommandType cmdType)
    {
        this->command = cmd;
        this->commandType = cmdType;
    }
    QueuedCommand(){this->command = "", this->commandType = ContextCommand;}
    QString command;
    QueuedCommandType commandType;
};

class ServerInfo;
class MainWindow;

const qint32 rconID = 0x7001;

class RconQuery : public QObject
{
    Q_OBJECT
public:
    RconQuery(MainWindow *main, ServerInfo *info);
    ~RconQuery() {
        if(socket)
            socket->deleteLater();
    }
    bool isAuthed;
    QList<QueuedCommand> queuedList;
    void auth();
    void execCommand(QString command, bool history = true);

signals:
    void rconOutput(ServerInfo *, QByteArray);
    void rconAuth(ServerInfo *,  QList<QueuedCommand>);
    void rconHistory(QString);
private slots:
    void socketReadyRead();
    void socketDisconnected();

private:
    QTcpSocket *socket;
    ServerInfo *server;
    qint32 responseSize;
};

#endif // RCON_H
