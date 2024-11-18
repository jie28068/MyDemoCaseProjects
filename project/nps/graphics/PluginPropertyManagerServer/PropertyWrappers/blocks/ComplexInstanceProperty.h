#ifndef COMPLEXINSTANCEPROPERTY_H
#define COMPLEXINSTANCEPROPERTY_H

#pragma once
#include "CommonWrapper.h"
#include "ComplexInstanceWidget.h"

class ComplexInstanceProperty : public CommonWrapper
{
public:
    ComplexInstanceProperty(QSharedPointer<Kcc::BlockDefinition::Model> complexModel,
                            ComplexInstanceWidget::Operation ope, const QString &cpxProtoName = "",
                            bool readOnly = false);
    ~ComplexInstanceProperty();
    virtual void init();
    virtual QString getTitle();
    QSharedPointer<Kcc::BlockDefinition::Model> getComplexModel();

    virtual void onDialogExecuteResult(QDialog::DialogCode code);

private:
    ComplexInstanceWidget *m_complexWidget;
    ComplexInstanceWidget::Operation m_Operation;
    QString m_cpxInitProtoModelName;
    QSharedPointer<Kcc::BlockDefinition::Model> m_complexModel;
};

#endif