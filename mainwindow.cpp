#include "ui_mainwindow.h"
#include "mainwindow.h"
#include "settings.h"
#include <QKeyEvent>
#include <QTimer>

extern Settings *settings;
extern QList<ServerInfo *> serverList;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    pPlayerQuery = NULL;
    pRulesQuery = NULL;
    ui->setupUi(this);
    this->setWindowTitle("Source Admin Tool");

    this->SetRconEnabled(false);
    settings = new Settings(this);
    settings->SetDefaultSettings();
    settings->ReadSettings();

    if(this->ui->browserTable->rowCount() > 0)
    {
        this->ui->browserTable->selectRow(0);
        this->SetRconEnabled(true);
        this->SetRconSignals(true);
        this->RestoreRcon(0);
        this->SetRconSignals(false);
    }

    this->HookEvents();
    this->ConnectSlots();

    this->updateTimer = new QTimer(this);
    connect(this->updateTimer, &QTimer::timeout, this, &MainWindow::TimedUpdate);
    this->updateTimer->start(1000);
}

MainWindow::~MainWindow()
{
    delete settings;
    serverList.clear();
    delete ui;
}

//SERVER EVENT HANDLERS
AddServerError MainWindow::CheckServerList(QString server)
{
    ServerInfo info(server);

    if(!info.isValid)
        return AddServerError_Invalid;

    for(int i = 0; i < serverList.size(); i++)
    {
        if(serverList.at(i)->isEqual(info))
        {
            return AddServerError_AlreadyExists;
        }
    }

    return AddServerError_None;
}
