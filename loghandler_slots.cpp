#include "ui_mainwindow.h"
#include "mainwindow.h"
#include "serverinfo.h"
#include <QMessageBox>
#include <QRegularExpression>
#include <QScrollBar>

extern QList<ServerInfo *> serverList;

QStringList blueTeams = {"BLU", "CT"};
QStringList redTeams = {"RED", "TERRORIST"};

QRegularExpression chatRegex("^L\\d{2}\\/\\d{2}\\/\\d{4} - \\d{2}:\\d{2}:\\d{2}: \"(.+)<(\\d+)><([^>]+)><([^>]*)>\"(?: (say(?:_team)?) \"(.*)\")?.*?$");
QRegularExpression actionRegex("^L\\d{2}\\/\\d{2}\\/\\d{4} - \\d{2}:\\d{2}:\\d{2}: \"(.+)<(\\d+)><([^>]+)><([^>]*)>\".*?\"(.+)<(\\d+)><([^>]+)><([^>]*)>\".*?$");

void MainWindow::getLog()
{
    ServerTableIndexItem *item = this->GetServerTableIndexItem(this->ui->browserTable->currentRow());
    ServerInfo *info = item->GetServerInfo();

    if(!info)
        return;//WHAT?!?!

    if(this->pLogHandler->externalIP.isNull())
    {
        QMessageBox::critical(this, "Log Handler Error", "Failed to get external ip. Logging can not be enabled.");
        return;
    }
    else if(!info->rcon || !info->rcon->isAuthed)
    {
        QList<QueuedCommand>cmds;
        cmds.append(QueuedCommand("log on", QueuedCommandType::GetLogCommand));
        cmds.append(QueuedCommand(QString("logaddress_add %1:%2").arg(this->pLogHandler->externalIP.toString(), this->pLogHandler->szPort), QueuedCommandType::GetLogCommand));
        this->rconLoginQueued(cmds);
        return;
    }

    info->rcon->execCommand("log on", false);
    info->rcon->execCommand(QString("logaddress_add %1:%2").arg(this->pLogHandler->externalIP.toString(), this->pLogHandler->szPort), false);
    pLogHandler->addServer(info);
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

    ServerTableIndexItem *item = this->GetServerTableIndexItem(this->ui->browserTable->currentRow());
    ServerInfo *currentInfo = item->GetServerInfo();

    //Show and save the log line in the log tab
    if(info == currentInfo)
    {
        int sliderPos = this->ui->logOutput->verticalScrollBar()->sliderPosition();
        bool shouldAutoScroll = sliderPos == this->ui->logOutput->verticalScrollBar()->maximum();

        this->ui->logOutput->moveCursor(QTextCursor::End);
        this->ui->logOutput->insertPlainText(logLine);
        this->ui->logOutput->moveCursor(QTextCursor::End);

        if(!shouldAutoScroll)
            this->ui->logOutput->verticalScrollBar()->setSliderPosition(sliderPos);
        else
           this->ui->logOutput->verticalScrollBar()->setSliderPosition(this->ui->logOutput->verticalScrollBar()->maximum());
    }

    while(info->logOutput.size() > 100)
        info->logOutput.removeFirst();

    info->logOutput.append(logLine);

    //Check if it is a chat event, display and save if so
    QStringList captures = actionRegex.match(logLine).capturedTexts();

    if(captures.length() == 9) //Player action 2 players
    {
        info->logHashTable.insert(captures.at(1), PlayerLogInfo(captures.at(2).toUInt(), captures.at(3)));
        info->logHashTable.insert(captures.at(5), PlayerLogInfo(captures.at(6).toUInt(), captures.at(7)));
    }
    else
    {
        captures = chatRegex.match(logLine).capturedTexts();

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

            if(info == currentInfo)
            {
                int sliderPos = this->ui->chatOutput->verticalScrollBar()->sliderPosition();
                bool shouldAutoScroll = sliderPos == this->ui->chatOutput->verticalScrollBar()->maximum();

                this->ui->chatOutput->moveCursor(QTextCursor::End);
                this->ui->chatOutput->insertHtml(chatLine);
                this->ui->chatOutput->moveCursor(QTextCursor::End);

                if(!shouldAutoScroll)
                    this->ui->chatOutput->verticalScrollBar()->setSliderPosition(sliderPos);
                else
                   this->ui->chatOutput->verticalScrollBar()->setSliderPosition(this->ui->chatOutput->verticalScrollBar()->maximum());
            }

            while(info->chatOutput.size() > 100)
                info->chatOutput.removeFirst();

            info->chatOutput.append(chatLine);
        }

        if(captures.length() >= 5)
        {
            if(captures.at(3) != "Console")
            {
                info->logHashTable.insert(captures.at(1), PlayerLogInfo(captures.at(2).toUInt(), captures.at(3)));
            }
        }
    }
}

void MainWindow::sendChat()
{
    if(this->ui->browserTable->selectedItems().size() == 0)
    {
        this->browserTableItemSelected();
        return;
    }

    ServerTableIndexItem *item = this->GetServerTableIndexItem(this->ui->browserTable->currentRow());
    ServerInfo *info = item->GetServerInfo();

    if(info && (info->rcon == NULL || !info->rcon->isAuthed))
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
