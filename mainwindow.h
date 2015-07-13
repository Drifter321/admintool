#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"
#include <QDebug>
#include <QMainWindow>
#include <QSplitter>
#include <QStringList>
#include <QHostAddress>
#include <QTableWidget>
#include <QTimer>
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
    void on_actionDark_Theme_triggered();

private slots:
    void on_actionAdd_Server_triggered();
    void on_browserTable_itemSelectionChanged();
    void TimedUpdate();
    void processCommand();
    void passwordUpdated(const QString &);
    void rconSaveToggled(bool);
    void commandOutputUpdated();
    void rconLogin();

private:
    Ui::MainWindow *ui;
    QTimer *updateTimer;
};

extern QPalette defaultPalette;

#endif // MAINWINDOW_H
