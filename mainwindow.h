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
    PlayerQuery *pPlayerQuery;
    RulesQuery *pRulesQuery;

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

private slots:
    void addServer();
    void browserTableItemSelected();
    void TimedUpdate();
    void processCommand();
    void passwordUpdated(const QString &);
    void rconSaveToggled(bool);
    void commandOutputUpdated();
    void rconLogin();

private:
    Ui::MainWindow *ui;
    QTimer *updateTimer;
    void ConnectSlots();
    void HookEvents();
};
#endif // MAINWINDOW_H
