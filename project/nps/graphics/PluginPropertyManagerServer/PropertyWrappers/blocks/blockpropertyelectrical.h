#pragma once

#include "FeatureCurveWidget.h"
#include "blockproperty.h"
#include "electricaldevicetype.h"
#include "electricalparameterwidget.h"
#include "powerflowparamwidget.h"
#include "server/SimuNPSAdapter/ISimuNPSDataDictionaryServer.h"
#include "simusetwidget.h"
#include "varselectwidget.h"
using namespace Kcc::SimuNPSAdapter;
class VarSelectWidget;
// 电气
class ElectricalBlockPropertyEditor : public BlockProperty
{
    Q_OBJECT
public:
    ElectricalBlockPropertyEditor(QSharedPointer<Kcc::BlockDefinition::Model> pElectricalBlock,
                                  bool isReadOnly = false);
    ~ElectricalBlockPropertyEditor();
    virtual void init() override;

    // CommonWrapper
    virtual QPixmap getBlockPixmap();
    virtual void onDialogExecuteResult(QDialog::DialogCode code);
    virtual bool checkValue(QString &errorinfo = QString()) override;
    virtual QString getHelpUrl() override;

private:
    bool boardActiveStatus(QSharedPointer<Kcc::BlockDefinition::Model> model);
    /// @brief 判断模块是否可编辑结果保存
    /// @param model
    /// @return
    bool blockCanCheckVar(QSharedPointer<Kcc::BlockDefinition::Model> model);
    void addModelWidget(QSharedPointer<Kcc::BlockDefinition::Model> model, const QString &tabStrStart, bool addTabName,
                        bool isFirst = true);
    /// @brief 检查电气参数有效性
    /// @param errorinfo 错误信息
    /// @param widgetMap
    /// @return
    bool checkParamTabValid(QString &errorinfo, const QMap<QString, QWidget *> &widgetMap);
    /// @brief 获取同层构造模块名列表
    /// @param model 模块
    /// @return
    QStringList getSameLayerCombineNames(QSharedPointer<Kcc::BlockDefinition::Model> model);

private:
    QSharedPointer<Kcc::BlockDefinition::Model> m_pElectricalBlock;
};
