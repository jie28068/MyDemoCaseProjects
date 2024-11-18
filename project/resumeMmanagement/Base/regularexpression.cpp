#include "regularexpression.h"

RegularExpression::RegularExpression()
{
}

void RegularExpression::ToDealFormat(QString &str) // demo    2022-3-4==>2022-03-04    2022-3 ==> 2022-03
{
    QStringList list = str.split("-");
    for (int i = 0; i < list.count(); i++)
    {
        if (list.at(i).length() == 1)
        {
            QString tempstr = QString("0%1").arg(list.at(i));
            list.replace(i, tempstr);
        }
        if (list.at(i) == "") // 新增处理  "2020-03-"==>"2020-03"
        {
            list.removeAt(i);
        }
    }
    QString joinstr = list.join("-");
    str.clear();
    str = joinstr;
}

void RegularExpression::ToFiltFormat(QString &str) // demo   2022-10-9-2022-11-10==>2022-10-2022-11   去除日期
{
    QStringList list = str.split("-");
    if (list.count() == 6)
    {
        list.removeAt(5);
        list.removeAt(2);
    }
    str.clear();
    str = list.join("-");
}

bool caseInsensitiveLessThan(const QString &s1, const QString &s2)
{
    return s1.toLower() < s2.toLower();
}

QStringList RegularExpression::DealListdata(QStringList &list)
{
    QStringList templist;
    if (list.isEmpty())
    {
        return templist;
    }
    qDebug() << "初始:";
    qDebug() << list;
    /////对list处理
    list.removeAll(QString("")); // 移除空字符串
    for (int i = 0; i < list.count(); i++)
    {
        QString temp = list.value(i);
        temp.remove(QChar(' '), Qt::CaseInsensitive);
        temp.remove(QChar('\r'), Qt::CaseInsensitive);
        temp.remove(QChar('\t'), Qt::CaseInsensitive); // 移除空格
        temp.replace("年", ".");
        temp.replace("月", ".");
        temp.replace("日", ".");
        temp.replace("至", "-");
        temp.replace(".", "-");
        temp.replace("~", "-");
        temp.replace("/", "-");
        temp.replace("--", "-");
        temp.replace("---", "-"); // 对一些字符替换处理
        templist.append(temp);
    }

    templist = templist.toSet().toList();                             // 去重复项
    qSort(templist.begin(), templist.end(), caseInsensitiveLessThan); // 排序

    for (int i = 0; i < templist.count(); i++) // 对一些特定的数据格式进行处理   便于统一格式、数据比对
    {
        QString str = templist.at(i);
        ToDealFormat(str);
        ToFiltFormat(str);
        if (str != templist.at(i)) // 如果str发生改变，就替换
        {
            templist.replace(i, str);
        }
    }
    QStringList retlist;
    for (int i = 0; i < templist.count(); i++)
    {
        if (templist.at(i).length() != 9)
        {
            retlist.append(templist.at(i));
        }
        else
        {
            qDebug() << "剔除格式:" << templist.at(i); // 剔除xxxx-xxxx格式无用数据
        }
    }

    qDebug() << "处理:";
    qDebug() << retlist << endl;
    return retlist;
}

QString RegularExpression::GetTextBirthday(QStringList &list) // 获取生日
{
    if (list.isEmpty())
        return QString();

    if (IsContainBirthday(list))
    {
        return list.at(0);
    }
    else
    {
        return NULL;
    }
}

bool RegularExpression::IsContainBirthday(QStringList &list)
{
    if (list.isEmpty())
        return false;
    qDebug() << "Get Text list[0] :" << list.at(0);
    // QRegularExpression re("\\d{4}[-/年.]\\d{1,2}[月]?([.-/]?\\d{1,2}[日]?)? ?");    // xxxx.x/xx.(x/xx)
    if (list.at(0).length() <= 10) // xxxx-xx:7  xxxx-xx-xx:10  xxxx-xx-xxxx-xx:15
    {
        qDebug() << "Text contain birthday,birthday matched sucsses";

        return true;
    }
    qDebug() << "Text no birthday info";
    return false;
}

QStringList RegularExpression::GetTextExperience(QStringList &list)
{
    QStringList templist = list;
    if (IsContainBirthday(templist))
    {
        templist.removeAt(0);
    }
    return templist;
}

QStringList RegularExpression::GetRegularTimeList(QString text)
{
    QRegularExpression::MatchType matchType = QRegularExpression::NormalMatch;
    QRegularExpression::PatternOptions patternOptions = QRegularExpression::NoPatternOption;
    QRegularExpression::MatchOptions matchOptions = QRegularExpression::NoMatchOption;
    QStringList list;
    QString expression = "\\d{4}[-/年.]\\d{1,2}[月]?([.-/]?\\d{1,2}[日]?)? ?[-~至 ]* ?(\\d{4}[-/年.]\\d{1,2}[月]?([.-/]?\\d{1,2}[日]?)?)?"; // 筛选文本 正则表达式
    QRegularExpression rx(expression);
    QRegularExpressionMatch match = rx.match(text);
    if (!rx.isValid())
    {
        qDebug() << ("QRegularExpression is Invalid");
        return list;
    }

    patternOptions |= QRegularExpression::DontCaptureOption; // 过滤重复的

    rx.setPatternOptions(patternOptions);

    const int capturingGroupsCount = rx.captureCount() + 1;

    QRegularExpressionMatchIterator iterator = rx.globalMatch(text, 0, matchType, matchOptions);

    while (iterator.hasNext())
    {
        QRegularExpressionMatch match = iterator.next();
        for (int captureGroupIndex = 0; captureGroupIndex < capturingGroupsCount; ++captureGroupIndex)
        {
            list.append(match.captured(captureGroupIndex));
        }
    }
    QStringList templist = DealListdata(list);
    return templist;
}
