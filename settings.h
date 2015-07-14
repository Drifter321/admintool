#ifndef SETTINGS
#define SETTINGS

#include "mainwindow.h"
#include <QSettings>
#include <QMAp>

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

#endif // SETTINGS

