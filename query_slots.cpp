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

            infoQuery->query(serverList.at(index-1), id);
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

    //Ignore this workers results delete is called automatically on quit
    if(pPlayerQuery)
    {
        pPlayerQuery->disconnect(pPlayerQuery->worker, &Worker::playerInfoReady, this, &MainWindow::PlayerInfoReady);
    }

    if(pRulesQuery && updateRules)
    {
        pRulesQuery->disconnect(pRulesQuery->worker, &Worker::rulesInfoReady, this, &MainWindow::RulesInfoReady);
    }

    if(removeFirst)
    {
        while(this->ui->playerTable->rowCount() > 0)
        {
            this->ui->playerTable->removeRow(0);
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
    pPlayerQuery->query(serverList.at(index-1), item);

    if(updateRules)
    {
        pRulesQuery = new RulesQuery(this);
        pRulesQuery->query(serverList.at(index-1), item);
    }
}

//QUERY INFO READY
void MainWindow::ServerInfoReady(ServerInfo *info, InfoReply *reply, QTableWidgetItem *indexCell)
{
    if(!info->isValid || !indexCell)
        return;

    int row = this->ui->browserTable->rowCount();

    for(int i = 0; i < row; i++)
    {
        if(this->ui->browserTable->item(i, 0) == indexCell)
        {
            row = i;
            break;
        }
    }

    if(reply && reply->success)
    {
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

        this->ui->browserTable->setItem(row, 4, new QTableWidgetItem(reply->hostname));
        this->ui->browserTable->setItem(row, 5, new QTableWidgetItem(reply->map));

        PlayerTableItem *playerItem = new PlayerTableItem();
        playerItem->players = reply->players;
        playerItem->setText(QString("%1/%2 (%3)").arg(QString::number(reply->players), QString::number(reply->maxplayers), QString::number(reply->bots)));

        this->ui->browserTable->setItem(row, 6, playerItem);

        this->ui->browserTable->setSortingEnabled(true);
        delete reply;
    }
    else
    {
        QTableWidgetItem *item = new QTableWidgetItem();

        item->setTextColor(errorColor);
        item->setText(QString("Failed to query %1, retrying in %2 seconds").arg(info->ipPort, QString::number(UPDATE_TIME)));
        this->ui->browserTable->setItem(row, 4, item);
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

    while(this->ui->rulesTable->rowCount() > 0)
    {
        this->ui->rulesTable->removeRow(0);
    }

    this->ui->rulesTable->setSortingEnabled(false);

    for(int i = 0; i < list->size(); i++)
    {
        this->ui->rulesTable->insertRow(i);
        this->ui->rulesTable->setItem(i, 0, new QTableWidgetItem(list->at(i).name));
        this->ui->rulesTable->setItem(i, 1, new QTableWidgetItem(list->at(i).value));
    }

    if(list)
    {
        delete list;
    }

    this->ui->rulesTable->setSortingEnabled(true);
}
