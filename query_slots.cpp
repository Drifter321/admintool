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
extern QColor errorColor;
extern QColor queryingColor;

#define UPDATE_TIME 15

//TIMER TRIGGERED UPDATING
void MainWindow::TimedUpdate()
{
    static int run = 1;

    if(run % UPDATE_TIME == 0)
    {
        for(int i = 0; i < this->ui->browserTable->rowCount(); i++)
        {
            QTableWidgetItem *id = this->ui->browserTable->item(i, 0);
            int index = id->text().toInt();

            InfoQuery *infoQuery = new InfoQuery(this);

            serverList.at(index-1)->cleanHashTable();

            infoQuery->query(&serverList.at(index-1)->host, serverList.at(index-1)->port, id);
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

    QTableWidgetItem *item = this->ui->browserTable->selectedItems().at(0);
    int index = item->text().toInt();

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
    pPlayerQuery->query(&serverList.at(index-1)->host, serverList.at(index-1)->port, item);

    if(updateRules)
    {
        pRulesQuery = new RulesQuery(this);
        pRulesQuery->query(&serverList.at(index-1)->host, serverList.at(index-1)->port, item);
    }
    this->UpdateInfoTable(serverList.at(index-1));
}

void MainWindow::SetTableItemAndDelete(int row, int col, QTableWidgetItem *item)
{
    QTableWidgetItem *current = this->ui->browserTable->item(row, col);

    if(current)
    {
        delete current;
    }

    this->ui->browserTable->setItem(row, col, item);
}

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
                pixmap.fill(QColor("transparent"));

                QPainter painter(&pixmap);

                painter.setPen(QPen(Qt::transparent));
                QLinearGradient lgrad(QPoint(0, 0), QPoint(0,50));
                lgrad.setColorAt(0.0, Qt::red);
                lgrad.setColorAt(1.0, Qt::green);
                painter.setBrush(lgrad);

                for(int i = 0; i < info->pingList.length(); i++)
                {
                    if(i > this->ui->infoTable->width())
                        break;

                    int h = qRound(((float)info->pingList.at(i)/300.0)*50);

                    if(h <= 1)
                        h = 2;

                    if(h >= 50)
                        h = 50;

                    painter.drawRect((1*i), 50-h, 1, h);
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
void MainWindow::ServerInfoReady(InfoReply *reply, QTableWidgetItem *indexCell)
{
    int index = -1;
    int row = -1;

    for(int i = 0; i < this->ui->browserTable->rowCount(); i++)
    {
        QTableWidgetItem *cell = this->ui->browserTable->item(i, 0);

        if(cell == indexCell)
        {
            row = i;
            index = cell->text().toInt();
            break;
        }
    }

    if(index == -1)
    {
        if(reply)
            delete reply;
        return;
    }

    ServerInfo *info = serverList.at(index-1);

    if(!info || !info->isValid)
    {
        if(reply)
            delete reply;
        return;
    }

    if(reply)
    {
        while(info->pingList.length() >= 1000)
        {
            info->pingList.removeFirst();
        }

        info->pingList.append(reply->ping);

        quint64 totalPing = 0;
        for(int i = 0; i < info->pingList.length(); i++)
        {
            totalPing += info->pingList.at(i);
        }

        QTableWidgetItem *pingItem = new QTableWidgetItem(QString("%1, Ø%2").arg(QString::number(reply->ping), QString::number(totalPing/info->pingList.length())));

        if(reply->ping > 200)
            pingItem->setTextColor(errorColor);
        else
            pingItem->setTextColor(queryingColor);

        this->SetTableItemAndDelete(row, 7, pingItem);
    }

    if(reply && reply->success)
    {
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

        QTableWidgetItem *mod = new QTableWidgetItem();

        QImage icon;

        icon.load(appIDMap.value(info->appId, QString(":/icons/icons/hl2.gif")));

        mod->setData(Qt::DecorationRole, icon);

        this->ui->browserTable->setSortingEnabled(false);

        this->SetTableItemAndDelete(row, 1, mod);

        if(reply->vac)
        {
            QTableWidgetItem *vacItem = new QTableWidgetItem();
            vacItem->setData(Qt::DecorationRole, this->GetVACImage());
            this->SetTableItemAndDelete(row, 2, vacItem);
        }

        if(reply->visibility)
        {
            QTableWidgetItem *lockedItem = new QTableWidgetItem();
            lockedItem->setData(Qt::DecorationRole, this->GetLockImage());
            this->SetTableItemAndDelete(row, 3, lockedItem);
        }

        QTableWidgetItem *hostname = new QTableWidgetItem(reply->hostname);
        hostname->setToolTip(info->ipPort);
        this->ui->browserTable->setItem(row, 4,hostname);

        QTableWidgetItem *mapItem = new QTableWidgetItem(reply->map);
        mapItem->setTextColor(errorColor);
        this->SetTableItemAndDelete(row, 5, mapItem);

        PlayerTableItem *playerItem = new PlayerTableItem();
        playerItem->players = reply->players;

        if(reply->players == reply->maxplayers)
            playerItem->setTextColor(errorColor);
        else
            playerItem->setTextColor(queryingColor);

        playerItem->setText(info->playerCount);
        this->SetTableItemAndDelete(row, 6, playerItem);

        this->ui->browserTable->setSortingEnabled(true);
        delete reply;
    }
    else
    {
        QTableWidgetItem *item = new QTableWidgetItem();

        item->setTextColor(errorColor);
        item->setText(QString("Failed to query %1, retrying in %2 seconds").arg(info->ipPort, QString::number(UPDATE_TIME)));
        item->setToolTip(info->ipPort);
        this->SetTableItemAndDelete(row, 4, item);

        if(reply)
            delete reply;
    }
    this->UpdateInfoTable(info, (row == this->ui->browserTable->currentRow()));
}

void MainWindow::PlayerInfoReady(QList<PlayerInfo> *list, QTableWidgetItem *indexCell)
{
    if(this->ui->browserTable->selectedItems().empty() || this->ui->browserTable->selectedItems().at(0) != indexCell)
    {
        if(list)
        {
            delete list;
        }

        return;
    }

    int index = this->ui->browserTable->selectedItems().at(0)->text().toInt();

    ServerInfo *info = serverList.at(index-1);

    while(this->ui->playerTable->rowCount() > 0)
    {
        this->ui->playerTable->removeRow(0);
    }

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

void MainWindow::RulesInfoReady(QList<RulesInfo> *list, QTableWidgetItem *indexCell)
{
    if(this->ui->browserTable->selectedItems().empty() || this->ui->browserTable->selectedItems().at(0) != indexCell)
    {
        if(list)
        {
            delete list;
        }

        return;
    }

    int index = this->ui->browserTable->selectedItems().at(0)->text().toInt();

    ServerInfo *info = serverList.at(index-1);
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

    while(this->ui->rulesTable->rowCount() > 0)
    {
        this->ui->rulesTable->removeRow(0);
    }

    this->ui->rulesTable->setSortingEnabled(false);

    this->UpdateInfoTable(info, true, list);

    if(list)
    {
        delete list;
    }

    this->ui->rulesTable->setSortingEnabled(true);
}
