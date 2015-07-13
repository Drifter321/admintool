#ifndef PLAYERTIMEITEM_H
#define PLAYERTIMEITEM_H

#include <QTableWidgetItem>

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
