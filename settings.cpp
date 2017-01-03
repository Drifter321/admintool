#include "settings.h"
#include "ui_mainwindow.h"
#include "query.h"
#include "rcon.h"
#include "simplecrypt.h"
#include <QImage>
#include <QMap>
#include <QTime>

QColor errorColor(255, 60, 60);
QColor queryingColor(80, 170, 80);
QMap<int, QString> appIDMap;
Settings *settings;
QList<ServerInfo *> serverList;

Settings::Settings(MainWindow *main)
{
    this->pMain = main;
    this->pSettings = new QSettings("settings.ini", QSettings::IniFormat);
    this->pAppIds = new QSettings("app_list_map.ini", QSettings::IniFormat);
    GetAppIDListMap();
}

Settings::~Settings()
{
    this->SaveSettings();
    delete this->pSettings;
    delete this->pAppIds;
}

void Settings::GetAppIDListMap()
{
    QStringList keys = pAppIds->allKeys();

    for(int i = 0; i < keys.size(); i++)
    {
        appIDMap.insert(pAppIds->value(keys.at(i), -1).toInt(), keys.at(i));
    }
}

void Settings::SetDefaultSettings()
{
    pMain->showMaximized();

    QList<int> intList;
    intList.append(qRound(pMain->GetUi()->splitter->height()*.5));
    intList.append(qRound(pMain->GetUi()->splitter->height()*.25));
    intList.append(qRound(pMain->GetUi()->splitter->height()*.25));

    pMain->GetUi()->splitter->setSizes(intList);

    intList.clear();

    intList.append(qRound(pMain->GetUi()->splitter->width()*.25));
    intList.append(qRound(pMain->GetUi()->splitter->width()*.75));

    pMain->GetUi()->rulesSplitter->setSizes(intList);

    intList.clear();

    intList.append(qRound(pMain->GetUi()->splitter->width()*.80));
    intList.append(qRound(pMain->GetUi()->splitter->width()*.20));

    pMain->GetUi()->browserSplitter->setSizes(intList);

    intList.clear();

    intList.append(50);
    intList.append(25);
    intList.append(25);
    intList.append(25);
    intList.append(600);
    intList.append(150);
    intList.append(100);
    intList.append(70);

    for(int i = 0; i < intList.size(); i++)
        pMain->GetUi()->browserTable->setColumnWidth(i, intList.at(i));

    pMain->GetUi()->browserTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
    pMain->GetUi()->browserTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Fixed);
    pMain->GetUi()->browserTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Fixed);
    pMain->GetUi()->browserTable->horizontalHeaderItem(5)->setTextAlignment(Qt::AlignLeft);
    pMain->GetUi()->browserTable->horizontalHeaderItem(6)->setTextAlignment(Qt::AlignLeft);

    intList.clear();

    intList.append(125);
    intList.append(125);

    for(int i = 0; i < intList.size(); i++)
        pMain->GetUi()->rulesTable->setColumnWidth(i, intList.at(i));

    intList.clear();

    intList.append(75);
    intList.append(225);
    intList.append(100);
    intList.append(150);
    intList.append(200);

    for(int i = 0; i < intList.size(); i++)
        pMain->GetUi()->playerTable->setColumnWidth(i, intList.at(i));

    qsrand((uint)(QTime::currentTime()).msec());
    pMain->u16logPort = qrand() % ((PORT_MAX + 1) - PORT_MIN) + PORT_MIN;

    pMain->showLoggingInfo = true;

    pMain->GetUi()->rconShow->setChecked(false);
}

void Settings::ReadSettings()
{
    if(!pSettings->value("windowMaximized", pMain->isMaximized()).toBool())
    {
        pMain->restoreGeometry(pSettings->value("windowSize", pMain->saveGeometry()).toByteArray());
    }

    pMain->restoreState(pSettings->value("windowState", pMain->saveState()).toByteArray());

    pMain->GetUi()->splitter->restoreState(pSettings->value("splitterState", pMain->GetUi()->splitter->saveState()).toByteArray());

    pMain->GetUi()->rulesSplitter->restoreState(pSettings->value("rulesSplitterState", pMain->GetUi()->rulesSplitter->saveState()).toByteArray());

    pMain->GetUi()->browserSplitter->restoreState(pSettings->value("browserSplitterState", pMain->GetUi()->browserSplitter->saveState()).toByteArray());

    pMain->GetUi()->browserTable->horizontalHeader()->restoreState(pSettings->value("browserTableState", pMain->GetUi()->browserTable->horizontalHeader()->saveState()).toByteArray());

    pMain->GetUi()->playerTable->horizontalHeader()->restoreState(pSettings->value("playerTableState", pMain->GetUi()->playerTable->horizontalHeader()->saveState()).toByteArray());

    pMain->GetUi()->rulesTable->horizontalHeader()->restoreState(pSettings->value("rulesTableState", pMain->GetUi()->rulesTable->horizontalHeader()->saveState()).toByteArray());

    bool darkTheme = pSettings->value("darkTheme", false).toBool();

    if(pSettings->value("showRCONpass", false).toBool())
    {
        pMain->GetUi()->rconShow->setChecked(true);
        pMain->GetUi()->rconPassword->setEchoMode(QLineEdit::Normal);
    }

    pMain->showLoggingInfo = pSettings->value("showLoggingInfo", true).toBool();

    uint temp = pSettings->value("logPort", pMain->u16logPort).toUInt();

    if(temp <= PORT_MAX && temp >= PORT_MIN)
    {
        pMain->u16logPort = temp;
    }

    if(darkTheme)
    {
        pMain->GetUi()->menuSettings->actions().at(0)->setChecked(true);
        pMain->darkThemeTriggered();
    }

    pSettings->beginGroup("servers");
    QStringList keys = pSettings->childKeys();

    for(int i = 1; i <= keys.size(); i++)
    {   
        QStringList list = pSettings->value(QString::number(i), QStringList()).toStringList();

        if(list.size() == 0 || list.size() > 2)
        {
            continue;
        }

        if(pMain->CheckServerList(list.at(0)) == AddServerError_None)
        {
            ServerInfo *info = new ServerInfo(list.at(0));
            serverList.append(info);

            int row = pMain->GetUi()->browserTable->rowCount();
            pMain->GetUi()->browserTable->insertRow(row);

            QTableWidgetItem *item = new QTableWidgetItem();
            QTableWidgetItem *id = new QTableWidgetItem();
            id->setData(Qt::DisplayRole, serverList.size());

            item->setTextColor(queryingColor);
            item->setText(QString("Querying server %1...").arg(info->ipPort));
            item->setToolTip(info->ipPort);
            pMain->GetUi()->browserTable->setItem(row, 0, id);
            pMain->GetUi()->browserTable->setItem(row, 4, item);

            InfoQuery *infoQuery = new InfoQuery(pMain);
            infoQuery->query(&info->host, info->port, id);

            if(list.size() == 2)
            {
                SimpleCrypt encrypt;
                encrypt.setKey(list.at(0).toLongLong());
                QString pass = encrypt.decryptToString(list.at(1));
                if(encrypt.lastError() == SimpleCrypt::ErrorNoError)
                {
                    info->rconPassword = pass;
                    info->saveRcon = true;
                }
            }
        }
    }
    pSettings->endGroup();
    this->SaveSettings();//Call this again to make sure we cleaned up any garbage entries
}

void Settings::SaveSettings()
{
    pSettings->setValue("windowState", pMain->saveState());

    if(!pMain->isMaximized())
        pSettings->setValue("windowSize", pMain->saveGeometry());

    pSettings->setValue("windowMaximized", pMain->isMaximized());

    pSettings->setValue("splitterState", pMain->GetUi()->splitter->saveState());

    pSettings->setValue("rulesSplitterState", pMain->GetUi()->rulesSplitter->saveState());

    pSettings->setValue("browserSplitterState", pMain->GetUi()->browserSplitter->saveState());

    pSettings->setValue("browserTableState", pMain->GetUi()->browserTable->horizontalHeader()->saveState());

    pSettings->setValue("rulesTableState", pMain->GetUi()->rulesTable->horizontalHeader()->saveState());

    pSettings->setValue("playerTableState", pMain->GetUi()->playerTable->horizontalHeader()->saveState());

    pSettings->setValue("darkTheme", pMain->GetUi()->menuSettings->actions().at(0)->isChecked());

    pSettings->setValue("showLoggingInfo", pMain->showLoggingInfo);

    pSettings->setValue("logPort", pMain->u16logPort);

    pSettings->setValue("showRCONpass", pMain->GetUi()->rconShow->isChecked());

    pSettings->beginGroup("servers");

    pSettings->remove("");

    if(serverList.size() > 0)
    {
        for(int i = 0; i < serverList.size(); i++)
        {
            ServerInfo *info = serverList.at(i);
            QStringList list;
            list.append(info->ipPort);

            if(info->saveRcon && info->rconPassword.length() >0)
            {
                SimpleCrypt encrypt;
                encrypt.setKey(info->ipPort.toLongLong());
                list.append(encrypt.encryptToString(info->rconPassword));

            }
            pSettings->setValue(QString::number(i+1), list);
        }
    }

    pSettings->endGroup();
}
