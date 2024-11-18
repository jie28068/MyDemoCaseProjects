#include "slotproperty.h"
#include "CommonModelAssistant.h"
#include "GlobalAssistant.h"
#include "KLWidgets/KCustomDialog.h"
#include "PluginComponentServer/IPluginComponentServer.h"
#include "PropertyServerMng.h"
#include <QMenu>
#include <QUuid>
#include <qglobal.h>

using namespace Kcc::PluginComponent;

USE_LOGOUT_("SlotProperty")

//////////////////////////////////////////////////////////////////////////
SlotProperty::SlotProperty(QSharedPointer<Kcc::BlockDefinition::SlotBlock> pSlotBlock, bool bReadOnly)
    : BlockProperty(pSlotBlock, bReadOnly),
      m_pSlotBlock(pSlotBlock),
      m_basicPage(nullptr),
      m_pInputTableWidget(nullptr),
      m_pOutPutTableWidget(nullptr)
{
}

SlotProperty::~SlotProperty() { }

void SlotProperty::init()
{
    refreshWidget(m_pSlotBlock);
    setPropertyEditableStatus(m_readOnly);
}

QPixmap SlotProperty::getBlockPixmap()
{
    if (PropertyServerMng::getInstance().m_ControlComponentServer == nullptr
        || PropertyServerMng::getInstance().m_ElectricalComponentServer == nullptr || m_pSlotBlock == nullptr
        || m_basicPage == nullptr) {
        return QPixmap();
    }
    PModel tmpModel = m_basicPage->getSelectModel();
    if (tmpModel == nullptr) {
        return PropertyServerMng::getInstance().m_ControlComponentServer->GetComponentIcon(
                m_pSlotBlock->getPrototypeName(), m_pSlotBlock->isInstance() ? BLOCKTYPE_INSTANCE : BLOCKTYPE_PROTOTYPE,
                false);
    } else if (ElectricalBlock::Type == tmpModel->getModelType()) {
        return PropertyServerMng::getInstance().m_ElectricalComponentServer->GetComponentIcon(
                tmpModel->getPrototypeName(), tmpModel->isInstance() ? BLOCKTYPE_INSTANCE : BLOCKTYPE_PROTOTYPE,
                CombineBoardModel::Type == tmpModel->getModelType());
    } else {
        return PropertyServerMng::getInstance().m_ControlComponentServer->GetComponentIcon(
                tmpModel->getPrototypeName(), tmpModel->isInstance() ? BLOCKTYPE_INSTANCE : BLOCKTYPE_PROTOTYPE,
                CombineBoardModel::Type == tmpModel->getModelType());
    }

    // if (m_basicPage->getSelectModel() != nullptr) {
    //     QVariant picvalur = m_basicPage->getSelectModel()->getResource().value(PNG_NORMAL_PIC, QPixmap());
    //     if (picvalur.canConvert<QPixmap>()) {
    //         QPixmap pix = picvalur.value<QPixmap>();
    //         if (!pix.isNull()) {
    //             return pix;
    //         }
    //     }
    //     return drawTextIcon(m_basicPage->getSelectModel()->getName(), m_basicPage->getSelectModel()->isInstance(),
    //                         false);
    // }
    // QVariant picvalur = m_pSlotBlock->getResource().value(PNG_NORMAL_PIC, QPixmap());
    // if (picvalur.canConvert<QPixmap>()) {
    //     QPixmap pix = picvalur.value<QPixmap>();
    //     if (!pix.isNull()) {
    //         return pix;
    //     }
    // }
    // return drawTextIcon(m_pSlotBlock->getName(), m_pSlotBlock->isInstance(), false);
}

void SlotProperty::onDialogExecuteResult(QDialog::DialogCode code)
{
    BlockProperty::onDialogExecuteResult(code);
    if (code != QDialog::Accepted || m_basicPage == nullptr || m_pSlotBlock == nullptr) {
        return;
    }

    // 保存插槽指向实例与插槽名称，或插槽自定义端口
    if (m_basicPage->saveData(m_pInputTableWidget == nullptr ? QStringList() : m_pInputTableWidget->getNewSavedList(),
                              m_pOutPutTableWidget == nullptr ? QStringList()
                                                              : m_pOutPutTableWidget->getNewSavedList())) {
        m_bPropertyIsChanged = true;
    }
    if (m_bPropertyIsChanged) {
        m_pSlotBlock->setModifyTime(QDateTime::currentDateTime());
        CMA::saveModel(m_pSlotBlock);
    }
}

QString SlotProperty::getHelpUrl()
{

    return m_pSlotBlock->getPrototypeName();
}

void SlotProperty::refreshWidget(QSharedPointer<Kcc::BlockDefinition::Model> model)
{
    CommonWrapper::init();
    if (m_pSlotBlock == nullptr || PropertyServerMng::getInstance().m_ModelManagerServer == nullptr) {
        return;
    }

    if (nullptr == m_basicPage) {
        m_basicPage = new SlotBasicPage(m_pSlotBlock);
        connect(m_basicPage, SIGNAL(slotTypeChanged(QSharedPointer<Kcc::BlockDefinition::Model>)), this,
                SLOT(onSlotTypeChanged(QSharedPointer<Kcc::BlockDefinition::Model>)));
    }
    addWidget(m_basicPage, CMA::BLOCK_TAB_NAME_SLOT);
    if (model == nullptr || m_pSlotBlock->isInstance() || m_basicPage == nullptr) {
        return;
    }

    if (!m_basicPage->getSelectBlockDef().isEmpty()) {
        QList<CustomModelItem> inputlistdata;
        QList<CustomModelItem> outputlistdata;
        bool canope = false;
        if (m_pSlotBlock == model) {
            PModel selectModel = PropertyServerMng::getInstance().m_ModelManagerServer->GetToolkitModel(
                    m_basicPage->getSelectBlockDef(), KL_TOOLKIT::ELECTRICAL_TOOLKIT);
            if (selectModel != nullptr && ElectricalBlock::Type == selectModel->getModelType()) {
                canope = true;
                getInputOutputList(selectModel, inputlistdata, outputlistdata, m_basicPage->getPhase());
            } else {
                getInputOutputList(m_pSlotBlock, inputlistdata, outputlistdata);
            }
            setModelItemListChecked(inputlistdata,
                                    CMA::getVarGroupCheckList(m_pSlotBlock, RoleDataDefinition::InputSignal));
            setModelItemListChecked(outputlistdata,
                                    CMA::getVarGroupCheckList(m_pSlotBlock, RoleDataDefinition::OutputSignal));
        } else if (ElectricalBlock::Type == model->getModelType()) {
            getInputOutputList(model, inputlistdata, outputlistdata, m_basicPage->getPhase());
            canope = true;
        } else {
            getInputOutputList(model, inputlistdata, outputlistdata);
            setModelItemListChecked(inputlistdata, true);
            setModelItemListChecked(outputlistdata, true);
        }
        if (nullptr == m_pInputTableWidget) {
            m_pInputTableWidget = new VarSelectWidget(canope);
        }
        if (nullptr == m_pOutPutTableWidget) {
            m_pOutPutTableWidget = new VarSelectWidget(canope);
        }
        m_pInputTableWidget->setCanOperation(canope);
        m_pOutPutTableWidget->setCanOperation(canope);
        m_pInputTableWidget->updateCheckedVariableInfo(inputlistdata);
        m_pOutPutTableWidget->updateCheckedVariableInfo(outputlistdata);
        addWidget(m_pInputTableWidget, CMA::BLOCK_TAB_NAME_SELECT_INPUT);
        addWidget(m_pOutPutTableWidget, CMA::BLOCK_TAB_NAME_SELECT_OUTPUT);
    } else {
        if (m_pInputTableWidget != nullptr) {
            delete m_pInputTableWidget;
            m_pInputTableWidget = nullptr;
        }
        if (nullptr != m_pOutPutTableWidget) {
            delete m_pOutPutTableWidget;
            m_pOutPutTableWidget = nullptr;
        }
    }
}

QList<CustomModelItem> SlotProperty::getModelItemList(const QMap<QString, Kcc::ElecSys::ElecParamDef> &elecvarmap)
{
    QList<CustomModelItem> listdata;
    QVariantMap otherproperty;
    for (QString keyword : elecvarmap.keys()) {
        otherproperty.clear();
        otherproperty.insert(KEY_DISCRIPTION, elecvarmap[keyword].description);
        otherproperty.insert(KEY_VARIABLECHECKED, false);
        otherproperty.insert(KEY_VARIABLECHECKENABLE, true);
        listdata.append(CustomModelItem(keyword, keyword, false, RoleDataDefinition::ControlTypeCheckbox, true, "",
                                        otherproperty));
    }
    return listdata;
}

void SlotProperty::setModelItemListChecked(QList<CustomModelItem> &listdata, bool bchecked)
{
    for (int i = 0; i < listdata.size(); ++i) {
        listdata[i].otherPropertyMap[KEY_VARIABLECHECKED] = bchecked;
    }
}

void SlotProperty::setModelItemListChecked(QList<CustomModelItem> &listdata, const QStringList &checkList)
{
    for (int i = 0; i < listdata.size(); ++i) {
        listdata[i].otherPropertyMap[KEY_VARIABLECHECKED] = checkList.contains(listdata[i].keywords);
    }
}

void SlotProperty::getInputOutputList(QSharedPointer<Kcc::BlockDefinition::Model> model,
                                      QList<CustomModelItem> &inputList, QList<CustomModelItem> &outputList,
                                      const int &phaseNo)
{
    if (model == nullptr) {
        return;
    }
    inputList.clear();
    outputList.clear();
    if (ElectricalBlock::Type == model->getModelType()) {
        PElectricalBlock peblock = model.dynamicCast<ElectricalBlock>();
        if (peblock == nullptr || PropertyServerMng::getInstance().m_pIElecSysServer == nullptr) {
            return;
        }
        inputList = getModelItemList(
                PropertyServerMng::getInstance().m_pIElecSysServer->GetInputParamMap(model->getPrototypeName()));
        outputList = getModelItemList(PropertyServerMng::getInstance().m_pIElecSysServer->GetOutputParamMap(
                model->getPrototypeName(), phaseNo));
    } else {
        inputList =
                CMA::getPropertyModelItemList(model, RoleDataDefinition::InputSignal, true, "", QStringList(), false);
        outputList =
                CMA::getPropertyModelItemList(model, RoleDataDefinition::OutputSignal, true, "", QStringList(), false);
    }
}

void SlotProperty::onSlotTypeChanged(QSharedPointer<Kcc::BlockDefinition::Model> model)
{
    refreshWidget(model);
    emit viewChanged();
}
