#include "ui_mainwindow.h"
#include "mainwindow.h"
#include "worker.h"
#include "query.h"
#include "serverinfo.h"
#include "settings.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QPalette>
#include <QSignalMapper>

#define STEAMID_COLUMN 4
#define NAME_COLUMN 1

extern QPalette defaultPalette;
extern QMap<int, QString> appIDMap;
extern Settings *settings;
extern QList<ServerInfo *> serverList;
extern QColor errorColor;
extern QColor queryingColor;
extern QList<ContextMenuItem> contextMenuItems;

void MainWindow::ConnectSlots()
{
    this->ui->commandText->connect(this->ui->commandText, &QLineEdit::returnPressed, this, &MainWindow::processCommand);
    this->ui->commandText->connect(this->ui->sendChat, &QLineEdit::returnPressed, this, &MainWindow::sendChat);
    this->ui->rconPassword->connect(this->ui->rconPassword, &QLineEdit::returnPressed, this, &MainWindow::rconLogin);
    this->ui->rconPassword->connect(this->ui->rconPassword, &QLineEdit::textChanged, this, &MainWindow::passwordUpdated);
    this->ui->rconSave->connect(this->ui->rconSave, &QCheckBox::toggled, this, &MainWindow::rconSaveToggled);
    this->ui->rconLogin->connect(this->ui->rconLogin, &QPushButton::released, this, &MainWindow::rconLogin);
    this->ui->logGetLog->connect(this->ui->logGetLog, &QPushButton::released, this, &MainWindow::getLog);
    this->ui->actionAdd_Server->connect(this->ui->actionAdd_Server, &QAction::triggered, this, &MainWindow::addServerEntry);
    this->ui->actionDark_Theme->connect(this->ui->actionDark_Theme, &QAction::triggered, this, &MainWindow::darkThemeTriggered);
    this->ui->actionSet_Log_Port->connect(this->ui->actionSet_Log_Port, &QAction::triggered, this, &MainWindow::showPortEntry);
    this->ui->actionAbout->connect(this->ui->actionAbout, &QAction::triggered, this, &MainWindow::showAbout);
    this->ui->browserTable->connect(this->ui->browserTable, &QTableWidget::itemSelectionChanged, this, &MainWindow::browserTableItemSelected);
    this->ui->rconShow->connect(this->ui->rconShow, &QCheckBox::clicked, this, &MainWindow::showRconClicked);
    this->ui->playerTable->connect(this->ui->playerTable, &QTableWidget::customContextMenuRequested, this, &MainWindow::customPlayerContextMenu);
}

QString CreateCommand(QString command, QString subValue, ContextTypes type, QString name, QString SteamID)
{
    QString target = "";

    if(type == ContextTypeSteamID)
    {
        target = SteamID;
    }
    else if(type == ContextTypeName)
    {
        target = name;
    }

    if(!subValue.isEmpty())
    {
        if(target.isEmpty())
            return command.arg(subValue);
        else
            return command.arg(target, subValue);
    }
    else
    {
        if(target.isEmpty())
            return command;
        else
            return command.arg(target);
    }
}

void MainWindow::customPlayerContextMenu(const QPoint &pos)
{
    int row = this->ui->playerTable->rowAt(pos.y());

    if(row != -1)
    {
        QPoint globalpos = this->ui->playerTable->mapToGlobal(pos);
        QString name = this->ui->playerTable->item(row, NAME_COLUMN)->text();
        QString steamid = this->ui->playerTable->item(row, STEAMID_COLUMN)->text();

        QMenu *pContextMenu = new QMenu(this);

        QSignalMapper* signalMapper = new QSignalMapper(pContextMenu);

        ContextMenuItem ctxItem;

        foreach(ctxItem, contextMenuItems)
        {
            if((ctxItem.type == ContextTypeSteamID && steamid.isEmpty()) || (ctxItem.type == ContextTypeName && name.isEmpty()))
                continue;

            if(ctxItem.subItems.length() > 0)
            {
                QMenu *submenu = new QMenu(ctxItem.display, pContextMenu);
                submenu->addSection(ctxItem.subTitle);

                if(!ctxItem.defaultSub.isNull())
                {
                    QAction *pAction = new QAction("Default", submenu);

                    pAction->connect(pAction, &QAction::triggered, signalMapper, static_cast<void(QSignalMapper::*)()>(&QSignalMapper::map));
                    signalMapper->setMapping(pAction, CreateCommand(ctxItem.defaultSub, "", ctxItem.type, name, steamid));

                    submenu->addAction(pAction);
                }

                CtxSubItem ctxSubItem;

                foreach(ctxSubItem, ctxItem.subItems)
                {
                    QAction *pAction = new QAction(ctxSubItem.display, submenu);

                    pAction->connect(pAction, &QAction::triggered, signalMapper, static_cast<void(QSignalMapper::*)()>(&QSignalMapper::map));
                    signalMapper->setMapping(pAction, CreateCommand(ctxItem.cmd, ctxSubItem.val, ctxItem.type, name, steamid));

                    submenu->addAction(pAction);
                }
                pContextMenu->addMenu(submenu);
            }
            else
            {
                QAction *pAction = new QAction(ctxItem.display, pContextMenu);

                pAction->connect(pAction, &QAction::triggered, signalMapper, static_cast<void(QSignalMapper::*)()>(&QSignalMapper::map));
                signalMapper->setMapping(pAction, CreateCommand(ctxItem.cmd, "", ctxItem.type, name, steamid));

                pContextMenu->addAction(pAction);
            }
        }

        signalMapper->connect(signalMapper, static_cast<void(QSignalMapper::*)(const QString &)>(&QSignalMapper::mapped), this, &MainWindow::playerContextMenuAction);
        pContextMenu->connect(pContextMenu, &QMenu::aboutToHide, this, &MainWindow::hideContextMenu);
        pContextMenu->exec(globalpos);
    }
}
void MainWindow::hideContextMenu()
{
    QObject* obj = sender();
    obj->deleteLater();
}

void MainWindow::playerContextMenuAction(const QString &cmd)
{
    if(this->ui->browserTable->selectedItems().size() == 0)
    {
        this->browserTableItemSelected();
        return;
    }

    QTableWidgetItem *item = this->ui->browserTable->selectedItems().at(0);
    int index = item->text().toInt();

    ServerInfo *info = serverList.at(index-1);

    if(info->rcon == NULL || !info->rcon->isAuthed)
    {
        QList<QueuedCommand>cmds;
        cmds.append(QueuedCommand(cmd, QueuedCommandType::ContextCommand));
        this->rconLoginQueued(cmds);
        return;
    }
    info->rcon->execCommand(cmd, false);
}

void MainWindow::addServerEntry()
{
    QMessageBox message(this);
    while(true)
    {
        bool ok;
        QString server = QInputDialog::getText(this, tr("Add Server"), tr("IP:Port"), QLineEdit::Normal,"", &ok, Qt::WindowSystemMenuHint | Qt::WindowTitleHint);

        if(ok && !server.isEmpty())
        {
            AddServerError error = this->CheckServerList(server);
            if(error == AddServerError_None)
            {
                this->AddServer(server);
                settings->SaveSettings();
                break;
            }
            else if(error == AddServerError_AlreadyExists)//Valid ip but exists.
            {
                message.setText("Server already exists");
                message.exec();
                break;
            }
            else
            {
                message.setText("Invalid IP or Port");
                message.exec();
            }
        }
        else
            break;
    }
}

void MainWindow::browserTableItemSelected()
{
    if(this->ui->browserTable->selectedItems().size() == 0)
    {
        this->SetRconEnabled(false);
        return;
    }

    this->SetRconEnabled(true);

    QTableWidgetItem *item = this->ui->browserTable->selectedItems().at(0);
    int index = item->text().toInt();

    this->SetRconSignals(true);
    this->RestoreRcon(index-1);
    this->SetRconSignals(false);

    this->UpdateSelectedItemInfo(true, true);
}

void MainWindow::darkThemeTriggered()
{
    if(this->ui->actionDark_Theme->isChecked())
    {
        QPalette palette;
        palette.setColor(QPalette::Window, QColor(50,50,50));
        palette.setColor(QPalette::WindowText, Qt::white);
        palette.setColor(QPalette::Base, QColor(60,60,60));
        palette.setColor(QPalette::AlternateBase, QColor(80,80,80));
        palette.setColor(QPalette::ToolTipBase, Qt::white);
        palette.setColor(QPalette::ToolTipText, Qt::white);
        palette.setColor(QPalette::Text, Qt::white);
        palette.setColor(QPalette::Button, QColor(50,50,50));
        palette.setColor(QPalette::ButtonText, Qt::white);
        palette.setColor(QPalette::BrightText, Qt::red);

        palette.setColor(QPalette::Highlight, QColor(80,80,80).lighter());
        palette.setColor(QPalette::HighlightedText, Qt::black);
        qApp->setPalette(palette);
    }
    else
    {
        qApp->setPalette(defaultPalette);
    }

    for(int i = 0; i < this->ui->browserTable->rowCount(); i++)
    {
        if(this->ui->browserTable->item(i, kBrowserColVACIcon))
        {
            QTableWidgetItem *vacItem = new QTableWidgetItem();
            vacItem->setData(Qt::DecorationRole, this->GetVACImage());
            this->ui->browserTable->setItem(i, kBrowserColVACIcon, vacItem);
        }
        if(this->ui->browserTable->item(i, kBrowserColLockIcon))
        {
            QTableWidgetItem *lockedItem = new QTableWidgetItem();
            lockedItem->setData(Qt::DecorationRole, this->GetLockImage());
            this->ui->browserTable->setItem(i, kBrowserColLockIcon, lockedItem);
        }
    }
}

void MainWindow::showPortEntry()
{
    bool ok;
    uint port = QInputDialog::getInt(this, tr("Select Port"), tr("Port Range %1 -> %2").arg(QString::number(PORT_MIN), QString::number(PORT_MAX)), this->u16logPort, PORT_MIN, PORT_MAX, 1, &ok, Qt::WindowSystemMenuHint | Qt::WindowTitleHint);

    if(ok)
    {
        this->u16logPort = port;
        settings->SaveSettings();
    }
}

void MainWindow::showAbout()
{
    QMessageBox message(this);
    message.setTextFormat(Qt::RichText);
    message.setText(
                "Version: 1.0.10\n"
                "Created by Dr!fter @ <a href=\"https://github.com/Drifter321\">https://github.com/Drifter321</a><br>"
                "Using miniupnpc @ <a href=\"https://github.com/miniupnp/miniupnp\">https://github.com/miniupnp/miniupnp</a><br><br>"
                "This product includes GeoLite2 data created by MaxMind, available from<br>"
                "<a href=\"http://www.maxmind.com\">http://www.maxmind.com</a>."
            );
    message.exec();
}

void MainWindow::AddRconHistory(QString chat)
{
    while(this->commandHistory.size() > 30)
        this->commandHistory.removeLast();

    this->commandHistory.prepend(chat);
    this->commandIter->toFront();
}

void MainWindow::AddChatHistory(QString txt)
{
    while(this->sayHistory.size() > 30)
        this->sayHistory.removeLast();

    this->sayHistory.prepend(txt);
    this->sayIter->toFront();
}
