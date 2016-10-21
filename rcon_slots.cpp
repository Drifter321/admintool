#include "ui_mainwindow.h"
#include "mainwindow.h"
#include "rcon.h"
#include "settings.h"
#include <QMessageBox>

extern QList<ServerInfo *> serverList;

//RCON HANDLING
void MainWindow::processCommand()
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
        this->browserTableItemSelected();
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
        this->browserTableItemSelected();
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
        this->browserTableItemSelected();
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
        this->browserTableItemSelected();
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
    if(info->rconPassword == 0)
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
        this->browserTableItemSelected();
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
        this->browserTableItemSelected();
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

void MainWindow::SetRconSignals(bool block)
{
    this->ui->rconSave->blockSignals(block);
    this->ui->rconPassword->blockSignals(block);
    this->ui->commandOutput->blockSignals(block);
}

void MainWindow::RestoreRcon(int index)
{
    this->ui->rconSave->setChecked(serverList.at(index)->saveRcon);
    this->ui->rconPassword->setText(serverList.at(index)->rconPassword);
    this->ui->commandOutput->setPlainText(serverList.at(index)->rconOutput);
    this->ui->logOutput->setPlainText(serverList.at(index)->logOutput);
    this->ui->logOutput->moveCursor(QTextCursor::End);
    this->ui->commandOutput->moveCursor(QTextCursor::End);
}

void MainWindow::SetRconEnabled(bool enabled)
{
    this->ui->rconSave->setEnabled(enabled);
    this->ui->rconPassword->setEnabled(enabled);
    this->ui->commandOutput->setEnabled(enabled);
    this->ui->commandText->setEnabled(enabled);
}
