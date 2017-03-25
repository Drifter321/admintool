#include "query.h"
#include "worker.h"
#include "mainwindow.h"
#include <QDateTime>

const qint32 k_nAppIDTheShip = 2400;
const qint32 k_nAppIDKillingFloor = 1250;
const qint32 k_nAppIDKillingFloor2 = 232090;

QString GetStringFromStream(QDataStream &stream)
{
    qint64 pos = stream.device()->pos();
    qint64 bytes = stream.device()->bytesAvailable();

    do
    {
        qint8 byte;
        stream >> byte;

        if(byte == 0x00)
        {
            qint64 size = bytes - stream.device()->bytesAvailable();

            char *ret = new char[size];

            stream.device()->reset();
            stream.skipRawData(pos);

            stream.readRawData(ret, size);

            QString res = QString(ret);
            delete [] ret;
            return res;
        }

    }while(stream.device()->bytesAvailable() > 0);

    stream.device()->reset();
    stream.skipRawData(pos);

    return NULL;
}

QString GetRichUEStringFromStream(QDataStream &stream)
{
    qint64 pos = stream.device()->pos();
    qint64 bytes = stream.device()->bytesAvailable();

    do
    {
        qint8 byte;
        stream >> byte;

        if(byte == 0x00)
        {
            qint64 size = bytes - stream.device()->bytesAvailable();

            char *ret = new char[size];

            stream.device()->reset();
            stream.skipRawData(pos);

            stream.readRawData(ret, size);

            std::string res;
            bool inColor = false;
            for (size_t i = 0; i < size; ++i)
            {
                if (ret[i] == 0x1B && ((i + 3) < size))
                {
                    if (inColor)
                    {
                        res += "</font>";
                    }

                    unsigned char r = ret[i+1];
                    unsigned char g = ret[i+2];
                    unsigned char b = ret[i+3];
                    res += QString("<font color=\"%1\">").arg(
                        QColor::fromRgb(r, g, b).name()
                        ).toStdString();

                    i += 3;
                }
                else
                {
                    res += ret[i];
                }
            }

            if (inColor)
            {
                res += "</font>";
            }

            delete [] ret;

            return QString::fromLatin1(res.c_str());
        }

    }while(stream.device()->bytesAvailable() > 0);

    stream.device()->reset();
    stream.skipRawData(pos);

    return NULL;
}

QByteArray SendUDPQuery(QByteArray query, QHostAddress host, quint16 port)
{
    qint8 tryCount = 0;

    //Use a while loop for rust and other servers that use gamserverport+1 for query
    do
    {
        QUdpSocket socket;
        socket.connectToHost(host, port+tryCount);

        if(socket.isValid())
        {
            if(socket.write(query) != -1 && socket.waitForReadyRead(QUERY_TIMEOUT))
            {
                QByteArray reply;
                reply.resize(socket.pendingDatagramSize());
                socket.readDatagram(reply.data(), reply.size());

                QDataStream response(reply);
                response.setByteOrder(QDataStream::LittleEndian);

                qint32 header;
                response >> header;

                if(header == -1)
                {
                    socket.close();
                    return reply;
                }
                else if(header == -2)
                {
                    //We have multiple split packets
                    qint32 id;
                    qint8 skip = 0;
                    response >> id;

                    bool compressed;
                    response.device()->seek(response.device()->pos()-1);

                    response >> compressed;

                    // TODO:Support compressed packs
                    if(!compressed)
                    {
                        qint8 total;
                        response >> total;
                        qint8 packetNum;
                        response >> packetNum;

                        response >> header;

                        if(header != -1)
                        {
                            response.device()->seek(response.device()->pos()-sizeof(qint32)+sizeof(qint16));
                            skip = sizeof(qint16);
                            response >> header;
                        }

                        if(header == -1)
                        {
                            reply.clear();

                            response.device()->seek(response.device()->pos()-sizeof(qint32));

                            reply.append(response.device()->read(response.device()->size()));

                            do
                            {
                                if(socket.isValid())
                                {
                                    if(socket.write(query) != -1 && socket.waitForReadyRead(QUERY_TIMEOUT))
                                    {
                                        QByteArray replyTemp;
                                        replyTemp.resize(socket.pendingDatagramSize());
                                        socket.readDatagram(replyTemp.data(), replyTemp.size());

                                        QDataStream tempStream(replyTemp);
                                        tempStream.setByteOrder(QDataStream::LittleEndian);

                                        tempStream >> header;

                                        qint32 tempId;
                                        tempStream >> tempId;

                                        if(header != -2 || id != tempId)
                                            break;

                                        tempStream.skipRawData(sizeof(qint8));

                                        tempStream >> packetNum;
                                        tempStream.skipRawData(skip);

                                        reply.append(tempStream.device()->read(tempStream.device()->size()));
                                    }
                                }

                            }while(packetNum != total-1);

                            socket.close();
                            return reply;
                        }
                    }
                }
            }
            socket.close();
        }

        tryCount ++;

    }while(tryCount != 2);

    return QByteArray();
}

InfoReply::InfoReply(QByteArray response, qint64 ping)
{
    this->success = false;
    this->appId = -1;
    this->rawServerId = 0;
    this->ping = ping;

    if(response.size() > 5)
    {
        QDataStream data(response);

        data.setFloatingPointPrecision(QDataStream::SinglePrecision);
        data.setByteOrder(QDataStream::LittleEndian);

        qint32 header;
        qint8 check;

        data >> header;
        data >> check;

        if(header == -1 && check == A2S_INFO_CHECK)
        {
            this->success = true;
            data >> this->protocol;
            qint64 hostnamePos = data.device()->pos();
            this->hostnameRich = GetStringFromStream(data);
            this->map = GetStringFromStream(data);
            this->mod = GetStringFromStream(data);
            this->gamedesc = GetStringFromStream(data);
            qint16 id;
            data >> id;
            this->appId = id;
            data >> this->players;
            data >> this->maxplayers;
            data >> this->bots;
            data.device()->getChar(&(this->type));
            data.device()->getChar(&(this->os));
            data >> this->visibility;
            data >> this->vac;

            //The ship stuff...
            if(this->appId == k_nAppIDTheShip)
            {
                data.skipRawData(sizeof(qint8)*3);
            }

            this->version = GetStringFromStream(data);//Version

            qint8 edf;
            data >> edf;

            if(edf & 0x80)
            {
                data.skipRawData(sizeof(qint16));
            }
            if(edf & 0x10)
            {
                data >> this->rawServerId;

                quint32 accountID = (this->rawServerId & 0xFFFFFFFF);
                quint64 accountInst = (this->rawServerId >> 32) & 0xFFFFF;
                quint64 accountType = (this->rawServerId >> 52) & 0xF;
                quint8 accountUni = (this->rawServerId >> 56) & 0xFF;

                if(accountType == 4 || accountType == 3)
                {
                    if(accountType == 4)
                    {
                        this->serverID = QString("[A:%1:%2:%3]").arg(QString::number(accountUni), QString::number(accountID), QString::number(accountInst));
                    }
                    else
                    {
                        this->serverID = QString("[G:%1:%2]").arg(QString::number(accountUni), QString::number(accountID));
                    }
                }

            }
            if(edf & 0x40)
            {
                data.skipRawData(sizeof(qint16));
                GetStringFromStream(data);
            }
            if(edf & 0x20)
            {
                this->tags = GetStringFromStream(data);
            }
            if(edf & 0x01)
            {
                qint64 temp;
                data >> temp;

                this->appId = temp & ((1 << 24) - 1);
            }

            // TODO: move these out to a config and add more games.
            // Unreal Engine uses Extended ASCII instead of UTF8, and also supports colors in hostname.
            if (this->appId == k_nAppIDKillingFloor || this->appId == k_nAppIDKillingFloor2)
            {
                data.device()->seek(hostnamePos);
                this->hostnameRich = GetRichUEStringFromStream(data);
            }
        }
    }
}

InfoQuery::InfoQuery(MainWindow *main)
{
    Worker *worker = new Worker;
    worker->moveToThread(&workerThread);
    connect(&workerThread, &QThread::finished, worker, &Worker::deleteLater);
    connect(&workerThread, &QThread::finished, this, &InfoQuery::deleteLater);
    connect(this, &InfoQuery::query, worker, &Worker::getServerInfo);
    connect(worker, &Worker::serverInfoReady, main, &MainWindow::ServerInfoReady);

    workerThread.start();
}

InfoReply *GetInfoReply(QHostAddress host, quint16 port)
{
    QByteArray query;
    QDataStream data(&query, QIODevice::ReadWrite);

    data << A2S_HEADER << (qint8)A2S_INFO;
    data.writeRawData(A2S_INFO_STRING, sizeof(A2S_INFO_STRING));

    qint64 ping = QDateTime::currentMSecsSinceEpoch();
    QByteArray response = SendUDPQuery(query, host, port);
    ping = QDateTime::currentMSecsSinceEpoch()-ping;
    if(ping > 2000)
    {
        ping = ping - 2000;
    }
    if(ping > 2000)
    {
        ping = 2000;
    }

    return new InfoReply(response, ping);
}

PlayerQuery::PlayerQuery(MainWindow *main)
{
    pMain = main;
    this->worker = new Worker;
    this->worker->moveToThread(&workerThread);
    connect(&workerThread, &QThread::finished, this, &PlayerQuery::deleteLater);
    connect(&workerThread, &QThread::finished, this->worker, &Worker::deleteLater);
    connect(this, &PlayerQuery::query, this->worker, &Worker::getPlayerInfo);
    connect(this->worker, &Worker::playerInfoReady, main, &MainWindow::PlayerInfoReady);

    workerThread.start();
}
PlayerQuery::~PlayerQuery()
{
    workerThread.quit();
    workerThread.wait();

    if(this == pMain->pPlayerQuery)
        pMain->pPlayerQuery = NULL;
}

QList<PlayerInfo> *GetPlayerReply(QHostAddress host, quint16 port)
{
    QList<PlayerInfo> *list = new QList<PlayerInfo>();

    QByteArray query;
    QDataStream data(&query, QIODevice::ReadWrite);
    data.setByteOrder(QDataStream::LittleEndian);
    data << A2S_HEADER << (qint8)A2S_PLAYER << qint32(-1);

    QByteArray byteResponse = SendUDPQuery(query, host, port);
    QDataStream response(byteResponse);
    response.setByteOrder(QDataStream::LittleEndian);

    qint32 header;
    qint8 byteCheck;

    response >> header;
    response >> byteCheck;

    if(header == -1 && (byteCheck == A2S_PLAYER_CHALLENGE_CHECK || byteCheck == A2S_PLAYER_CHECK))
    {
        if(byteCheck == A2S_PLAYER_CHALLENGE_CHECK)
        {
            qint32 challenge;
            response >> challenge;

            data.device()->reset();
            data << A2S_HEADER << (qint8)A2S_PLAYER << challenge;

            byteResponse = SendUDPQuery(query, host, port);
        }
        QDataStream playerResponse(byteResponse);

        playerResponse.setFloatingPointPrecision(QDataStream::SinglePrecision);
        playerResponse.setByteOrder(QDataStream::LittleEndian);

        playerResponse >> header;

        if(header == -1)
        {
            playerResponse >> byteCheck;
            if(byteCheck == A2S_PLAYER_CHECK)
            {
                quint8 count;
                playerResponse >> count;

                for(int i = 0; i < count; i++)
                {
                    PlayerInfo playerInfo;
                    playerResponse >> byteCheck;
                    playerInfo.name = GetStringFromStream(playerResponse);
                    playerResponse >> playerInfo.score;
                    playerResponse >> playerInfo.time;
                    list->append(playerInfo);
                }
            }
        }
    }
    return list;
}

RulesQuery::RulesQuery(MainWindow *main)
{
    pMain = main;
    this->worker = new Worker;
    this->worker->moveToThread(&workerThread);
    connect(&workerThread, &QThread::finished, this, &RulesQuery::deleteLater);
    connect(&workerThread, &QThread::finished, this->worker, &Worker::deleteLater);
    connect(this, &RulesQuery::query, this->worker, &Worker::getRulesInfo);
    connect(this->worker, &Worker::rulesInfoReady, main, &MainWindow::RulesInfoReady);

    workerThread.start();
}
RulesQuery::~RulesQuery()
{
    workerThread.quit();
    workerThread.wait();

    if(this == pMain->pRulesQuery)
        pMain->pRulesQuery = NULL;
}

QList<RulesInfo> *GetRulesReply(QHostAddress host, quint16 port)
{
    QList<RulesInfo> *list = new QList<RulesInfo>();

    QByteArray query;
    QDataStream data(&query, QIODevice::ReadWrite);
    data.setByteOrder(QDataStream::LittleEndian);
    data << A2S_HEADER << (qint8)A2S_RULES << qint32(-1);

    QByteArray byteResponse = SendUDPQuery(query, host, port);
    QDataStream response(byteResponse);
    response.setByteOrder(QDataStream::LittleEndian);

    qint32 header;
    qint8 byteCheck;

    response >> header;
    response >> byteCheck;

    if(header == -1 && byteCheck == A2S_RULES_CHALLENGE_CHECK)
    {
        qint32 challenge;
        response >> challenge;

        data.device()->reset();
        data << A2S_HEADER << (qint8)A2S_RULES << challenge;

        byteResponse = SendUDPQuery(query, host, port);
    }

    QDataStream rulesResponse(byteResponse);
    rulesResponse.setByteOrder(QDataStream::LittleEndian);

    rulesResponse >> header;
    rulesResponse >> byteCheck;

    if(header == -1 && byteCheck == A2S_RULES_CHECK)
    {
        quint16 count;
        rulesResponse >> count;

        QString name;
        QString value;
        for(int i = 0; i < count; i++)
        {
            name = GetStringFromStream(rulesResponse);
            value = GetStringFromStream(rulesResponse);
            list->append(RulesInfo(name, value));
            name = "";
            value = "";
        }
    }
    return list;
}
