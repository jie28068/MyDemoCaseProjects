#ifndef REGULAREXPRESSION_H
#define REGULAREXPRESSION_H

#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QDebug>
#include <QStringList>
class RegularExpression
{
public:
    RegularExpression();
    void ToDealFormat(QString &str);                    //格式处理
    void ToFiltFormat(QString &str);                    //过滤一些格式转换成固定格式
    QStringList  DealListdata(QStringList &list);     //对列表进行处理
    QString GetTextBirthday(QStringList &list);           //获取生日
    bool IsContainBirthday(QStringList &list);           //判断列表是否有生日
    QStringList GetTextExperience(QStringList &list);         //获取经历段
    QStringList GetRegularTimeList(QString text);     //获取生日 经历段列表
};

#endif // REGULAREXPRESSION_H
