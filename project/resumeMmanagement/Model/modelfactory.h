#ifndef MODELFACTORY_H
#define MODELFACTORY_H
#include <QObject>
#include <QVariantMap>
#include "absmtmodel.h"
#include "usermodel.h"

class ModelFactory: public QObject
{
    Q_OBJECT
public:
    ModelFactory(QObject* parent=nullptr);
    static AbsMTModel* createModel(QString modelName, QVariantMap dataSrc){
        QByteArray nameByte = modelName.toUtf8();
        AbsMTModel* model;
        f.metaObject()->invokeMethod(&f, nameByte.data(), Qt::DirectConnection, Q_RETURN_ARG(AbsMTModel*, model),Q_ARG(QVariantMap, dataSrc));
        return model;
    }

    Q_INVOKABLE static UserModel* createUserModel(QVariantMap dataSrc){
        UserModel* model = new UserModel;
        model->updateSrcData(dataSrc);
        return new UserModel();
    };

private:
    static ModelFactory f;
};

#endif // MODELFACTORY_H
