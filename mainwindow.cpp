#include "mainwindow.h"
#include "query.h"
#include "serverinfo.h"
#include "settings.h"
#include "worker.h"
#include "customitems.h"
#include "rcon.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QDateTime>
#include <QKeyEvent>
#include <QMap>
#include <QTcpSocket>

QList<ServerInfo *> serverList;
QMap<int, QString> appIDMap;
Settings *settings;

#define UPDATE_TIME 15

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    this->setHidden(true);
    pPlayerQuery = NULL;
    pRulesQuery = NULL;
    ui->setupUi(this);
    this->setWindowTitle("Source Admin Tool");

    settings = new Settings(this);
    settings->SetDefaultSettings();
    settings->ReadSettings();

    this->ui->browserTable->installEventFilter(this);
    this->ui->commandText->connect(this->ui->commandText, &QLineEdit::returnPressed, this, &MainWindow::processCommand);
    this->ui->rconPassword->connect(this->ui->rconPassword, &QLineEdit::returnPressed, this, &MainWindow::rconLogin);
    this->ui->rconPassword->connect(this->ui->rconPassword, &QLineEdit::textChanged, this, &MainWindow::passwordUpdated);
    this->ui->rconSave->connect(this->ui->rconSave, &QCheckBox::toggled, this, &MainWindow::rconSaveToggled);
    this->ui->commandText->connect(this->ui->commandOutput, &QPlainTextEdit::textChanged, this, &MainWindow::commandOutputUpdated);
    this->ui->rconLogin->connect(this->ui->rconLogin, &QPushButton::released, this, &MainWindow::rconLogin);

    this->updateTimer = new QTimer(this);
    connect(this->updateTimer, &QTimer::timeout, this, &MainWindow::TimedUpdate);
    this->updateTimer->start(1000);
}

MainWindow::~MainWindow()
{
    delete settings;
    serverList.clear();
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    settings->SaveSettings();
    QMainWindow::closeEvent(event);
}

//SERVER EVENT HANDLERS
AddServerError MainWindow::CheckServerList(QString server)
{
    ServerInfo info(server);

    if(!info.isValid)
        return AddServerError_Invalid;

    for(int i = 0; i < serverList.size(); i++)
    {
        if(serverList.at(i)->isEqual(info))
        {
            return AddServerError_AlreadyExists;
        }
    }

    return AddServerError_None;
}

void MainWindow::on_actionAdd_Server_triggered()
{
    QMessageBox message(this);
    while(true)
    {
        bool ok;
        QString server = QInputDialog::getText(this, tr("Add Server"), tr("IP:Port"), QLineEdit::Normal,"", &ok);

        if (ok && !server.isEmpty())
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

                item->setTextColor(QColor(80, 170, 80));
                item->setText(QString("Querying server %1...").arg(server));
                this->ui->browserTable->setItem(row, 0, id);
                this->ui->browserTable->setItem(row, 4, item);

                InfoQuery *infoQuery = new InfoQuery(this);
                infoQuery->query(info, id);
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

void MainWindow::on_browserTable_itemSelectionChanged()
{
    if(this->ui->browserTable->selectedItems().size() == 0)
    {
        this->ui->rconPassword->setEnabled(false);
        this->ui->commandText->setEnabled(false);
        this->ui->commandOutput->setEnabled(false);
        this->ui->rconSave->setEnabled(false);
        this->ui->rconSave->setChecked(false);
        this->ui->rconLogin->setEnabled(false);
        return;
    }

    this->ui->rconPassword->setEnabled(true);
    this->ui->commandText->setEnabled(true);
    this->ui->commandOutput->setEnabled(true);
    this->ui->rconSave->setEnabled(true);
    this->ui->rconLogin->setEnabled(true);

    QTableWidgetItem *item = this->ui->browserTable->selectedItems().at(0);
    int index = item->text().toInt();

    this->ui->rconSave->blockSignals(true);
    this->ui->rconPassword->blockSignals(true);
    this->ui->commandOutput->blockSignals(true);
    this->ui->rconSave->setChecked(serverList.at(index-1)->saveRcon);
    this->ui->rconPassword->setText(serverList.at(index -1)->rconPassword);
    this->ui->commandOutput->setPlainText(serverList.at(index-1)->rconOutput);
    this->ui->commandOutput->moveCursor(QTextCursor::End);
    this->ui->rconSave->blockSignals(false);
    this->ui->rconPassword->blockSignals(false);
    this->ui->commandOutput->blockSignals(false);

    this->UpdateSelectedItemInfo(true, true);
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

bool MainWindow::eventFilter(QObject *object, QEvent *event)
{
    if(object == this->ui->browserTable && this->ui->browserTable->selectedItems().size() && event->type() == QEvent::KeyPress)
    {
        Qt::Key key = (Qt::Key)(((QKeyEvent *)event)->key());
        if(key == Qt::Key_Delete)
        {
            int index = this->ui->browserTable->selectedItems().at(0)->text().toInt();

            QMessageBox message(this);
            message.setInformativeText(QString("Delete %1?").arg(serverList.at(index-1)->ipPort));
            message.setText("Delete server from list?");
            message.setStandardButtons(QMessageBox::Ok|QMessageBox::Cancel);
            message.setDefaultButton(QMessageBox::Cancel);
            int ret = message.exec();

            if(ret == QMessageBox::Ok)
            {
                for(int i = 0; i < this->ui->browserTable->rowCount(); i++)
                {
                    QTableWidgetItem *item = this->ui->browserTable->item(i, 0);

                    int other = item->data(Qt::DisplayRole).toInt();

                    if(other == index)
                    {
                        this->ui->browserTable->removeRow(i);
                        settings->beginGroup("servers");
                        settings->remove(serverList.at(index-1)->ipPort);
                        settings->endGroup();
                        break;
                    }
                }

                for(int i = 0; i < this->ui->browserTable->rowCount(); i++)
                {
                    QTableWidgetItem *item = this->ui->browserTable->item(i, 0);

                    int other = item->data(Qt::DisplayRole).toInt();

                    if(other > index)
                    {
                        item->setData(Qt::DisplayRole, other-1);
                    }
                }
                serverList.removeAt(index-1);
                settings->SaveSettings();
                return true;
            }
        }
    }
    return QMainWindow::eventFilter(object, event);
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

        QImage vacImage;
        if(reply->vac)
            vacImage.load(QString(":/icons/icons/vac.png"));

        QTableWidgetItem *vacItem = new QTableWidgetItem();
        vacItem->setData(Qt::DecorationRole, vacImage);

        this->ui->browserTable->setItem(row, 2, vacItem);

        QImage lockImage;
        if(reply->visibility)
            lockImage.load(QString(":/icons/icons/lock.png"));

        QTableWidgetItem *privateItem = new QTableWidgetItem();
        privateItem->setData(Qt::DecorationRole, lockImage);

        this->ui->browserTable->setItem(row, 3, privateItem);

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

        item->setTextColor(QColor(255, 60, 60));
        item->setText(QString("Failed to query %1, retrying in %2 seconds").arg(info->ipPort, QString::number(UPDATE_TIME)));
        this->ui->browserTable->setItem(row, 4, item);
    }
}

void MainWindow::PlayerInfoReady(QList<PlayerInfo> *list, QTableWidgetItem *indexCell)
{
    if(this->ui->browserTable->selectedItems().at(0) != indexCell)
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
    if(this->ui->browserTable->selectedItems().at(0) != indexCell)
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

//RCON HANDLING
void MainWindow::processCommand()
{
    if(this->ui->browserTable->selectedItems().size() == 0)
    {
        this->on_browserTable_itemSelectionChanged();
        return;
    }

    QTableWidgetItem *item = this->ui->browserTable->selectedItems().at(0);
    int index = item->text().toInt();

    ServerInfo *info = serverList.at(index-1);

    if(info->rcon == NULL || !info->rcon->isAuthed)
    {
        QMessageBox message(this);
        message.setText("Please authenticate first.");
        message.exec();
        return;
    }

    if(this->ui->commandText->text().length() != 0)
    {
        if(this->ui->commandText->text() == "clear")
        {
            this->ui->commandOutput->clear();
            this->ui->commandOutput->appendPlainText(QString("] %1").arg(this->ui->commandText->text()));
            this->ui->commandOutput->moveCursor(QTextCursor::End);
            info->rcon->execCommand(this->ui->commandText->text());
            this->ui->commandText->setText("");

            return;
        }
        this->ui->commandOutput->appendPlainText(QString("] %1").arg(this->ui->commandText->text()));
        this->ui->commandOutput->appendPlainText("");
        this->ui->commandOutput->moveCursor(QTextCursor::End);
        info->rcon->execCommand(this->ui->commandText->text());
        this->ui->commandText->setText("");
    }
}

void MainWindow::rconSaveToggled(bool checked)
{
    if(this->ui->browserTable->selectedItems().size() == 0)
    {
        this->on_browserTable_itemSelectionChanged();
        return;
    }

    QTableWidgetItem *item = this->ui->browserTable->selectedItems().at(0);
    int index = item->text().toInt();

    serverList.at(index-1)->saveRcon = checked;
}

void MainWindow::passwordUpdated(const QString &text)
{
    if(this->ui->browserTable->selectedItems().size() == 0)
    {
        this->on_browserTable_itemSelectionChanged();
        return;
    }

    QTableWidgetItem *item = this->ui->browserTable->selectedItems().at(0);
    int index = item->text().toInt();

    serverList.at(index-1)->rconPassword = text;
}

void MainWindow::commandOutputUpdated()
{ 
    if(this->ui->browserTable->selectedItems().size() == 0)
    {
        this->on_browserTable_itemSelectionChanged();
        return;
    }

    QTableWidgetItem *item = this->ui->browserTable->selectedItems().at(0);
    int index = item->text().toInt();

    serverList.at(index-1)->rconOutput = this->ui->commandOutput->toPlainText();
}

void MainWindow::rconLogin()
{
    if(this->ui->browserTable->selectedItems().size() == 0)
    {
        this->on_browserTable_itemSelectionChanged();
        return;
    }

    QTableWidgetItem *item = this->ui->browserTable->selectedItems().at(0);
    int index = item->text().toInt();

    ServerInfo *info = serverList.at(index-1);

    if(info->rcon == NULL)
    {
        info->rcon = new RconQuery(this, info);
    }
    else if(info->rcon->isAuthed)
    {
        QMessageBox message(this);
        message.setText("Already authenticated");
        message.exec();
        return;
    }
    else if(info->rconPassword == 0)
    {
        QMessageBox message(this);
        message.setText("Please enter a password");
        message.exec();
        return;
    }
    info->rcon->auth();
}

void MainWindow::RconAuthReady(ServerInfo *info)
{
    if(this->ui->browserTable->selectedItems().size() == 0)
    {
        this->on_browserTable_itemSelectionChanged();
        return;
    }

    QTableWidgetItem *item = this->ui->browserTable->selectedItems().at(0);
    int index = item->text().toInt();

    if(!info->rcon->isAuthed)
    {
        QMessageBox message(this);
        message.setText(QString("Failed to authenticate %1").arg(info->ipPort));
        message.exec();
        return;
    }
    else if(info == serverList.at(index-1))
    {
        info->rcon->execCommand("echo Welcome user!");
    }
}

void MainWindow::RconOutput(ServerInfo *info, QByteArray result)
{
    if(this->ui->browserTable->selectedItems().size() == 0)
    {
        this->on_browserTable_itemSelectionChanged();
        return;
    }

    if(result.length() == 0)
    {
        return;
    }

    QTableWidgetItem *item = this->ui->browserTable->selectedItems().at(0);
    int index = item->text().toInt();

    if(info == serverList.at(index-1))
    {
        this->ui->commandOutput->insertPlainText(result);
        this->ui->commandOutput->moveCursor(QTextCursor::End);
    }
    else if(info)
    {
        info->rconOutput.append(result);
    }
}

void MainWindow::on_actionDark_Theme_triggered()
{
    if(this->ui->menuTheme->actions().at(0)->isChecked())
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
}
