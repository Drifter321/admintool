#include "rcon.h"
#include "serverinfo.h"
#include "worker.h"
#include "mainwindow.h"

RconQuery::RconQuery(MainWindow *main, ServerInfo *info)
{
    this->isAuthed = false;
    this->socket = new QTcpSocket();
    this->server = info;

    connect(this->socket, &QTcpSocket::destroyed, this, &RconQuery::deleteLater);
    connect(this->socket, &QTcpSocket::readyRead, this, &RconQuery::socketReadyRead);
    connect(this->socket, &QTcpSocket::disconnected, this, &RconQuery::socketDisconnected);
    connect(this, &RconQuery::rconAuth, main, &MainWindow::RconAuthReady);
    connect(this, &RconQuery::rconOutput, main, &MainWindow::RconOutput);
    connect(this, &RconQuery::rconHistory, main, &MainWindow::AddRconHistory);
}

void RconQuery::socketDisconnected()
{
    this->isAuthed = false;
    this->socket->readAll();
}

void RconQuery::socketReadyRead()
{
    static qint32 id = 0;
    static qint32 type = 0;
    static qint32 size = 0;

    QDataStream stream(this->socket);
    stream.setByteOrder(QDataStream::LittleEndian);

    do
    {
        if(size == 0)
        {
            stream >> size;
            stream >> id;
            stream >> type;
        }

        if(this->socket->bytesAvailable() < (qint64)(size-sizeof(qint32)*2))
        {
            return;
        }

        if(type == RCON_AUTH_RESPONSE)
        {
            if(id == -1)
            {
                this->isAuthed = false;
            }
            else if(id == rconID)
            {
                this->isAuthed = true;
            }
            stream.skipRawData(size-sizeof(qint32)*2);
            emit rconAuth(this->server);
        }
        else if(type == RCON_EXEC_RESPONSE && id == rconID)
        {
            char data[4096];
            stream.readRawData(data, size-sizeof(qint32)*2);
            emit rconOutput(this->server, data);
        }
        else //Something we dont handle skip the bytes
        {
            stream.skipRawData(size-sizeof(qint32)*2);
        }

        size = 0;

    }while(this->socket->bytesAvailable() > 4 && this->socket->state() == QAbstractSocket::ConnectedState);
}

void RconQuery::auth()
{
    if(this->socket->state() != QAbstractSocket::ConnectedState)
    {
        this->socket->connectToHost(this->server->host, this->server->port);
    }

    this->socket->readAll();

    if(this->server->rconPassword.length() == 0 || this->isAuthed)
        return;

    QByteArray query;
    QDataStream data(&query, QIODevice::ReadWrite);
    data.setByteOrder(QDataStream::LittleEndian);

    qint32 size = sizeof(qint32)*2 + this->server->rconPassword.length()+1 + sizeof(qint8);

    data << size;
    data << rconID;
    data << RCON_AUTH;
    data.writeRawData(this->server->rconPassword.toUtf8().data(), this->server->rconPassword.toUtf8().length()+1);
    data << qint8(0x00);

    this->socket->write(query);
}

void RconQuery::execCommand(QString command, bool history)
{
    if(!this->isAuthed)
    {
        this->auth();
        return;
    }

    if(command.length() == 0)
        return;

    if(history)
        emit rconHistory(command);

    QByteArray query;
    QDataStream data(&query, QIODevice::ReadWrite);
    data.setByteOrder(QDataStream::LittleEndian);

    qint32 size = sizeof(qint32)*2 + command.length()+1 + sizeof(qint8);

    data << size;
    data << rconID;
    data << RCON_EXEC_COMMAND;
    data.writeRawData(command.toUtf8().data(), command.toUtf8().length()+1);
    data << qint8(0x00);

    this->socket->write(query);
}
