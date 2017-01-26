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
    setWindowTitle("Source Admin Tool");
    commandIter = new QMutableListIterator<QString>(this->commandHistory);
    sayIter = new QMutableListIterator<QString>(this->sayHistory);

    this->SetRconEnabled(false);
    settings = new Settings(this);
    settings->SetDefaultSettings();
    settings->ReadSettings();
    settings->GetCtxCommands();
    pLogHandler = new LogHandler(this);
    pLogHandler->createBind(u16logPort);

    if(this->ui->browserTable->rowCount() > 0)
    {
        this->ui->browserTable->selectRow(0);
        this->browserTableItemSelected();
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
    delete pLogHandler;
    delete sayIter;
    delete commandIter;
}

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

ServerInfo *MainWindow::AddServer(QString server)
{
    ServerInfo *info = new ServerInfo(server);

    if(!info)
        return nullptr;

    serverList.append(info);

    int row = ui->browserTable->rowCount();
    ui->browserTable->insertRow(row);

    ServerTableIndexItem *id = new ServerTableIndexItem(info);
    id->setData(Qt::DisplayRole, serverList.size());
    ui->browserTable->setItem(row, kBrowserColIndex, id);

    this->CreateTableItemOrUpdate(row, kBrowserColHostname, ui->browserTable, info);

    InfoQuery *infoQuery = new InfoQuery(this);
    infoQuery->query(&info->host, info->port, id);

    return info;
}

QImage MainWindow::GetVACImage()
{
    if(this->ui->actionDark_Theme->isChecked())
    {
        static QImage vacDark(":/icons/icons/vac.png");
        return vacDark;
    }
    else
    {
        static QImage vacLight(":/icons/icons/vac-light.png");
        return vacLight;
    }
}

QImage MainWindow::GetLockImage()
{
    if(this->ui->actionDark_Theme->isChecked())
    {
        static QImage lockDark(":/icons/icons/lock.png");
        return lockDark;
    }
    else
    {
        static QImage lockLight(":/icons/icons/lock-light.png");
        return lockLight;
    }
}
