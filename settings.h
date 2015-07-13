#ifndef SETTINGS
#define SETTINGS

#include "mainwindow.h"
#include <QSettings>

class Settings : public QSettings
{
public:
    Settings(MainWindow *main);
    ~Settings();
    void SetDefaultSettings();
    void ReadSettings();
    void SaveSettings();
    void GetAppIDListMap();
private:
    QSettings *pSettings;
    QSettings *pAppIds;
    MainWindow *pMain;
};

extern QList<ServerInfo *> serverList;
extern QMap<int, QString> appIDMap;
#endif // SETTINGS

