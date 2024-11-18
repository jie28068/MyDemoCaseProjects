#include "BlockDelegateBase.h"

#include "BlockDelegate/BlockDelegateMng.h"
#include "ElecBoardModel.h"
#include "ElectricalBlock.h"
#include "GraphicsModelingConst.h"
#include "GraphicsModelingTool.h"

#include <QFile>
#include <QSvgRenderer>
#include <math.h>

BlockDelegateBase::BlockDelegateBase()
{
    m_pBlockDelegateMng = nullptr;
}

void BlockDelegateBase::initBlock(PModel model, BlockDelegateMng *mng)
{
    if (!mng)
        return;

    m_pModel = model;
    m_pBlockDelegateMng = mng;
    m_boardModel = mng->m_boardModel;

    init(model);
}

void BlockDelegateBase::uninitBlock(PModel model)
{
    uninit();
}

bool BlockDelegateBase::onEditProperty(SourceProxy *source)
{
    if (!m_pModel)
        return false;

    return ModelingServerMng::getInstance().m_pPropertyManagerServer->ShowBlockProperty(m_pModel);
}

bool BlockDelegateBase::onDoubleClicked(SourceProxy *source)
{
    if (!m_pModel)
        return false;

    return ModelingServerMng::getInstance().m_pPropertyManagerServer->ShowBlockProperty(m_pModel);

    return false;
}

void BlockDelegateBase::init(PModel model) { }

void BlockDelegateBase::uninit() { }

void BlockDelegateBase::updateOutputVars(QString blockOldName, QString blockNewName)
{
    QMap<QString, PVariable> simuoutMap;
    auto curDict = NPS_DataDictionaryNodeName;
    if (!m_pModel || !ModelingServerMng::getInstance().m_pDataDictionaryServer) {
        return;
    }
    QString boardName = m_pModel->getParentModelName();
    QList<PVariable> tmpoutputList = m_pModel->getResultSaveVariableList();
    foreach (PVariable var, tmpoutputList) {
        simuoutMap[var->getName()] = var;
    }
    QMapIterator<QString, PVariable> iter(simuoutMap);
    while (iter.hasNext()) {
        iter.next();

        auto varOld = QString("%1.%2.%3.%4")
                              .arg(curDict)
                              .arg(boardName)
                              .arg(blockOldName)
                              .arg(iter.value()->getData(RoleDataDefinition::VariableCheckedRole).toString());
        ModelingServerMng::getInstance().m_pDataDictionaryServer->DelDataDictionary(varOld);

        auto varNew = QString("%1.%2.%3.%4")
                              .arg(curDict)
                              .arg(boardName)
                              .arg(blockNewName)
                              .arg(iter.value()->getData(RoleDataDefinition::VariableCheckedRole).toString());
        ModelingServerMng::getInstance().m_pDataDictionaryServer->AddDataDictionary(varNew);
    }
}