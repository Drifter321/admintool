#ifndef PLAYERTIMEITEM_H
#define PLAYERTIMEITEM_H

#include <QTableWidgetItem>

class ServerInfo;
class MainWindow;

//Time played in player area
class PlayerTimeTableItem : public QTableWidgetItem
{
public:
    void updateTime(float newTime);
    bool operator< (const QTableWidgetItem &other) const
    {
        return (this->time < ((PlayerTimeTableItem *)&other)->time);
    }
    float getTime(){return time;}
private:
    void setTime();
    float time;
};

//Server items
class ServerTableIndexItem : public QTableWidgetItem
{
public:
    ServerTableIndexItem(ServerInfo *info);
    ServerTableIndexItem(ServerInfo *info, QString text);
    ServerInfo *GetServerInfo(){return serverInfo;}
private:
    ServerInfo *serverInfo = nullptr;
};


//Player count in browser area
class PlayerTableItem : public QTableWidgetItem
{
public:
    bool operator< (const QTableWidgetItem &other) const
    {
        return (this->players < ((PlayerTableItem *)&other)->players);
    }
    int players;
};

#endif // PLAYERTIMEITEM_H
