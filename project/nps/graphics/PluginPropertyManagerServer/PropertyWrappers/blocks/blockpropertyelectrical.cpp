#include "blockpropertyelectrical.h"
#include "GlobalAssistant.h"
#include "PropertyServerMng.h"
#include <QDebug>
#include <QList>
#include <qmath.h>
// #include "server/UIDrawingBoardServer/IUIDrawingBoardServer.h"

// 0--不可见
// 1--可见，不可编辑
// 2--可见，可编辑
#define INVISIBLE 0
#define VISIBLE 1
#define VISIBLE_EDITABLE 2

// 注册及请求服务所需要的头文件
#include "CoreLib/ServerManager.h"
USE_LOGOUT_("ElectricalBlockPropertyEditor")

ElectricalBlockPropertyEditor::ElectricalBlockPropertyEditor(
        QSharedPointer<Kcc::BlockDefinition::Model> pElectricalBlock, bool isReadOnly)
    : BlockProperty(pElectricalBlock, isReadOnly), m_pElectricalBlock(pElectricalBlock)
{
}

ElectricalBlockPropertyEditor::~ElectricalBlockPropertyEditor() { }

void ElectricalBlockPropertyEditor::init()
{
    if (m_pElectricalBlock == nullptr) {
        return;
    }
    BlockProperty::init();
    bool boardActive = boardActiveStatus(m_pElectricalBlock);
    addModelWidget(m_pElectricalBlock, "", ElecCombineBoardModel::Type == m_pElectricalBlock->getModelType());
    m_readOnly = m_readOnly ? true : !boardActive;
    setPropertyEditableStatus(m_readOnly);
}

QPixmap ElectricalBlockPropertyEditor::getBlockPixmap()
{
    if (PropertyServerMng::getInstance().m_ElectricalComponentServer == nullptr || m_pElectricalBlock == nullptr) {
        return QPixmap();
    }

    return PropertyServerMng::getInstance().m_ElectricalComponentServer->GetComponentIcon(
            m_pElectricalBlock->getPrototypeName(),
            m_pElectricalBlock->isInstance() ? BLOCKTYPE_INSTANCE : BLOCKTYPE_PROTOTYPE,
            CombineBoardModel::Type == m_pElectricalBlock->getModelType());
    // QVariant picvalur = m_pElectricalBlock->getResource().value(PNG_NORMAL_PIC, QPixmap());
    // if (picvalur.canConvert<QPixmap>()) {
    //     QPixmap pix = picvalur.value<QPixmap>();
    //     if (!pix.isNull()) {
    //         return pix;
    //     }
    // }
    // return drawTextIcon(m_pElectricalBlock->getName(), m_pElectricalBlock->isInstance(), false);
}

void ElectricalBlockPropertyEditor::onDialogExecuteResult(QDialog::DialogCode code)
{
    if (code != QDialog::Accepted || m_pElectricalBlock == nullptr) {
        m_bPropertyIsChanged = false;
        return;
    }
    for (TabDetailItem item : m_TabItemList) {
        for (QString tabWidgetName : item.nameMapWidgetMap.keys()) {
            if (item.nameMapWidgetMap[tabWidgetName] == nullptr) {
                continue;
            }
            CWidget *tmpwidget = dynamic_cast<CWidget *>(item.nameMapWidgetMap[tabWidgetName]);
            if (tmpwidget == nullptr) {
                continue;
            }
            if (tmpwidget->saveData()) {
                m_bPropertyIsChanged = true;
            }
        }
    }
    if (m_bPropertyIsChanged) {
        m_pElectricalBlock->setModifyTime(QDateTime::currentDateTime());
        CMA::saveModel(m_pElectricalBlock);
    }

    // 母线参数变更需要重新刷新画面block状态
    // if (m_pElectricalBlock->prototypeName == "Busbar" || m_pElectricalBlock->prototypeName == "DotBusbar") {
    //     QString boardName = PropertyServerMng::getInstance().m_pDataManagerServer->GetBoradNameByUUID(
    //             m_pElectricalBlock->getParameter(Block::boardUUID).toString());
    //     m_propertyManagerServer->notifySceneUpdateState(boardName);
    // }
}

bool ElectricalBlockPropertyEditor::checkValue(QString &errorinfo)
{
    errorinfo = "";
    if (ElectricalBlock::Type == m_pElectricalBlock->getModelType()) {
        return CommonWrapper::checkValue(errorinfo);
    }

    for (TabDetailItem &item : m_TabItemList) {
        if (CMA::BLOCK_TAB_NAME_ELEC_PARAM == item.treetabName) {
            if (!checkParamTabValid(errorinfo, item.nameMapWidgetMap)) {
                return false;
            }
            continue;
        }
        for (QString tabWidgetName : item.nameMapWidgetMap.keys()) {
            if (item.nameMapWidgetMap[tabWidgetName] == nullptr) {
                continue;
            }
            CWidget *tmpwidget = dynamic_cast<CWidget *>(item.nameMapWidgetMap[tabWidgetName]);
            if (tmpwidget != nullptr && !tmpwidget->checkLegitimacy(errorinfo)) {
                return false;
            }
        }
    }
    return true;
}

QString ElectricalBlockPropertyEditor::getHelpUrl()
{
    return m_pElectricalBlock->getPrototypeName();
}

bool ElectricalBlockPropertyEditor::boardActiveStatus(QSharedPointer<Kcc::BlockDefinition::Model> model)
{
    if (model == nullptr || model->getParentModel() == nullptr) {
        return false;
    }
    PModel parentModel = model->getParentModel();
    while (parentModel->getParentModel() != nullptr) {
        parentModel = parentModel->getParentModel();
    }
    if (parentModel == nullptr) {
        return false;
    }

    if (ElecCombineBoardModel::Type == parentModel->getModelType()) {
        return true;
    } else {
        if (PropertyServerMng::getInstance().m_projectManagerServer != nullptr
            && PropertyServerMng::getInstance()
                       .m_projectManagerServer->GetProjectConfig(KL_PRO::BOARD_ISACTIVATE)
                       .canConvert<QVariantMap>()) {
            QVariantMap boardactivemap = PropertyServerMng::getInstance()
                                                 .m_projectManagerServer->GetProjectConfig(KL_PRO::BOARD_ISACTIVATE)
                                                 .value<QVariantMap>();
            if (boardactivemap.value(parentModel->getUUID(), false).toBool()) {
                return true;
            }
        }
        return false;
    }
}

bool ElectricalBlockPropertyEditor::blockCanCheckVar(QSharedPointer<Kcc::BlockDefinition::Model> model)
{
    if (model == nullptr) {
        return false;
    }

    // 地线与断路器元件不需要结果保存属性页
    if (NPS::PROTOTYPENAME_GROUND == model->getPrototypeName()
        || NPS::PROTOTYPENAME_CIRCUITBREAKER == model->getPrototypeName()) {
        return false;
    }

    PModel parentModel = model;
    while (parentModel->getParentModel() != nullptr) {
        parentModel = parentModel->getParentModel();
    }

    if (ElecCombineBoardModel::Type == parentModel->getModelType()) {
        return false;
    } else if (ElecBoardModel::Type == parentModel->getModelType()) {
        return true;
    }
    return false;
}

void ElectricalBlockPropertyEditor::addModelWidget(QSharedPointer<Kcc::BlockDefinition::Model> model,
                                                   const QString &tabStrStart, bool addTabName, bool isFirst)
{
    if (model == nullptr) {
        return;
    }
    if (ElecCombineBoardModel::Type == model->getModelType()) {
        for (PModel cmodel : model->getChildModels().values()) {
            addModelWidget(
                    cmodel,
                    isFirst ? "" : (tabStrStart.isEmpty() ? model->getName() : (tabStrStart + "/" + model->getName())),
                    true, false);
        }
        return;
    }
    if (ElectricalBlock::Type != model->getModelType()
        || PropertyServerMng::getInstance().m_projectManagerServer == nullptr
        || PropertyServerMng::getInstance().m_pIElecSysServer == nullptr) {
        return;
    }
    PElectricalBlock pelecBlock = model.dynamicCast<ElectricalBlock>();
    if (pelecBlock == nullptr) {
        return;
    }

    if (pelecBlock->getParentModel() == nullptr) {
        return;
    }
    bool boardActive = boardActiveStatus(pelecBlock);
    ElectricalParameterWidget *parameterWidget = nullptr;  // 电气参数
    PowerFlowParamWidget *pPowerFlowParamWidget = nullptr; // 潮流计算参数
    ElectricalDeviceType *deviceWidget = nullptr;          // 设备类型
    FeatureCurveWidget *featureCurveWidget = nullptr;      // 特性曲线
    SimuSetWidget *pSimuWidget = nullptr;                  // 仿真参数
    InitSetWidget *pInitParamWidget = nullptr;             // 初始值参数
    CustomTableWidget *pCtrlSignalWidget = nullptr;        // 控制参数
    VarSelectWidget *pvarWidget = nullptr;                 // 结果保存
    // 设备类型
    QString tabwidgetName = addTabName
            ? (tabStrStart.isEmpty() ? pelecBlock->getName() : tabStrStart + "/" + pelecBlock->getName())
            : "";

    //////////////////////跨电网关联新功能临时代码///////////////////////
    QString strPrototypeName = pelecBlock->getPrototypeName();
    if (strPrototypeName == NPS::PROTOTYPENAME_PI_LINKLINE) {
        strPrototypeName = NPS::PROTOTYPENAME_PI_LINE;
    }
    ////////////////////////////////////////////////////////////////

    if (PropertyServerMng::getInstance().m_projectManagerServer->HaveDeviceModel(strPrototypeName)) {
        deviceWidget = new ElectricalDeviceType(pelecBlock);
        addWidget(deviceWidget, CMA::BLOCK_TAB_NAME_DEV_TYPE, true, tabwidgetName);
    } else {
        addWidget(nullptr, CMA::BLOCK_TAB_NAME_DEV_TYPE, true, tabwidgetName);
    }
    // 电气参数
    if (pelecBlock->getElectricalVariableGroup() != nullptr
        && pelecBlock->getElectricalVariableGroup()->getVariableMap().size() > 0) {
        parameterWidget = new ElectricalParameterWidget(pelecBlock);
        addWidget(parameterWidget, CMA::BLOCK_TAB_NAME_ELEC_PARAM, true, tabwidgetName);
    } else {
        addWidget(nullptr, CMA::BLOCK_TAB_NAME_ELEC_PARAM, true, tabwidgetName);
    }
    if (NPS::PROTOTYPENAME_PHOTOVOLTAICSOURCE == strPrototypeName) {
        featureCurveWidget = new FeatureCurveWidget(pelecBlock);
        addWidget(featureCurveWidget, CMA::BLOCK_TAB_NAME_FEATURECURVE, true, tabwidgetName);
    } else {
        addWidget(nullptr, CMA::BLOCK_TAB_NAME_FEATURECURVE, true, tabwidgetName);
    }

    // 潮流参数
    if (pelecBlock->getLoadFlowVariableGroup() != nullptr
        && pelecBlock->getLoadFlowVariableGroup()->getVariableMap().size() > 0) {
        pPowerFlowParamWidget = new PowerFlowParamWidget(
                pelecBlock, (parameterWidget == nullptr) ? QList<CustomModelItem>() : parameterWidget->getTableList());
        addWidget(pPowerFlowParamWidget, CMA::BLOCK_TAB_NAME_TIDE_PARAM, true, tabwidgetName);
    } else {
        addWidget(nullptr, CMA::BLOCK_TAB_NAME_TIDE_PARAM, true, tabwidgetName);
    }

    // 仿真设置
    if (pelecBlock->getSimulationVariableGroup() != nullptr
        && pelecBlock->getSimulationVariableGroup()->getVariableMap().size() > 0) {
        pSimuWidget = new SimuSetWidget(pelecBlock);
        addWidget(pSimuWidget, CMA::BLOCK_TAB_NAME_ELEC_SIMU, true, tabwidgetName);
    } else {
        addWidget(nullptr, CMA::BLOCK_TAB_NAME_ELEC_SIMU, true, tabwidgetName);
    }

    // 初始值设置
    if (pelecBlock->getInitializationVariableGroup() != nullptr
        && pelecBlock->getInitializationVariableGroup()->getVariableMap().size() > 0) {
        pInitParamWidget = new InitSetWidget(pelecBlock);
        addWidget(pInitParamWidget, CMA::BLOCK_TAB_NAME_ELEC_INIT, true, tabwidgetName);
    } else {
        addWidget(nullptr, CMA::BLOCK_TAB_NAME_ELEC_INIT, true, tabwidgetName);
    }
    // 控制信号
    QMap<QString, ElecParamDef> inputMap =
            PropertyServerMng::getInstance().m_pIElecSysServer->GetInputParamMap(strPrototypeName);
    if (inputMap.size() > 0) {
        pCtrlSignalWidget = new CustomTableWidget();
        QList<CustomModelItem> listdata;
        QVariantMap otherPropertyMap;
        for (QString strName : inputMap.keys()) {
            otherPropertyMap.insert(KEY_DISCRIPTION, inputMap[strName].description);
            listdata.append(CustomModelItem(strName, strName, inputMap[strName].description,
                                            RoleDataDefinition::ControlTypeTextbox, false));
        }
        pCtrlSignalWidget->setListData(listdata, QStringList() << tr("Signal Name") << CMA::HEADER_DESCRIPTION);
        addWidget(pCtrlSignalWidget, CMA::BLOCK_TAB_NAME_CTRLSIGNAL, true, tabwidgetName);
    } else {
        addWidget(nullptr, CMA::BLOCK_TAB_NAME_CTRLSIGNAL, true, tabwidgetName);
    }

    // 结果保存// 地线与断路器元件不需要结果保存属性页
    if (blockCanCheckVar(pelecBlock)) {
        bool bCanSetVar = true;
        if (pelecBlock->getState() != Model::StateNormal && pelecBlock->getState() != Model::StateWarring) {
            bCanSetVar = false;
        }
        if (pvarWidget == nullptr) {
            pvarWidget = new VarSelectWidget(boardActive && bCanSetVar);
        }
        pvarWidget->updateElecCheckedVariableInfo(PropertyServerMng::getInstance().m_pIElecSysServer->GetOutputParamMap(
                                                          strPrototypeName, pelecBlock->getPhaseNumber()),
                                                  pelecBlock);
        addWidget(pvarWidget, CMA::BLOCK_TAB_NAME_ELEC_SAVE, true, tabwidgetName);
    } else {
        addWidget(nullptr, CMA::BLOCK_TAB_NAME_ELEC_SAVE, true, tabwidgetName);
    }
    // 连接信号
    if (parameterWidget != nullptr) {
        connect(parameterWidget, SIGNAL(jumpToModel()), this, SIGNAL(closeWrapper()));
    }
    if (parameterWidget != nullptr && pPowerFlowParamWidget != nullptr) {
        connect(parameterWidget, SIGNAL(electricalParamChanged(const QList<CustomModelItem> &)), pPowerFlowParamWidget,
                SLOT(onParamChanged(const QList<CustomModelItem> &)));
    }
    if (parameterWidget != nullptr && pvarWidget != nullptr) {
        connect(parameterWidget, &ElectricalParameterWidget::phaseNoChanged, pvarWidget,
                &VarSelectWidget::updateElecCheckedVariableInfo);
    }
    if (deviceWidget != nullptr && pPowerFlowParamWidget != nullptr) {
        connect(deviceWidget, SIGNAL(deviceTypeChanged(QSharedPointer<Kcc::BlockDefinition::DeviceModel>)),
                pPowerFlowParamWidget, SLOT(onDeviceTypeChanged(QSharedPointer<Kcc::BlockDefinition::DeviceModel>)));
    }
    if (deviceWidget != nullptr && featureCurveWidget != nullptr) {
        connect(deviceWidget, SIGNAL(deviceTypeChanged(QSharedPointer<Kcc::BlockDefinition::DeviceModel>)),
                featureCurveWidget, SLOT(updateGraphWidget(QSharedPointer<Kcc::BlockDefinition::DeviceModel>)));
    }
    if (pvarWidget != nullptr && deviceWidget != nullptr) {
        connect(pvarWidget, SIGNAL(syncLineComponentInfo()), deviceWidget, SLOT(onSyncLineComponentInfo()));
    }
}

bool ElectricalBlockPropertyEditor::checkParamTabValid(QString &errorinfo, const QMap<QString, QWidget *> &widgetMap)
{
    QMap<QString, QStringList> layerNamesMap;
    for (QString tabWidgetName : widgetMap.keys()) {
        if (widgetMap[tabWidgetName] == nullptr) {
            continue;
        }
        ElectricalParameterWidget *eleParamWidget = dynamic_cast<ElectricalParameterWidget *>(widgetMap[tabWidgetName]);
        if (eleParamWidget == nullptr) {
            continue;
        }
        PElectricalBlock eleBlock = eleParamWidget->getElecBlock();
        if (eleBlock == nullptr) {
            continue;
        }
        PModel parentModel = eleBlock->getParentModel();
        QString layerStr;
        while (parentModel != nullptr) {
            layerStr = layerStr.isEmpty() ? parentModel->getName() : layerStr + "/" + parentModel->getName();
            parentModel = parentModel->getParentModel();
        }
        if (!layerNamesMap.contains(layerStr)) {
            layerNamesMap.insert(layerStr, getSameLayerCombineNames(eleBlock));
        }
        if (layerNamesMap[layerStr].contains(eleParamWidget->getName())) {
            errorinfo = tr("Block [%1]'s name [%2] already exists. Please rename!")
                                .arg(tabWidgetName)
                                .arg(eleParamWidget->getName());
            return false;
        } else {
            layerNamesMap[layerStr].append(eleParamWidget->getName());
        }
        if (!eleParamWidget->checkExcludeName(errorinfo)) {
            return false;
        }
    }
    return true;
}

QStringList ElectricalBlockPropertyEditor::getSameLayerCombineNames(QSharedPointer<Kcc::BlockDefinition::Model> model)
{
    if (model == nullptr || model->getParentModel() == nullptr) {
        return QStringList();
    }
    QStringList blockList;
    for (PModel cmodel : model->getParentModel()->getChildModels().values()) {
        if (cmodel != nullptr && ElecCombineBoardModel::Type == cmodel->getModelType()) {
            blockList.append(cmodel->getName());
        }
    }
    return blockList;
}
