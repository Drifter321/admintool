#include "ui_mainwindow.h"
#include "mainwindow.h"
#include "customitems.h"
#include "worker.h"
#include "query.h"
#include "serverinfo.h"
#include "settings.h"

#include <QPainter>

extern QMap<int, QString> appIDMap;
extern QList<ServerInfo *> serverList;

QColor errorColor(255, 60, 60);
QColor queryingColor(80, 170, 80);

#define UPDATE_TIME 15

//TIMER TRIGGERED UPDATING
void MainWindow::TimedUpdate()
{
    static int run = 1;

    if(run % UPDATE_TIME == 0)
    {
        for(int i = 0; i < this->ui->browserTable->rowCount(); i++)
        {
            ServerTableIndexItem *id = this->GetServerTableIndexItem(i);
            ServerInfo *info = id->GetServerInfo();

            InfoQuery *infoQuery = new InfoQuery(this);

            info->cleanHashTable();

            infoQuery->query(&info->host, info->port, id);
        }
        if(run % 60 == 0)
        {
            this->UpdateSelectedItemInfo(false, true);
            run = 1;
        }
        else
        {
            this->UpdateSelectedItemInfo(false, false);
            run++;
        }
    }
    else
    {
        for(int i = 0; i < this->ui->playerTable->rowCount(); i++)
        {
            PlayerTimeTableItem *item = (PlayerTimeTableItem *)this->ui->playerTable->item(i, 3);

            item->updateTime(item->getTime()+1.0);
        }
        run++;
    }
}

void MainWindow::UpdateSelectedItemInfo(bool removeFirst, bool updateRules)
{
    if(this->ui->browserTable->selectedItems().size() == 0)
        return;

    ServerTableIndexItem *item = this->GetServerTableIndexItem(this->ui->browserTable->currentRow());
    ServerInfo *info = item->GetServerInfo();

    if(removeFirst)
    {
        while(this->ui->playerTable->rowCount() > 0)
        {
            this->ui->playerTable->removeRow(0);
        }
        while(this->ui->infoTable->rowCount() > 0)
        {
            this->ui->infoTable->removeRow(0);
        }
        if(updateRules)
        {
            while(this->ui->rulesTable->rowCount() > 0)
            {
                this->ui->rulesTable->removeRow(0);
            }
        }
    }

    pPlayerQuery = new PlayerQuery(this);
    pPlayerQuery->query(&info->host, info->port, item);

    if(updateRules)
    {
        pRulesQuery = new RulesQuery(this);
        pRulesQuery->query(&info->host, info->port, item);
    }
    this->UpdateInfoTable(info);
}

//Update
void MainWindow::CreateTableItemOrUpdate(size_t row, size_t col, QTableWidget *table, ServerInfo *info)
{
    if(table == this->ui->browserTable)
    {
         bool bAddItem = (table->item(row, col) == nullptr);
        //Handle player stuff differently
        if(col == kBrowserColPlayerCount)
        {
            PlayerTableItem *playerItem = bAddItem ? new PlayerTableItem() : (PlayerTableItem *)table->item(row, col);
            playerItem->players = info->currentPlayers;

            if(info->currentPlayers == info->maxPlayers)
                playerItem->setTextColor(errorColor);
            else
                playerItem->setTextColor(queryingColor);

            playerItem->setText(info->playerCount);

            if(bAddItem)
            {
                table->setItem(row, col, playerItem);
            }

            return;
        }

        QTableWidgetItem *item = bAddItem ? new QTableWidgetItem() : table->item(row, col);

        if(bAddItem)
        {
            table->setItem(row, col, item);
        }

        switch(col)
        {
            case kBrowserColModIcon:
            {
                QImage icon;
                icon.load(appIDMap.value(info->appId, ":/icons/icons/hl2.gif"));
                item->setData(Qt::DecorationRole, icon);
                break;
            }
            case kBrowserColVACIcon:
                if(bAddItem)
                {
                    item->setData(Qt::DecorationRole, this->GetVACImage());
                }
                break;
            case kBrowserColLockIcon:
                if(bAddItem)
                {
                    item->setData(Qt::DecorationRole, this->GetLockImage());
                }
                break;
            case kBrowserColFlagIcon:
                if(bAddItem)
                {
                    item->setData(Qt::DecorationRole, info->countryFlag);
                }
                break;
            case kBrowserColHostname:
                if(info->queryState == QuerySuccess)
                {
                    item->setTextColor(this->GetTextColor());
                    item->setText(info->serverName);
                }
                else if(info->queryState == QueryFailed)
                {
                    item->setTextColor(errorColor);
                    item->setText(QString("Failed to query %1, retrying in %2 seconds").arg(info->ipPort, QString::number(UPDATE_TIME)));
                }
                else if(info->queryState == QueryRunning)
                {
                    item->setTextColor(queryingColor);
                    item->setText(QString("Querying server %1...").arg(info->ipPort));
                }
                if(bAddItem)
                {
                    item->setToolTip(info->ipPort);
                }
                break;
            case kBrowserColMap:
                item->setText(info->currentMap);
                item->setTextColor(errorColor);
                break;
            case kBrowserColPing:
            {
                //If timedout show 2000 else show the actual avg.
                quint16 avgPing = (info->lastPing == 2000) ? 2000 : info->avgPing;

                item->setText(QString("%1, Ø%2").arg(QString::number(info->lastPing), QString::number(avgPing)));
                if(info->lastPing > 200)
                    item->setTextColor(errorColor);
                else
                    item->setTextColor(queryingColor);
                break;
            }
        }
    }
}

//Cleanup?
void MainWindow::UpdateInfoTable(ServerInfo *info, bool current, QList<RulesInfo> *list)
{
    QString mapString;
    QString gameString;

    if(list)
    {
        info->nextMap.clear();
        info->ff.clear();
        info->timelimit.clear();
        info->mods.clear();

        for(int i = 0; i < list->size(); i++)
        {
            this->ui->rulesTable->insertRow(i);
            this->ui->rulesTable->setItem(i, 0, new QTableWidgetItem(list->at(i).name));
            this->ui->rulesTable->setItem(i, 1, new QTableWidgetItem(list->at(i).value));

            if(list->at(i).name == "mp_friendlyfire")
                info->ff = list->at(i).value.toInt() ? "On":"Off";
            if(list->at(i).name == "mp_timelimit")
                info->timelimit = list->at(i).value;
            if(list->at(i).name == "sm_nextmap")
                info->nextMap = list->at(i).value;
            if(list->at(i).name == "sourcemod_version")
                info->mods.append(QString("SourceMod v%1").arg(list->at(i).value));
            if(list->at(i).name == "metamod_version")
                info->mods.prepend(QString("MetaMod v%1").arg(list->at(i).value));
        }
    }

    //Add info list items, gg
    while(this->ui->infoTable->rowCount() > 0 && current)
    {
        this->ui->infoTable->removeRow(0);
    }

    if(info->haveInfo && current)
    {
        if(!info->currentMap.isEmpty() && !info->nextMap.isEmpty())
        {
            mapString = QString("%1 (Nextmap : %2)").arg(info->currentMap, info->nextMap);
        }
        else if(!info->currentMap.isEmpty())
        {
            mapString = info->currentMap;
        }

        if(!info->gameName.isEmpty() && info->appId != -1)
        {
            gameString = QString("%1 (%2)").arg(info->gameName, QString::number(info->appId));
        }
        else if(!info->gameName.isEmpty())
        {
            gameString = info->gameName;
        }

        QList<InfoTableItem> items;
        items.append(InfoTableItem("Server IP", info->ipPort));
        items.append(InfoTableItem("PingGraph", ""));//Not used but place holder
        items.append(InfoTableItem("Server Name", info->serverName));
        items.append(InfoTableItem("Game", gameString));
        items.append(InfoTableItem("Players", info->playerCount));
        items.append(InfoTableItem("Map", mapString));
        items.append(InfoTableItem("Timelimit", info->timelimit));
        //This line is ugly, but im way too lazy.
        items.append(InfoTableItem("Version", QString("v%1 (%2, %3, Protocol %4)").arg(info->version, info->os == "l" ? "Linux" : info->os == "m" ? "Mac" : "Windows", info->type == "d" ? "Dedicated" : "Local", QString::number(info->protocol))));

        QString modString;

        if(info->mods.length() > 0)
        {
            modString = info->mods.join("  ");
        }

        items.append(InfoTableItem("Addons", modString));
        items.append(InfoTableItem("AntiCheat", info->vac ? "VAC" : ""));
        items.append(InfoTableItem("Steam ID", info->serverID));

        quint8 row = 0;
        InfoTableItem item;

        for(int i = 0; i < items.length(); i++)
        {
            item = items.at(i);

            if(i == 1)//Ping graph
            {
                this->ui->infoTable->insertRow(row);
                this->ui->infoTable->setSpan(row, 0, 1, 2);
                this->ui->infoTable->setRowHeight(row, 50);
                QPixmap pixmap(this->ui->infoTable->width(), 50);
                pixmap.fill(Qt::transparent);

                QPainter painter(&pixmap);

                painter.setPen(QPen(Qt::transparent));
                QLinearGradient lgrad(QPoint(0, 0), QPoint(0,50));
                lgrad.setColorAt(0.0, Qt::red);
                lgrad.setColorAt(1.0, Qt::green);
                painter.setBrush(lgrad);

                int idx = (info->pingList.length() - this->ui->infoTable->width()) > 0 ? info->pingList.length() - this->ui->infoTable->width() : 0;

                for(int i = idx; i < info->pingList.length(); i++)
                {
                    int h = qRound(((float)info->pingList.at(i)/300.0)*50);

                    if(h <= 1)
                        h = 2;

                    if(h >= 50)
                        h = 50;

                    painter.drawRect((1*(i-idx)), 50-h, 1, h);
                }
                QLabel *label = new QLabel(this);
                label->setPixmap(pixmap);
                this->ui->infoTable->setCellWidget(row, 0, label);
                row++;
            }
            else if(!item.val.isEmpty())
            {
                this->ui->infoTable->insertRow(row);
                this->ui->infoTable->setItem(row, 0, new QTableWidgetItem(item.display));
                this->ui->infoTable->setItem(row, 1, new QTableWidgetItem(item.val));
                row++;
            }
        }
        items.clear();
    }
}

//QUERY INFO READY
void MainWindow::ServerInfoReady(InfoReply *reply, ServerTableIndexItem *indexCell)
{
    QTableWidget *browserTable = this->ui->browserTable;

    int row = -1;

    //Make sure our item still exists.
    for(int i = 0; i < this->ui->browserTable->rowCount(); i++)
    {
        if(indexCell == this->GetServerTableIndexItem(i))
        {
            row = i;
            break;
        }
    }

    if(row == -1)
    {
        if(reply)
            delete reply;
        return;
    }

    ServerInfo *info = indexCell->GetServerInfo();

    if(reply)
    {
        info->lastPing = reply->ping;

        while(info->pingList.length() >= 1000)
        {
            info->pingList.removeFirst();
        }

        info->pingList.append(reply->ping);

        quint64 totalPing = 0;
        quint16 totalPings = 0;

        for(int i = 0; i < info->pingList.length(); i++)
        {
            if(info->pingList.at(i) == 2000)//only count completed pings
                continue;

            totalPing += info->pingList.at(i);
            totalPings++;
        }

        if(totalPings)
        {
            info->avgPing = totalPing/totalPings;
        }

        this->CreateTableItemOrUpdate(row, kBrowserColPing, browserTable, info);
    }

    if(reply && reply->success)
    {
        bool appIdChanged = info->appId != reply->appId;

        info->vac = reply->vac;
        info->appId = reply->appId;
        info->os = reply->os;
        info->tags = reply->tags;
        info->rawServerId = reply->rawServerId;
        info->protocol = reply->protocol;
        info->version = reply->version;
        info->currentMap = reply->map;
        info->gameName = reply->gamedesc;
        info->type = reply->type;
        info->serverName = reply->hostname;
        info->playerCount = QString("%1 (%3)/%2").arg(QString::number(reply->players), QString::number(reply->maxplayers), QString::number(reply->bots));
        info->haveInfo = true;
        info->serverID = reply->serverID;
        info->queryState = QuerySuccess;
        info->maxPlayers = reply->maxplayers;
        info->currentPlayers = reply->players;

        if (!info->countryFlag.isNull())
        {
            this->CreateTableItemOrUpdate(row, kBrowserColFlagIcon, browserTable, info);
        }

        if(appIdChanged)
        {
            this->CreateTableItemOrUpdate(row, kBrowserColModIcon, browserTable, info);
        }

        if(reply->vac)
        {
            this->CreateTableItemOrUpdate(row, kBrowserColVACIcon, browserTable, info);
        }
        else if(!reply->vac && browserTable->item(row, kBrowserColVACIcon))
        {
            browserTable->removeCellWidget(row, kBrowserColVACIcon);
        }

        if(reply->visibility)
        {
            this->CreateTableItemOrUpdate(row, kBrowserColLockIcon, browserTable, info);
        }
        else if(!reply->visibility && browserTable->item(row, kBrowserColLockIcon))
        {
            browserTable->removeCellWidget(row, kBrowserColLockIcon);
        }

        this->CreateTableItemOrUpdate(row, kBrowserColHostname, browserTable, info);
        this->CreateTableItemOrUpdate(row, kBrowserColMap, browserTable, info);
        this->CreateTableItemOrUpdate(row, kBrowserColPlayerCount, browserTable, info);

        delete reply;
    }
    else
    {
        info->queryState = QueryFailed;
         this->CreateTableItemOrUpdate(row, kBrowserColHostname, browserTable, info);

        if(reply)
            delete reply;
    }
    this->UpdateInfoTable(info, (row == this->ui->browserTable->currentRow()));
}

void MainWindow::PlayerInfoReady(QList<PlayerInfo> *list, ServerTableIndexItem *indexCell)
{
    if(this->ui->browserTable->selectedItems().empty() || this->ui->browserTable->selectedItems().at(0) != indexCell)
    {
        if(list)
        {
            delete list;
        }

        return;
    }

    ServerInfo *info = indexCell->GetServerInfo();
    this->ui->playerTable->setRowCount(0);

    this->ui->playerTable->setSortingEnabled(false);

    for(int i = 0; i < list->size(); i++)
    {
        QTableWidgetItem *itemScore = new QTableWidgetItem();
        QTableWidgetItem *id = new QTableWidgetItem();

        id->setData(Qt::DisplayRole, i+1);
        itemScore->setData(Qt::DisplayRole, list->at(i).score);

        this->ui->playerTable->insertRow(i);
        this->ui->playerTable->setItem(i, 0, id);
        this->ui->playerTable->setItem(i, 1, new QTableWidgetItem(list->at(i).name));
        this->ui->playerTable->setItem(i, 2, itemScore);

        PlayerTimeTableItem *time = new PlayerTimeTableItem();
        time->updateTime(list->at(i).time);

        this->ui->playerTable->setItem(i, 3, time);

        QString steamID = "";

        if(info->logHashTable.contains(list->at(i).name))
        {
            steamID = (info->logHashTable.value(list->at(i).name).steamID);
        }

        this->ui->playerTable->setItem(i, 4, new QTableWidgetItem(steamID));
    }

    if(list)
    {
        delete list;
    }

    this->ui->playerTable->setSortingEnabled(true);
}

void MainWindow::RulesInfoReady(QList<RulesInfo> *list, ServerTableIndexItem *indexCell)
{
    if(this->ui->browserTable->selectedItems().empty() || this->ui->browserTable->selectedItems().at(0) != indexCell)
    {
        if(list)
        {
            delete list;
        }

        return;
    }
    ServerInfo *info = indexCell->GetServerInfo();

    if(info->haveInfo)
    {
        list->append(RulesInfo("vac", QString::number(info->vac)));
        list->append(RulesInfo("version", info->version));
        list->append(RulesInfo("appID", QString::number(info->appId)));
        list->append(RulesInfo("os", info->os));

        if(info->rawServerId != 0)
        {
           list->append(RulesInfo("steamID64", QString::number(info->rawServerId)));
        }
    }

    this->ui->rulesTable->setRowCount(0);
    this->ui->rulesTable->setSortingEnabled(false);

    this->UpdateInfoTable(info, true, list);

    if(list)
    {
        delete list;
    }

    this->ui->rulesTable->setSortingEnabled(true);
}
