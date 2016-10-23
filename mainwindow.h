#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDebug>
#include <QMainWindow>
#include <QSplitter>
#include <QStringList>
#include <QHostAddress>
#include <QTableWidget>
#include "query.h"
#include "serverinfo.h"
#include "loghandler.h"

enum AddServerError
{
    AddServerError_None,
    AddServerError_Invalid,
    AddServerError_AlreadyExists
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
    Ui::MainWindow *GetUi(){return ui;}
    ~MainWindow();
    void parseLogLine(QString, ServerInfo *);
    PlayerQuery *pPlayerQuery;
    RulesQuery *pRulesQuery;
    quint16 u16logPort;

protected:
    void closeEvent(QCloseEvent *event);
    bool eventFilter(QObject *object, QEvent *event);

public slots:
    void ServerInfoReady(ServerInfo *, InfoReply *, QTableWidgetItem *);
    void PlayerInfoReady(QList<PlayerInfo> *, QTableWidgetItem *);
    void RulesInfoReady(QList<RulesInfo> *, QTableWidgetItem *);
    void RconAuthReady(ServerInfo *info);
    void RconOutput(ServerInfo *info, QByteArray res);
    void darkThemeTriggered();
    void showPortEntry();

private slots:
    void addServer();
    void browserTableItemSelected();
    void TimedUpdate();
    void processCommand();
    void passwordUpdated(const QString &);
    void rconSaveToggled(bool);
    void rconLogin();
    void getLog();

private:
    Ui::MainWindow *ui;
    QTimer *updateTimer;
    void ConnectSlots();
    void HookEvents();
    void SetRconSignals(bool block);
    void RestoreRcon(int index);
    void SetRconEnabled(bool);
    QImage GetVACImage();
    QImage GetLockImage();
    LogHandler *pLogHandler;
};
#endif // MAINWINDOW_H
