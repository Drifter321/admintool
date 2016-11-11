#include "customitems.h"

void PlayerTimeTableItem::updateTime(float newTime)
{
    this->time = newTime;
    this->setTime();
}

void PlayerTimeTableItem::setTime()
{
    QString res;

    int days = qRound(this->time) / 60 / 60 / 24;
    int hours = qRound(this->time) / 60 / 60 % 24;
    int minutes = qRound(this->time) / 60 % 60;
    int seconds = qRound(this->time) % 60;

    if(days)
    {
        res.append(QString("%1%2").arg(QString::number(days), "d "));
    }
    if(hours)
    {
        res.append(QString("%1%2").arg(QString::number(hours), "h "));
    }
    if(minutes)
    {
        res.append(QString("%1%2").arg(QString::number(minutes), "m "));
    }
    res.append(QString("%1%2").arg(QString::number(seconds), "s"));

    this->setText(res);
}
