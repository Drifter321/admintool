#include "serverinfo.h"
#include "query.h"
#include "util.h"
#include <QFile>

#include <maxminddb.h>

ServerInfo::ServerInfo(QString server)
{
    this->isValid = false;
    this->appId = -1;
    this->rconPassword = "";
    this->saveRcon = false;
    this->rcon = NULL;
    this->vac = 0;
    this->version = "";
    this->os = "";
    this->tags = "";
    this->haveInfo = false;

    QStringList address = server.split(":");
    bool ok;

    if(address.size() != 2)
        return;

    QString ip = address.at(0);
    this->port = address.at(1).toInt(&ok);

    if(this->host.setAddress(address.at(0)) && port && ok)
    {
        this->ipPort = server;
        this->isValid = true;
    }

    MMDB_s mmdb;
    int status = MMDB_open(BuildPath("GeoLite2-Country.mmdb"), MMDB_MODE_MMAP, &mmdb);
    if (status == MMDB_SUCCESS)
    {
        int gai_error, mmdb_error;
        MMDB_lookup_result_s results = MMDB_lookup_string(&mmdb, ip.toLatin1().data(), &gai_error, &mmdb_error);
        if (gai_error == 0 && mmdb_error == MMDB_SUCCESS && results.found_entry)
        {
            MMDB_entry_data_s entry_data;
            int res = MMDB_get_value(&results.entry, &entry_data, "country", "iso_code", NULL);
            if (res == MMDB_SUCCESS && entry_data.has_data && entry_data.type == MMDB_DATA_TYPE_UTF8_STRING)
            {
                QString countryName = QString(QByteArray::fromRawData(entry_data.utf8_string, entry_data.data_size)).toLower();
                QString flagPath = QString(":/icons/icons/countries/%1.png").arg(countryName);
                if (QFile::exists(flagPath))
                {
                    countryFlag.load(flagPath);
                }
                else
                {
                    qDebug() << "Flag icon does not exist at " << flagPath << ".";
                }
            }
            else
            {
                qDebug() << "Bad entry. MMDBerror " << MMDB_strerror(res) << ", HasData: " << entry_data.has_data << ", DataType: " << entry_data.utf8_string;
            }
        }
        else
        {
            qDebug() << "Lookup failure. gai: " << gai_error << ", MMDBerror " << MMDB_strerror(mmdb_error) << " (" << mmdb_error << ")";
        }

        MMDB_close(&mmdb);
    }
    else
    {
        qDebug() << "Failed to open MaxMind db (" << MMDB_strerror(status) << ")";
    }
}

bool ServerInfo::isEqual(ServerInfo other) const
{
    return (this->host == other.host && this->port == other.port);
}

bool ServerInfo::isEqual(ServerInfo *other) const
{
    return (this->host == other->host && this->port == other->port);
}

void ServerInfo::cleanHashTable()
{
    QList<QString> keys = this->logHashTable.keys();
    for(int i = 0; i < keys.length(); i++)
    {
        PlayerLogInfo info = this->logHashTable.value(keys.at(i));
        if(info.time+(1000*60*30) < QDateTime::currentMSecsSinceEpoch())//remove the key if its older than 30 minutes
        {
            this->logHashTable.remove(keys.at(i));
        }
    }
}
