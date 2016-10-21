#include "loghandler.h"
#include "mainwindow.h"
#include <QMessageBox>

#define MINIUPNP_STATICLIB
#include <miniupnpc.h>
#include <upnpcommands.h>

#ifdef WIN32
#include <winsock2.h>
#endif

LogHandler::LogHandler(MainWindow *main)
{
    isBound = false;
    this->logPort = 0;
    this->logsocket = new QUdpSocket();
    QObject::connect(this->logsocket, &QUdpSocket::readyRead, this, &LogHandler::socketReadyRead);
    this->pMain = main;
}

LogHandler::~LogHandler()
{
   delete this->logsocket;
}

void LogHandler::socketReadyRead()
{
    QByteArray datagram;
    datagram.resize(this->logsocket->pendingDatagramSize());

    QHostAddress sender;
    quint16 senderPort;

    this->logsocket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);

    ServerInfo *info = NULL;
    for(int i = 0; i < this->logList.size(); i++)
    {
        if((this->logList.at(i)->host == sender || (this->logList.at(i)->host == this->internalIP && this->externalIP == sender)) && this->logList.at(i)->port == senderPort)
        {
            info = this->logList.at(i);
            break;
        }
    }
    int idx = QString(datagram).indexOf(" ");
    pMain->parseLogLine(QString(datagram).remove(0, idx+1), info);
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
    if(!this->logsocket)
    {
        this->logsocket = new QUdpSocket();
        connect(this->logsocket, &QUdpSocket::readyRead, this, &LogHandler::socketReadyRead);
        this->isBound = false;
    }

    if(this->isBound)
    {
       this->logsocket->close();
    }

    this->logPort = port;
    this->szPort = QString::number(port);

    if(!this->logsocket->bind(QHostAddress::AnyIPv4, logPort))
    {
        QMessageBox::critical(pMain, "Log Handler Error", "Failed to bind to port");
        return;
    }

    this->isBound = true;

    if(!this->setupUPnP())
    {
        QMessageBox::critical(pMain, "Log Handler Error", "Failed port mapping");
        return;
    }
}

#ifdef WIN32
bool LogHandler::setupUPnP()
{
    WSADATA wsaData;
    int nResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if(nResult != NO_ERROR)
        return false;

    int error;
    UPNPDev *devlist = upnpDiscover(2000, NULL, NULL, 0, 0, 2, &error);

    if(!devlist)
    {
        return false;
    }

    char lanaddress[64] = "";

    UPNPUrls urls;
    IGDdatas data;

    nResult = UPNP_GetValidIGD(devlist, &urls, &data, lanaddress, sizeof(lanaddress));

    this->internalIP = QHostAddress(QString(lanaddress));

    char externalIPAddress[64] = "";
    nResult = UPNP_GetExternalIPAddress(urls.controlURL, data.first.servicetype, externalIPAddress);

    if(nResult != UPNPCOMMAND_SUCCESS)
    {
       return false;
    }

    this->externalIP = QHostAddress(QString(externalIPAddress));

    UPNP_AddPortMapping(urls.controlURL, data.first.servicetype,  this->szPort.toLatin1().data(), this->szPort.toLatin1().data(), lanaddress, "Source Admin Tool", "UDP", 0, "0");
    freeUPNPDevlist(devlist);
    WSACleanup();

    return true;
}
#endif
