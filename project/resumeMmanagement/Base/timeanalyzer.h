#ifndef TIMEANALYZER_H
#define TIMEANALYZER_H

#include <QTime>

class TimeAnalyzer
{
public:
    TimeAnalyzer(QString func);
    ~TimeAnalyzer();
    QTime m_time;
    QString m_func;
};

#endif // TIMEANALYZER_H
