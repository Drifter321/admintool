#include "ui_mainwindow.h"
#include "mainwindow.h"
#include "serverinfo.h"
#include <QMessageBox>
#include <QRegularExpression>

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

    info->rcon->execCommand("log on");
    info->rcon->execCommand(QString("logaddress_add %1:%2").arg(this->pLogHandler->externalIP.toString(), this->pLogHandler->szPort));
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
        this->ui->logOutput->insertPlainText(logLine);
        this->ui->logOutput->moveCursor(QTextCursor::End);
    }

    while(info->logOutput.size() > 100)
        info->logOutput.removeFirst();

    info->logOutput.append(logLine);

    //Check if it is a chat event, display and save if so
    QStringList captures = chatRegex.match(logLine).capturedTexts();

    if(captures.length() == 7 && captures.at(3) != "Console")//We have 6, 0 = whole line. Ignore console say messages.
    {
        QString start = "";
        if(captures.at(5) == "say_team")
            start = QString("(TEAM %1) ").arg(captures.at(4));

        QString chatLine = QString("%1%2<%3> : %4\n").arg(start, captures.at(1), captures.at(3), captures.at(6));

        if(info == serverList.at(index-1))
        {
            this->ui->chatOutput->insertPlainText(chatLine);
            this->ui->chatOutput->moveCursor(QTextCursor::End);
        }

        while(info->chatOutput.size() > 100)
            info->chatOutput.removeFirst();

        info->chatOutput.append(chatLine);
    }
}
