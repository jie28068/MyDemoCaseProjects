#include "computationtime.h"

ComputationTime::ComputationTime()
{

}

QString ComputationTime::MTime(QString str)
{
    if(str == NULL)
    {
        return NULL;
    }
    QString page = str.left(4);
    QDateTime dateTime(QDateTime::currentDateTime());
    QString nage = dateTime.toString().right(4);
    QString iage =QString::number((nage.toInt() - page.toInt()));
    return iage;
}
