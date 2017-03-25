#include "ui_mainwindow.h"
#include "mainwindow.h"
#include "settings.h"
#include <QMessageBox>
#include <QClipboard>

extern Settings *settings;
extern QList<ServerInfo *> serverList;

void MainWindow::HookEvents()
{
    this->ui->browserTable->installEventFilter(this);
    this->ui->commandText->installEventFilter(this);
    this->ui->sendChat->installEventFilter(this);
    this->ui->playerTable->installEventFilter(this);
    this->ui->infoTable->installEventFilter(this);
    this->ui->rulesTable->installEventFilter(this);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    settings->SaveSettings();
    QMainWindow::closeEvent(event);
}

bool MainWindow::eventFilter(QObject *object, QEvent *event)
{
    if((object == this->ui->commandText || object == this->ui->sendChat) && event->type() == QEvent::KeyPress)
    {
        Qt::Key key = (Qt::Key)(((QKeyEvent *)event)->key());
        if(key == Qt::Key_Up)
        {
            if(object == this->ui->commandText && this->commandHistory.size() > 0)
            {
                if(!this->commandIter->hasNext())
                {
                    this->commandIter->toFront();
                }
                this->ui->commandText->setText(this->commandIter->next());
            }
            else if(object == this->ui->sendChat && this->sayHistory.size() > 0)
            {
                if(!this->sayIter->hasNext())
                {
                    this->sayIter->toFront();
                }
                this->ui->sendChat->setText(this->sayIter->next());
            }
            return true;
        }
        else if(key == Qt::Key_Down)
        {
            if(object == this->ui->commandText && this->commandHistory.size() > 0)
            {
                if(!this->commandIter->hasPrevious())
                {
                    this->commandIter->toBack();
                }
                this->ui->commandText->setText(this->commandIter->previous());
            }
            else if(object == this->ui->sendChat && this->sayHistory.size() > 0)
            {
                if(!this->sayIter->hasPrevious())
                {
                    this->sayIter->toBack();
                }
                this->ui->sendChat->setText(this->sayIter->previous());
            }
            return true;
        }
    }
    else if(object == this->ui->browserTable && this->ui->browserTable->selectedItems().size() && event->type() == QEvent::KeyPress)
    {
        Qt::Key key = (Qt::Key)(((QKeyEvent *)event)->key());
        if(key == Qt::Key_Delete)
        {
            if(this->deleteServerDialog())
                return true;
        }
    }
    else if((object == this->ui->playerTable || object == this->ui->infoTable || object == this->ui->rulesTable) && event->type() == QEvent::KeyPress)
    {
        QKeyEvent *key_event = (QKeyEvent *)event;

        if(key_event->matches(QKeySequence::Copy))
        {
            QList<QTableWidgetItem *>selectedItems;

            if(object == this->ui->playerTable)
            {
                selectedItems = this->ui->playerTable->selectedItems();
            }
            else if(object == this->ui->infoTable)
            {
               selectedItems = this->ui->infoTable->selectedItems();
            }
            else if(object == this->ui->rulesTable)
            {
                selectedItems = this->ui->rulesTable->selectedItems();
            }

            if(selectedItems.size() == 0)
                return QMainWindow::eventFilter(object, event);

            QClipboard *clipboard = QApplication::clipboard();

            QTableWidgetItem *item;
            QString copy;

            int row = -1;

            foreach(item, selectedItems)
            {
                if(row == -1)
                {
                    row = item->row();
                }
                else if(item->row() == row)
                {
                    copy.append('\t');
                }
                else
                {
                    copy.append('\n');
                    row = item->row();
                }
                copy.append(item->text());
            }
            clipboard->setText(copy);
            return true;
        }
    }
    return QMainWindow::eventFilter(object, event);
}
