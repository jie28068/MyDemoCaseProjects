#include "timeanalyzer.h"
#include <QDebug>

TimeAnalyzer::TimeAnalyzer(QString func)
{
    m_time.start();
    m_func = func;
}

TimeAnalyzer::~TimeAnalyzer()
{
    qDebug()<<m_func<<"  --  "<<m_time.elapsed()/1000.0<<"s";
}
