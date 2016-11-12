#include "ui_mainwindow.h"
#include "mainwindow.h"
#include "worker.h"
#include "query.h"
#include "serverinfo.h"
#include "settings.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QPalette>

extern QPalette defaultPalette;
extern QMap<int, QString> appIDMap;
extern Settings *settings;
extern QList<ServerInfo *> serverList;
extern QColor errorColor;
extern QColor queryingColor;

void MainWindow::ConnectSlots()
{
    this->ui->commandText->connect(this->ui->commandText, &QLineEdit::returnPressed, this, &MainWindow::processCommand);
    this->ui->commandText->connect(this->ui->sendChat, &QLineEdit::returnPressed, this, &MainWindow::sendChat);
    this->ui->rconPassword->connect(this->ui->rconPassword, &QLineEdit::returnPressed, this, &MainWindow::rconLogin);
    this->ui->rconPassword->connect(this->ui->rconPassword, &QLineEdit::textChanged, this, &MainWindow::passwordUpdated);
    this->ui->rconSave->connect(this->ui->rconSave, &QCheckBox::toggled, this, &MainWindow::rconSaveToggled);
    this->ui->rconLogin->connect(this->ui->rconLogin, &QPushButton::released, this, &MainWindow::rconLogin);
    this->ui->logGetLog->connect(this->ui->logGetLog, &QPushButton::released, this, &MainWindow::getLog);
    this->ui->actionAdd_Server->connect(this->ui->actionAdd_Server, &QAction::triggered, this, &MainWindow::addServer);
    this->ui->actionDark_Theme->connect(this->ui->actionDark_Theme, &QAction::triggered, this, &MainWindow::darkThemeTriggered);
    this->ui->actionSet_Log_Port->connect(this->ui->actionSet_Log_Port, &QAction::triggered, this, &MainWindow::showPortEntry);
    this->ui->actionAbout->connect(this->ui->actionAbout, &QAction::triggered, this, &MainWindow::showAbout);
    this->ui->browserTable->connect(this->ui->browserTable, &QTableWidget::itemSelectionChanged, this, &MainWindow::browserTableItemSelected);
}


void MainWindow::addServer()
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
                ServerInfo *info = new ServerInfo(server);
                serverList.append(info);

                settings->SaveSettings();

                int row = this->ui->browserTable->rowCount();
                this->ui->browserTable->insertRow(row);

                QTableWidgetItem *item = new QTableWidgetItem();
                QTableWidgetItem *id = new QTableWidgetItem();
                id->setData(Qt::DisplayRole, row+1);

                item->setTextColor(queryingColor);
                item->setText(QString("Querying server %1...").arg(server));
                item->setToolTip(server);
                this->ui->browserTable->setItem(row, 0, id);
                this->ui->browserTable->setItem(row, 4, item);

                InfoQuery *infoQuery = new InfoQuery(this);
                infoQuery->query(&info->host, info->port, id);
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
        if(this->ui->browserTable->item(i, 2))
        {
            QTableWidgetItem *vacItem = new QTableWidgetItem();
            vacItem->setData(Qt::DecorationRole, this->GetVACImage());
            this->ui->browserTable->setItem(i, 2, vacItem);
        }
        if(this->ui->browserTable->item(i, 3))
        {
            QTableWidgetItem *lockedItem = new QTableWidgetItem();
            lockedItem->setData(Qt::DecorationRole, this->GetLockImage());
            this->ui->browserTable->setItem(i, 3, lockedItem);
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
    message.setText("Version: 1.0.6\nCreated by Dr!fter @ https://github.com/Drifter321\nUsing miniupnpc @ https://github.com/miniupnp/miniupnp");
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
