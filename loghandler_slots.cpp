#include "ui_mainwindow.h"
#include "mainwindow.h"
#include "serverinfo.h"
#include <QMessageBox>
#include <QRegularExpression>

QStringList blueTeams = {"BLU", "CT"};
QStringList redTeams = {"RED", "TERRORIST"};
extern QList<ServerInfo *> serverList;
QRegularExpression chatRegex("^L\\d{2}\\/\\d{2}\\/\\d{4} - \\d{2}:\\d{2}:\\d{2}: \"(.+)<(\\d+)><([^>]+)><([^>]*)>\"(?: (say(?:_team)?) \"(.*)\")?.*?$");

void MainWindow::getLog()
{
    int index = this->ui->browserTable->selectedItems().at(0)->text().toInt();
    ServerInfo *info = serverList.at(index-1);

    if(!info)
        return;//WHAT?!?!

    if(!info->rcon || !info->rcon->isAuthed)
    {
        QMessageBox::information(this, "Log Handler Error", "Please authenticate over RCon first.");
        return;
    }
    else if(this->pLogHandler->externalIP.isNull())
    {
        QMessageBox::critical(this, "Log Handler Error", "Failed to get external ip. Logging can not be enabled.");
    }

    info->rcon->execCommand("log on", false);
    info->rcon->execCommand(QString("logaddress_add %1:%2").arg(this->pLogHandler->externalIP.toString(), this->pLogHandler->szPort), false);
    pLogHandler->addServer(serverList.at(index-1));
}

void MainWindow::parseLogLine(QString line, ServerInfo *info)
{
    if(!info)
        return;

    if(this->ui->browserTable->selectedItems().size() == 0)
    {
        this->browserTableItemSelected();
        return;
    }

    if(line.length() == 0)
    {
        return;
    }

    QString logLine = QString("L%1").arg(line);

    QTableWidgetItem *item = this->ui->browserTable->selectedItems().at(0);
    int index = item->text().toInt();

    //Show and save the log line in the log tab
    if(info == serverList.at(index-1))
    {
        this->ui->logOutput->moveCursor(QTextCursor::End);
        this->ui->logOutput->insertPlainText(logLine);
        this->ui->logOutput->moveCursor(QTextCursor::End);
    }

    while(info->logOutput.size() > 100)
        info->logOutput.removeFirst();

    info->logOutput.append(logLine);

    //Check if it is a chat event, display and save if so
    QStringList captures = chatRegex.match(logLine).capturedTexts();

    if(captures.length() == 7)//We have 6, 0 = whole line. Ignore console say messages.
    {
        QString chatLine;
        if(captures.at(3) != "Console")
        {
            QString start = "<font>";
            if(captures.at(5) == "say_team")
            {
                QString team = captures.at(4);
                if(blueTeams.contains(team))
                {
                    start = QString("<font style='color:#32a0f0;'>(%1 TEAM) ").arg(captures.at(4));
                }
                else if(redTeams.contains(team))
                {
                    start = QString("<font style='color:#ff333a;'>(%1 TEAM) ").arg(captures.at(4));
                }
                else
                {
                    start = QString("<font>(%1 TEAM) ").arg(captures.at(4));
                }
            }

            chatLine = QString("%1%2&lt;%3&gt; : %4</font><br>").arg(start, captures.at(1).toHtmlEscaped(), captures.at(3), captures.at(6).toHtmlEscaped());
        }
        else
        {
           chatLine = QString("&lt;%1&gt;&lt;%1&gt; : %2<br>").arg(captures.at(3), captures.at(6));
        }

        if(info == serverList.at(index-1))
        {
            this->ui->chatOutput->insertHtml(chatLine);
            this->ui->chatOutput->moveCursor(QTextCursor::End);
        }

        while(info->chatOutput.size() > 100)
            info->chatOutput.removeFirst();

        info->chatOutput.append(chatLine);
    }
}

void MainWindow::sendChat()
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
        QMessageBox message(this);
        message.setText("Please authenticate first.");
        message.exec();
        return;
    }

    if(this->ui->sendChat->text().length() != 0)
    {
        this->AddChatHistory(this->ui->sendChat->text());
        info->rcon->execCommand(QString("say %1").arg(this->ui->sendChat->text()), false);
        this->ui->sendChat->setText("");
    }
}
