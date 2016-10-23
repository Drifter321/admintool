#include "ui_mainwindow.h"
#include "mainwindow.h"
#include "serverinfo.h"
#include <QMessageBox>

extern QList<ServerInfo *> serverList;

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

    QTableWidgetItem *item = this->ui->browserTable->selectedItems().at(0);
    int index = item->text().toInt();

    if(info == serverList.at(index-1))
    {
        this->ui->logOutput->insertPlainText(QString("L%1").arg(line));
        this->ui->logOutput->moveCursor(QTextCursor::End);
    }

    if(!line.isEmpty())
    {
        while(info->logOutput.size() > 100)
            info->logOutput.removeFirst();

        info->logOutput.append(QString("L%1").arg(line));
    }
}
