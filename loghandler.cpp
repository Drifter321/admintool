#include "loghandler.h"
#include "mainwindow.h"
#include "worker.h"
#include <QMessageBox>
#include <QNetworkReply>
#include <QNetworkInterface>

#define MINIUPNP_STATICLIB
#include <miniupnpc.h>
#include <upnpcommands.h>

#ifdef WIN32
#include <winsock2.h>
#endif

LogHandler::LogHandler(MainWindow *main)
{
    this->logPort = 0;
    this->manager = NULL;
    this->logsocket = NULL;
    this->pExternalIP = NULL;
    this->pInternalIP = NULL;
    this->pLocalAddress = NULL;
    this->createSocket();
    this->pMain = main;
    this->findLocalAddress();
}

LogHandler::~LogHandler()
{
    if(this->logsocket)
    {
        this->logsocket->close();
        delete this->logsocket;
    }

    if(this->pExternalIP)
        delete this->pExternalIP;
    if(this->pInternalIP)
        delete this->pInternalIP;
    if(this->pLocalAddress)
        delete this->pLocalAddress;

    if(this->manager)
        this->manager->deleteLater();

}

/* Checks if ip is within the following ranges
 *
 * 10.0.0.0 – 10.255.255.255
 * 172.16.0.0 – 172.31.255.255
 * 192.168.0.0 – 192.168.255.255
 *
*/
bool LogHandler::isPrivateIP(QHostAddress address)
{
    static QHostAddress arryPrivate[] = {QHostAddress("10.0.0.0"), QHostAddress("10.255.255.255"), QHostAddress("172.16.0.0"), QHostAddress("172.31.255.255"), QHostAddress("192.168.0.0"), QHostAddress("192.168.255.255")};

    quint32 iAddress = address.toIPv4Address();

    for(int i = 0; i < (sizeof(arryPrivate)/sizeof(arryPrivate[0])); i+=2)
    {
        if(iAddress >= arryPrivate[i].toIPv4Address() && iAddress <= arryPrivate[i+1].toIPv4Address())
            return true;
    }
    return false;
}

void LogHandler::findLocalAddress()
{
    if(!this->pLocalAddress)
        this->pLocalAddress = new QHostAddress();

    this->pLocalAddress->clear();

    foreach (const QHostAddress &address, QNetworkInterface::allAddresses())
    {
        if(address.protocol() == QAbstractSocket::IPv4Protocol && address != QHostAddress(QHostAddress::LocalHost) && this->isPrivateIP(address))
        {
            this->pLocalAddress->setAddress(address.toString());
            return;
        }
    }
}

void LogHandler::setExternalIP(QString szAddress)
{
    if(!this->pExternalIP)
        this->pExternalIP = new QHostAddress();

    this->pExternalIP->setAddress(szAddress);
}

void LogHandler::setInternalIP(QString szAddress)
{
    if(!this->pInternalIP)
        this->pInternalIP = new QHostAddress();

    this->pInternalIP->setAddress(szAddress);
}

void LogHandler::createSocket()
{
    if(!this->logsocket)
    {
        this->logsocket = new QUdpSocket();
        connect(this->logsocket, &QUdpSocket::readyRead, this, &LogHandler::socketReadyRead);
        connect(this->logsocket, &QUdpSocket::disconnected, this, &LogHandler::socketDisconnected);
        this->isBound = false;
    }
}

void LogHandler::socketDisconnected()
{
    this->isBound = false;
    this->createBind(this->logPort);
}

void LogHandler::socketReadyRead()
{
    QByteArray datagram(this->logsocket->pendingDatagramSize(), Qt::Uninitialized);

    QHostAddress sender;
    quint16 senderPort;

    this->logsocket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);

    ServerInfo *info = NULL;
    for(int i = 0; i < this->logList.size(); i++)
    {
        if(this->logList.at(i)->host == sender && this->logList.at(i)->port == senderPort)
        {
            info = this->logList.at(i);
            break;
        }
    }
    int idx = datagram.indexOf(" ");

    QString logLine = QString(datagram.remove(0, idx+1));

    pMain->parseLogLine(logLine, info);
}

void LogHandler::addServer(ServerInfo *info)
{
    for(int i = 0; i < this->logList.size(); i++)
    {
        if(this->logList.at(i)->isEqual(info))
        {
            return;
        }
    }
    this->logList.append(info);
}

void LogHandler::removeServer(ServerInfo *info)
{
    for(int i = 0; i < this->logList.size(); i++)
    {
        if(this->logList.at(i)->isEqual(info))
        {
            this->logList.removeAt(i);
            return;
        }
    }
}

void LogHandler::createBind(quint16 port)
{
    this->createSocket();
    if(this->isBound)
    {
       this->logsocket->close();
    }

    bool newPort = (this->logPort != port);

    this->logPort = port;
    this->szPort = QString::number(port);

    if(!this->logsocket->bind(QHostAddress::AnyIPv4, logPort))
    {
        QMessageBox::critical(pMain, "Log Handler Error", "Failed to bind to port:\n" + this->logsocket->errorString());
        return;
    }

    this->isBound = true;

    if(!workerThread.isRunning() && newPort)
    {
        Worker *worker = new Worker;
        worker->moveToThread(&workerThread);
        connect(&workerThread, &QThread::finished, worker, &Worker::deleteLater);
        connect(this, &LogHandler::setupUPnP, worker, &Worker::setupUPnP);
        connect(worker, &Worker::UPnPReady, this, &LogHandler::UPnPReady);

        workerThread.start();
        this->setupUPnP(this);
    }
}

void LogHandler::UPnPReady()
{
    if(this->pExternalIP && !this->pExternalIP->isNull())
    {
        if(this->manager)
        {
            this->manager->deleteLater();
            this->manager = NULL;
        }
        if(pMain->showLoggingInfo)
        {
            QMessageBox::information(this->pMain, "Log Handler", QString("Listening on: %1:%2").arg(this->pExternalIP->toString(), this->szPort));
            pMain->showLoggingInfo = false;
        }
    }
    else if(!this->manager)
    {
        this->manager = new QNetworkAccessManager(this);
        connect(this->manager, &QNetworkAccessManager::finished, this, &LogHandler::apiFinished);
        this->manager->get(QNetworkRequest(QUrl("http://api.ipify.org/")));
    }
    else
    {
        this->manager->deleteLater();
        this->manager = NULL;
        QMessageBox::critical(this->pMain, "Log Handler Error", QString("Failed to retrieve external IP."));
    }
}

void LogHandler::apiFinished(QNetworkReply *reply)
{
    if(!reply->error())
    {
        QString external = reply->readAll();
        this->setExternalIP(external);
    }
    reply->deleteLater();
    emit UPnPReady();
}

//TODO : Clean this up
void Worker::setupUPnP(LogHandler *logger)
{
    int nResult;

#ifdef WIN32
    WSADATA wsaData;
    nResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if(nResult != NO_ERROR)
    {
        WSACleanup();
        emit UPnPReady();
        this->currentThread()->quit();
        return;
    }
#endif

    int error;
    UPNPDev *devlist = upnpDiscover(2000, NULL, NULL, 0, 0, 2, &error);

    if(!devlist)
    {
#ifdef WIN32
        WSACleanup();
#endif
        emit UPnPReady();
        this->currentThread()->quit();
        return;
    }

    char lanaddress[64] = "";

    UPNPUrls urls;
    IGDdatas data;

    UPNP_GetValidIGD(devlist, &urls, &data, lanaddress, sizeof(lanaddress));

    logger->setInternalIP(QString(lanaddress));

    char externalIPAddress[64] = "";
    nResult = UPNP_GetExternalIPAddress(urls.controlURL, data.first.servicetype, externalIPAddress);

    if(nResult != UPNPCOMMAND_SUCCESS)
    {
        freeUPNPDevlist(devlist);
#ifdef WIN32
        WSACleanup();
#endif
        emit UPnPReady();
        this->currentThread()->quit();
        return;
    }

    logger->setExternalIP(QString(externalIPAddress));

    nResult = UPNP_AddPortMapping(urls.controlURL, data.first.servicetype,  logger->szPort.toLatin1().data(), logger->szPort.toLatin1().data(), lanaddress, "Source Admin Tool", "UDP", 0, "0");

    if(nResult != UPNPCOMMAND_SUCCESS)
    {
        freeUPNPDevlist(devlist);
#ifdef WIN32
        WSACleanup();
#endif
        emit UPnPReady();
        this->currentThread()->quit();
        return;
    }

    freeUPNPDevlist(devlist);

#ifdef WIN32
    WSACleanup();
#endif

    emit UPnPReady();
    this->currentThread()->quit();
}

// Returns internal ip (from either network interface look up or UPnP creation) or external depending on if server is LAN or not.
QHostAddress *LogHandler::getAddressToLogTo(QHostAddress serverAddress)
{
    if(this->isPrivateIP(serverAddress) && ((this->pInternalIP && !this->pInternalIP->isNull()) || (this->pLocalAddress && !this->pLocalAddress->isNull())))
    {
        if(this->pInternalIP && !this->pInternalIP->isNull())
            return this->pInternalIP;

        return this->pLocalAddress;
    }
    return this->pExternalIP;
}
