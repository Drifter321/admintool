#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDebug>
#include <QMainWindow>
#include <QSplitter>
#include <QStringList>
#include <QHostAddress>
#include <QTableWidget>
#include <QMutableListIterator>
#include "query.h"
#include "serverinfo.h"
#include "loghandler.h"

enum AddServerError
{
    AddServerError_None,
    AddServerError_Invalid,
    AddServerError_AlreadyExists
};

enum ContextTypes
{
    ContextTypeNone,
    ContextTypeSteamID,
    ContextTypeName
};

struct InfoTableItem
{
    InfoTableItem()
    {
        display =  "";
        val = "";
    }

    InfoTableItem(QString name, QString value)
    {
        display = name;
        val = value;
    }

    QString display;
    QString val;
};

struct CtxSubItem
{
    QString display;
    QString val;
};

struct ContextMenuItem
{
    QString display;
    QString cmd;
    ContextTypes type;
    QString subTitle;
    QString defaultSub;
    QList<CtxSubItem> subItems;
    void clear()
    {
        type = ContextTypeNone;
        display = QString();
        cmd = QString();
        subTitle = QString();
        defaultSub = QString();
        subItems.clear();
    }
};

enum : size_t
{
    kBrowserColIndex,
    kBrowserColModIcon,
    kBrowserColVACIcon,
    kBrowserColLockIcon,
    kBrowserColFlagIcon,
    kBrowserColHostname,
    kBrowserColMap,
    kBrowserColPlayerCount,
    kBrowserColPing,

    kBrowserColCount
};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    AddServerError CheckServerList(QString server);
    void UpdateSelectedItemInfo(bool removeFirst = true, bool updateRules = false);
    void MainWindow::SetTableItemAndDelete(int row, int col, QTableWidgetItem *item);
    Ui::MainWindow *GetUi(){return ui;}
    ~MainWindow();
    void parseLogLine(QString, ServerInfo *);
    PlayerQuery *pPlayerQuery;
    RulesQuery *pRulesQuery;
    quint16 u16logPort;
    bool showLoggingInfo;

protected:
    void closeEvent(QCloseEvent *event);
    bool eventFilter(QObject *object, QEvent *event);

public slots:
    void ServerInfoReady(InfoReply *, QTableWidgetItem *);
    void PlayerInfoReady(QList<PlayerInfo> *, QTableWidgetItem *);
    void RulesInfoReady(QList<RulesInfo> *, QTableWidgetItem *);
    void RconAuthReady(ServerInfo *info, QList<QueuedCommand>queuedcmds);
    void RconOutput(ServerInfo *info, QByteArray res);
    void darkThemeTriggered();
    void showPortEntry();
    void showAbout();
    void AddRconHistory(QString cmd);
    void AddChatHistory(QString txt);

private slots:
    void addServer();
    void browserTableItemSelected();
    void TimedUpdate();
    void processCommand();
    void sendChat();
    void passwordUpdated(const QString &);
    void rconSaveToggled(bool);
    void rconLogin();
    void getLog();
    void showRconClicked(bool checked);
    void customPlayerContextMenu(const QPoint &pos);
    void hideContextMenu();
    void playerContextMenuAction(const QString &cmd);


private:
    Ui::MainWindow *ui;
    QTimer *updateTimer;
    void ConnectSlots();
    void UpdateInfoTable(ServerInfo *info, bool current = true, QList<RulesInfo> *list = NULL);
    void HookEvents();
    void SetRconSignals(bool block);
    void RestoreRcon(int index);
    void SetRconEnabled(bool);
    QImage GetVACImage();
    QImage GetLockImage();
    void runCommand(ServerInfo *, QString);
    void rconLoginQueued(QList<QueuedCommand>);
    LogHandler *pLogHandler;
    QList<QString> commandHistory;
    QList<QString> sayHistory;
    QMutableListIterator<QString> *commandIter;
    QMutableListIterator<QString> *sayIter;
};
#endif // MAINWINDOW_H
