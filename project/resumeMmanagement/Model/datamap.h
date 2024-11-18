#ifndef DATAMAP_H
#define DATAMAP_H
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <QVariantMap>
#include <QJsonObject>

class DataMap
{
public:
    DataMap(const QVariantMap& map);
    //from functions
    static DataMap fromSqlQuery(QSqlQuery query);
    static DataMap fromJsonObject(QJsonObject obj){
        return DataMap(obj.toVariantMap());
    };

    //to functions
    QJsonObject toJsonObject(){
        return QJsonObject::fromVariantMap(dataMap);
    };

private:
    QVariantMap dataMap;
};

#endif // DATAMAP_H
