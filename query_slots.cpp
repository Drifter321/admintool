#include "ui_mainwindow.h"
#include "mainwindow.h"
#include "customitems.h"
#include "worker.h"
#include "query.h"
#include "serverinfo.h"
#include "settings.h"

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

    if(reply && reply->success)
    {
        info->vac = reply->vac;
        info->appId = reply->appId;
        info->os = reply->os;
        info->tags = reply->tags;
        info->protocol = reply->protocol;
        info->version = reply->version;
        info->currentMap = reply->map;
        info->gameName = reply->gamedesc;
        info->type = reply->type;
        info->serverName = reply->hostname;
        info->playerCount = QString("%1/%2 (%3)").arg(QString::number(reply->players), QString::number(reply->maxplayers), QString::number(reply->bots));
        info->haveInfo = true;

        QTableWidgetItem *mod = new QTableWidgetItem();

        QImage icon;

        icon.load(appIDMap.value(info->appId, QString(":/icons/icons/hl2.gif")));

        mod->setData(Qt::DecorationRole, icon);

        this->ui->browserTable->setSortingEnabled(false);

        this->ui->browserTable->setItem(row, 1, mod);

        if(reply->vac)
        {
            QTableWidgetItem *vacItem = new QTableWidgetItem();
            vacItem->setData(Qt::DecorationRole, this->GetVACImage());
            this->ui->browserTable->setItem(row, 2, vacItem);
        }

        if(reply->visibility)
        {
            QTableWidgetItem *lockedItem = new QTableWidgetItem();
            lockedItem->setData(Qt::DecorationRole, this->GetLockImage());
            this->ui->browserTable->setItem(row, 3, lockedItem);
        }

        QTableWidgetItem *hostname = new QTableWidgetItem(reply->hostname);
        hostname->setToolTip(info->ipPort);
        this->ui->browserTable->setItem(row, 4,hostname);
        QTableWidgetItem *mapItem = new QTableWidgetItem(reply->map);
        mapItem->setTextColor(errorColor);
        this->ui->browserTable->setItem(row, 5, mapItem);

        PlayerTableItem *playerItem = new PlayerTableItem();
        playerItem->players = reply->players;

        if(reply->players == reply->maxplayers)
            playerItem->setTextColor(errorColor);
        else
            playerItem->setTextColor(queryingColor);

        playerItem->setText(info->playerCount);
        this->ui->browserTable->setItem(row, 6, playerItem);

        this->ui->browserTable->setSortingEnabled(true);
        delete reply;
    }
    else
    {
        QTableWidgetItem *item = new QTableWidgetItem();

        item->setTextColor(errorColor);
        item->setText(QString("Failed to query %1, retrying in %2 seconds").arg(info->ipPort, QString::number(UPDATE_TIME)));
        item->setToolTip(info->ipPort);
        this->ui->browserTable->setItem(row, 4, item);

        if(reply)
            delete reply;
    }
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
    }

    while(this->ui->rulesTable->rowCount() > 0)
    {
        this->ui->rulesTable->removeRow(0);
    }

    this->ui->rulesTable->setSortingEnabled(false);

    QString nextmap;
    QString ff;
    QString timelimit;

    QStringList mods;

    for(int i = 0; i < list->size(); i++)
    {
        this->ui->rulesTable->insertRow(i);
        this->ui->rulesTable->setItem(i, 0, new QTableWidgetItem(list->at(i).name));
        this->ui->rulesTable->setItem(i, 1, new QTableWidgetItem(list->at(i).value));

        if(list->at(i).name == "mp_friendlyfire")
            ff = list->at(i).value.toInt() ? "On":"Off";
        if(list->at(i).name == "mp_timelimit")
            timelimit = list->at(i).value;
        if(list->at(i).name == "sm_nextmap")
            nextmap = list->at(i).value;
        if(list->at(i).name == "sourcemod_version")
            mods.append(QString("Sourcemod v%1").arg(list->at(i).value));
        if(list->at(i).name == "metamod_version")
            mods.prepend(QString("MetaMod v%1").arg(list->at(i).value));
    }

    //Add info list items, gg
    while(this->ui->infoTable->rowCount() > 0)
    {
        this->ui->infoTable->removeRow(0);
    }
    if(info->haveInfo)
    {
        this->ui->infoTable->insertRow(0);
        this->ui->infoTable->setItem(0, 0, new QTableWidgetItem("Server IP"));
        this->ui->infoTable->setItem(0, 1, new QTableWidgetItem(info->ipPort));

        this->ui->infoTable->insertRow(1);
        this->ui->infoTable->setItem(1, 0, new QTableWidgetItem("Server Name"));
        this->ui->infoTable->setItem(1, 1, new QTableWidgetItem(info->serverName));

        this->ui->infoTable->insertRow(2);
        this->ui->infoTable->setItem(2, 0, new QTableWidgetItem("Game"));
        this->ui->infoTable->setItem(2, 1, new QTableWidgetItem(QString("%1 (%2)").arg(info->gameName, QString::number(info->appId))));

        this->ui->infoTable->insertRow(3);
        this->ui->infoTable->setItem(3, 0, new QTableWidgetItem("Players"));
        this->ui->infoTable->setItem(3, 1, new QTableWidgetItem(info->playerCount));

        this->ui->infoTable->insertRow(4);
        this->ui->infoTable->setItem(4, 0, new QTableWidgetItem("Map"));
        this->ui->infoTable->setItem(4, 1, new QTableWidgetItem(QString("%1 (Nextmap : %2)").arg(info->currentMap, nextmap)));

        this->ui->infoTable->insertRow(5);
        this->ui->infoTable->setItem(5, 0, new QTableWidgetItem("Timelimit"));
        this->ui->infoTable->setItem(5, 1, new QTableWidgetItem(timelimit));

        this->ui->infoTable->insertRow(6);
        this->ui->infoTable->setItem(6, 0, new QTableWidgetItem("Friendly Fire"));
        this->ui->infoTable->setItem(6, 1, new QTableWidgetItem(ff));

        this->ui->infoTable->insertRow(7);
        this->ui->infoTable->setItem(7, 0, new QTableWidgetItem("Version"));
        //This line is ugly, but im way too lazy.
        this->ui->infoTable->setItem(7, 1, new QTableWidgetItem(QString("v%1 (%2, %3, Protocol %4)").arg(info->version, info->os == "l" ? "Linux" : info->os == "m" ? "Mac" : "Windows", info->type == "d" ? "Dedicated" : "Local", QString::number(info->protocol))));

        this->ui->infoTable->insertRow(8);
        this->ui->infoTable->setItem(8, 0, new QTableWidgetItem("Addons"));
        this->ui->infoTable->setItem(8, 1, new QTableWidgetItem(mods.join("  ")));

        this->ui->infoTable->insertRow(9);
        this->ui->infoTable->setItem(9, 0, new QTableWidgetItem("AntiCheat"));
        this->ui->infoTable->setItem(9, 1, new QTableWidgetItem(info->vac ? "VAC" : ""));
    }

    if(list)
    {
        delete list;
    }

    this->ui->rulesTable->setSortingEnabled(true);
}
