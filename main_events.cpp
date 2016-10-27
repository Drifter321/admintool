#include "ui_mainwindow.h"
#include "mainwindow.h"
#include "settings.h"
#include <QMessageBox>

extern Settings *settings;
extern QList<ServerInfo *> serverList;

void MainWindow::HookEvents()
{
    this->ui->browserTable->installEventFilter(this);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    settings->SaveSettings();
    QMainWindow::closeEvent(event);
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

                ServerInfo *info = serverList.at(index-1);
                pLogHandler->removeServer(info);
                delete info;

                serverList.removeAt(index-1);
                settings->SaveSettings();
                if(this->ui->browserTable->selectedItems().size() == 0)
                {
                   //Clear rules and players
                    while(this->ui->rulesTable->rowCount() > 0)
                    {
                        this->ui->rulesTable->removeRow(0);
                    }

                    while(this->ui->playerTable->rowCount() > 0)
                    {
                        this->ui->playerTable->removeRow(0);
                    }
                }
                return true;
            }
        }
    }
    return QMainWindow::eventFilter(object, event);
}
