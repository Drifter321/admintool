#include "settings.h"
#include "ui_mainwindow.h"
#include "query.h"
#include "rcon.h"
#include "simplecrypt.h"
#include "util.h"
#include <QImage>
#include <QMap>
#include <QTime>
#include <QXmlStreamReader>

QMap<int, QString> appIDMap;
Settings *settings;
QList<ServerInfo *> serverList;
QList<ContextMenuItem> contextMenuItems;

Settings::Settings(MainWindow *main)
{
    this->pMain = main;
    this->pSettings = new QSettings(BuildPath("settings.ini"), QSettings::IniFormat);
    this->pAppIds = new QSettings(BuildPath("app_list_map.ini"), QSettings::IniFormat);
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

    auto browserTable = pMain->GetUi()->browserTable;
    browserTable->setColumnWidth(kBrowserColIndex,         50);
    browserTable->setColumnWidth(kBrowserColFlagIcon,        25);
    browserTable->setColumnWidth(kBrowserColModIcon,     25);
    browserTable->setColumnWidth(kBrowserColVACIcon,   25);
    browserTable->setColumnWidth(kBrowserColLockIcon,        25);
    browserTable->setColumnWidth(kBrowserColHostname,    600);
    browserTable->setColumnWidth(kBrowserColMap,         150);
    browserTable->setColumnWidth(kBrowserColPlayerCount, 80);
    browserTable->setColumnWidth(kBrowserColPing,        70);

    auto header = pMain->GetUi()->browserTable->horizontalHeader();
    header->setSectionResizeMode(kBrowserColIndex, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(kBrowserColFlagIcon, QHeaderView::Fixed);
    header->setSectionResizeMode(kBrowserColModIcon, QHeaderView::Fixed);
    header->setSectionResizeMode(kBrowserColVACIcon, QHeaderView::Fixed);
    header->setSectionResizeMode(kBrowserColLockIcon, QHeaderView::Fixed);
    header->setSectionResizeMode(kBrowserColHostname, QHeaderView::Stretch);
    header->setSectionResizeMode(kBrowserColMap, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(kBrowserColPlayerCount, QHeaderView::Fixed);
    header->setSectionResizeMode(kBrowserColPing, QHeaderView::Fixed);

    browserTable->horizontalHeaderItem(kBrowserColPlayerCount)->setTextAlignment(Qt::AlignLeft);
    browserTable->horizontalHeaderItem(kBrowserColPing)->setTextAlignment(Qt::AlignLeft);

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
            ServerInfo *info = pMain->AddServer(list.at(0));

            if(info && list.size() == 2)
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

void Settings::GetCtxCommands()
{
    QFile file(BuildPath("commands.xml"));

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QXmlStreamReader xml;
    xml.setDevice(&file);

    QString name;

    ContextMenuItem menuitem;

    while(!xml.atEnd())
    {
        name = xml.name().toString();
        if(xml.isStartElement())
        {
            if(name == "Command")
            {
                menuitem.cmd = xml.readElementText();
            }
            else if(name == "DisplayName")
            {
                menuitem.display = xml.readElementText();
            }
            else if(name == "ParamType")
            {
                QString elem = xml.readElementText();
                if(elem == "SteamID")
                {
                    menuitem.type = ContextTypeSteamID;
                }
                else if(elem == "Name")
                {
                    menuitem.type = ContextTypeName;
                }
                else
                {
                    menuitem.type = ContextTypeNone;
                }
            }

            //If it is a group handle it in a loop
            if(name == "SubOptions")
            {
                xml.readNext();
                name = xml.name().toString();

                while(name != "SubOptions" && !xml.isEndElement())
                {
                    if(name == "TitleName")
                    {
                        menuitem.subTitle = xml.readElementText();
                    }
                    else if(name == "Default")
                    {
                        menuitem.defaultSub = xml.readElementText();
                    }
                    else if(name == "CommandGroup")
                    {
                        xml.readNext();
                        name = xml.name().toString();
                        CtxSubItem subItem;

                        while(name != "CommandGroup" && !xml.isEndElement())
                        {
                           if(name == "Display")
                           {
                               subItem.display = xml.readElementText();
                           }
                           else if(name == "ParamValue")
                           {
                               subItem.val = xml.readElementText();
                           }
                           xml.readNext();
                           name = xml.name().toString();
                        }
                        menuitem.subItems.append(subItem);
                    }
                    xml.readNext();
                    name = xml.name().toString();
                }
            }
        }
        else if(xml.isEndElement() && name == "Option")
        {
            contextMenuItems.append(menuitem);
            menuitem.clear();
        }
        xml.readNext();
    }
}
