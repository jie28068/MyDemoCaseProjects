#ifndef FORMATTRANSLATOR_H
#define FORMATTRANSLATOR_H
#include <QJsonDocument>
#include <QJsonObject>
#include <QVariantMap>
#include <QByteArray>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <QtDebug>

class FormatTransLator{
public:
    FormatTransLator(){};
    //在QByteArray,QVariantMap,QJsonObject之间转换[byte，map，json]

    static QVariantMap ByteToMap(const QByteArray& byte){
        QJsonDocument jDoc = QJsonDocument::fromJson(byte);
        QJsonObject jObj = jDoc.object();
        return jObj.toVariantMap();
    };
    /**
     * @brief QueryToMap    将sql查询结果query转成QList<QVariant(QVariantMap)>
     * @param query sql语句查询结果
     * @return  QList<QVariant(QVariantMap)>，一个map为一行数据
     */
    static QVariantList QueryToMap(QSqlQuery& query){
        QSqlRecord record = query.record();
        //获取列数
        int colCount = record.count();

        QVariantList mapList;
        while(query.next()){
            QVariantMap map;
            for(int i=0; i<colCount; i++)
            {
                QString key = record.fieldName(i);
                map.insert(key, query.value(key));
            }
            mapList.append(map);
        }

        return mapList;
    }

    //QList<QVariantMap>  ---->  QList<QVariantMap>
    static QList<QVariantMap> QVariantToMapList(QVariantList l){
        QList<QVariantMap> list;
        foreach(QVariant v, l)
        {
            list<<v.toMap();
        }
        return list;
    }
    //QList<QVariant>  ---->  QList<QString>
    static QStringList QVariantToStringList(QVariantList l){
        QStringList list;
        foreach(QVariant v, l)
        {
            list<<v.toString();
        }
        return list;
    }

    //QList<QVariant(QVariantMap)>   ----->   QMap<Key,T>
    template <class Key, class T>
    static QMap<Key,T> QVariantListToQMap(QVariantList list , QString keyField, QString valueField)
    {
        QMap<Key,T> map;
        foreach (QVariant v, list) {
            QVariantMap vMap = v.toMap();
            map.insert(vMap[keyField].value<Key>(), vMap[valueField].value<T>());
        }
        return map;
    }

    ////QList<QVariant(QVariantMap)>   ----->   QList<T>
    template<class T>
    static QList<T> QVariantListTranslate(QVariantList list, QString keyField)
    {
        QList<T> retList;
        foreach (QVariant v, list) {
            QVariantMap vMap = v.toMap();
            retList.append(vMap[keyField].value<T>());
        }
        return retList;
    }
    //......剩余的有需要了再添加
};

#endif // FORMATTRANSLATOR_H
