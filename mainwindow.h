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

class ServerTableIndexItem;

enum AddServerError
{
    AddServerError_None,
    AddServerError_Invalid,
    AddServerError_AlreadyExists,
    AddServerError_Hostname,
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

    InfoTableItem(QString name, QString value, bool richValue = false)
    {
        display = name;
        val = value;
        this->richValue = richValue;
    }

    QString display;
    QString val;
    bool richValue = false;
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

enum : uint8_t
{
    kIterInit,
    kIterForwards,
    kIterBackwards
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
    ServerInfo *AddServerToList(QString server, AddServerError *error = nullptr);
    void UpdateSelectedItemInfo(bool removeFirst = true, bool updateRules = false);
    void CreateTableItemOrUpdate(size_t row, size_t col, QTableWidget *table, ServerInfo *info);
    ServerTableIndexItem *GetServerTableIndexItem(size_t row);
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
    void ServerInfoReady(InfoReply *, ServerTableIndexItem *);
    void PlayerInfoReady(QList<PlayerInfo> *, ServerTableIndexItem *);
    void RulesInfoReady(QList<RulesInfo> *, ServerTableIndexItem *);
    void RconAuthReady(ServerInfo *info, QList<QueuedCommand>queuedcmds);
    void RconOutput(ServerInfo *info, QByteArray res);
    void darkThemeTriggered();
    void showPortEntry();
    void showAbout();
    void AddRconHistory(QString cmd);
    void AddChatHistory(QString txt);

private slots:
    void addServerEntry();
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
    void serverBrowserContextMenu(const QPoint &pos);


private:
    Ui::MainWindow *ui;
    QTimer *updateTimer;
    void ConnectSlots();
    void UpdateInfoTable(ServerInfo *info, bool current = true, QList<RulesInfo> *list = NULL);
    void HookEvents();
    void SetRconSignals(bool block);
    void RestoreRcon(ServerInfo *info);
    void SetRconEnabled(bool);
    QImage GetVACImage();
    QImage GetLockImage();
    QColor GetTextColor();
    void runCommand(ServerInfo *, QString);
    void rconLoginQueued(QList<QueuedCommand>);
    bool deleteServerDialog();
    void connectToServer();
    LogHandler *pLogHandler;
    QList<QString> commandHistory;
    QList<QString> sayHistory;
    QMutableListIterator<QString> *commandIter;
    uint8_t commandIterDirection;
    QMutableListIterator<QString> *sayIter;
    uint8_t sayIterDirection;
};
#endif // MAINWINDOW_H
