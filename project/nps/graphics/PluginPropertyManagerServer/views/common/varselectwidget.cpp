#include "varselectwidget.h"
#include "CommonModelAssistant.h"
#include "GlobalAssistant.h"
#include "KLModelDefinitionCore/KLModelDefinitionCore.h"
#include "KLWidgets/KCustomDialog.h"
#include "KLWidgets/KMessageBox.h"
#include "PropertyServerMng.h"
#include "Json/json.h"
#include <QCheckBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

static const QString PROTOTYPENAME_FMU = "FMU";
static const qreal HEADER_INIT_WIDTH = 240;
static const int HORIZONTAL_HEADER_HEIGHT = 27;
using namespace Kcc::BlockDefinition;

VarSelectWidget::VarSelectWidget(bool bcanOperation, QWidget *parent)
    : CWidget(parent),
      m_tableWidget(nullptr),
      m_bcanOperation(bcanOperation),
      m_rememberOldBlockInfo(true),
      m_elecModel(nullptr),
      m_prototype(QString())
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setMargin(0);
    m_tableWidget = new QTableWidget(this);
    mainLayout->addWidget(m_tableWidget);

    m_tableWidget->setColumnCount(2);
    m_tableWidget->setHorizontalHeaderLabels(QStringList() << CMA::HEADER_VARNAME << CMA::HEADER_DESCRIPTION);
    m_tableWidget->verticalHeader()->setVisible(false);
    m_tableWidget->verticalHeader()->setDefaultSectionSize(26);
    m_tableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    m_tableWidget->setSelectionBehavior(QAbstractItemView::SelectItems);
    m_tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    connect(m_tableWidget, SIGNAL(itemClicked(QTableWidgetItem *)), this, SLOT(onItemClicked(QTableWidgetItem *)));
    m_pHeaderView = new TableHeaderView(Qt::Horizontal, m_tableWidget);
    m_pHeaderView->setColumnCheckable(0, true);
    m_pHeaderView->setSectionResizeMode(QHeaderView::Stretch);
    m_pHeaderView->setCheckBoxEnableStatus(m_bcanOperation);
    m_pHeaderView->setMaximumHeight(27);
    connect(m_pHeaderView, SIGNAL(columuSectionClicked(int, bool)), this, SLOT(onColumnAllSelected(int, bool)));
    m_tableWidget->setHorizontalHeader(m_pHeaderView);

    setLayout(mainLayout);
}

VarSelectWidget::~VarSelectWidget() { }

bool VarSelectWidget::saveData()
{
    bool result = false;
    auto modelConnectionList = m_elecModel->getConnectedModel(ModelLineNodeLink);
    if (1 == modelConnectionList.size()) {
        if (modelConnectionList.first() != nullptr) {
            auto pCurLineComponent = modelConnectionList.first().toStrongRef().dynamicCast<ElectricalBlock>();
            result = saveData(pCurLineComponent);
        }
    }
    result = saveData(m_elecModel);

    //////////////////////跨电网关联新功能临时代码///////////////////////
    // 由于结果保存是最后一个saveData的窗口，所以在saveData中发送该信号，同步所有信息给node连接类型对象
    QString strPrototypeName = m_elecModel->getPrototypeName();
    if (!modelConnectionList.isEmpty() && strPrototypeName == "PiTypeTransmissionLinkLine") {
        emit syncLineComponentInfo(); // 同步Line元件信息给node连接类型对象
    }
    ////////////////////////////////////////////////////////////////

    return result;
}

bool VarSelectWidget::saveData(QSharedPointer<Kcc::BlockDefinition::Model> model, const QString &groupname)
{
    if (model == nullptr || PropertyServerMng::getInstance().m_projectManagerServer == nullptr
        || PropertyServerMng::getInstance().m_dataDictionaryServer == nullptr) {
        return false;
    }

    auto curDict = PropertyServerMng::getInstance().m_projectManagerServer->GetRealTimeSimulationName();
    bool bvaluechange = false;
    QStringList currentSelectlist;
    QString keyword;
    for (int i = 0; i < m_tableWidget->rowCount(); ++i) {
        keyword = m_tableWidget->item(i, 0)->data(Qt::UserRole).toString();
        if (m_tableWidget->item(i, 0)->checkState() == Qt::Checked) {
            currentSelectlist.append(keyword);
        }
    }

    PModel tmpmodel = model;
    QString realkey = "";
    for (QString keystr : currentSelectlist) {
        if (!m_oldBlockInfo.tmpInitList.contains(keystr)) {
            QVariantMap otherpromap;
            otherpromap.insert(KEY_VARIABLECHECKED, true);
            CustomModelItem item(keystr, keystr, true, RoleDataDefinition::ControlTypeCheckbox, true, "", otherpromap);
            if (ElectricalBlock::Type == model->getModelType()) {
                CMA::saveRealBlockData(model, RoleDataDefinition::ResultSaveVariables, keystr, item,
                                       CMA::SAVEDATA_CheckedVariable);
            } else if (SlotBlock::Type == model->getModelType()) {
                CMA::saveRealBlockData(model, groupname, keystr, item, CMA::SAVEDATA_CheckedVariable);
            } else if (model->getPrototypeName() == PROTOTYPENAME_FMU) {
                CMA::saveRealBlockData(model, RoleDataDefinition::OutputSignal, keystr, item,
                                       CMA::SAVEDATA_CheckedVariable);
            } else {
                tmpmodel = model;
                realkey = keystr;
                if (keystr.contains("/")) {
                    tmpmodel = model->findChildModelRecursive(m_keyMaptoUUID[keystr]);
                    realkey = keystr.section('/', -1);
                }
                CMA::saveRealBlockData(tmpmodel, RoleDataDefinition::OutputSignal, realkey, item,
                                       CMA::SAVEDATA_CheckedVariable);
                CMA::saveRealBlockData(tmpmodel, RoleDataDefinition::DiscreteStateVariable, realkey, item,
                                       CMA::SAVEDATA_CheckedVariable);
                CMA::saveRealBlockData(tmpmodel, RoleDataDefinition::ContinueStateVariable, realkey, item,
                                       CMA::SAVEDATA_CheckedVariable);
                CMA::saveRealBlockData(tmpmodel, RoleDataDefinition::Parameter, realkey, item,
                                       CMA::SAVEDATA_CheckedVariable);
            }
            bvaluechange = true;
            // 处理模型嵌套情况，除了顶层画板外，其它子模块需要用 / 组合变量名称
            QString dictvar = NPS::getDictBoardBlockVarStr(curDict, model, "", keystr);
            if (!dictvar.isEmpty()) {
                PropertyServerMng::getInstance().m_dataDictionaryServer->AddDataDictionary(dictvar);
            }
        }
    }

    for (QString keystr : m_oldBlockInfo.tmpInitList) {
        if (!currentSelectlist.contains(keystr)) {
            QVariantMap otherpromap;
            otherpromap.insert(KEY_VARIABLECHECKED, false);
            CustomModelItem item(keystr, keystr, false, RoleDataDefinition::ControlTypeCheckbox, true, "", otherpromap);
            if (ElectricalBlock::Type == model->getModelType()) {
                CMA::saveRealBlockData(model, RoleDataDefinition::ResultSaveVariables, keystr, item,
                                       CMA::SAVEDATA_CheckedVariable);
            } else if (SlotBlock::Type == model->getModelType()) {
                CMA::saveRealBlockData(model, groupname, keystr, item, CMA::SAVEDATA_CheckedVariable);
            } else if (model->getPrototypeName() == PROTOTYPENAME_FMU) {
                CMA::saveRealBlockData(model, RoleDataDefinition::OutputSignal, keystr, item,
                                       CMA::SAVEDATA_CheckedVariable);
            } else {
                tmpmodel = model;
                realkey = keystr;
                if (keystr.contains("/")) {
                    tmpmodel = model->findChildModelRecursive(m_keyMaptoUUID[keystr]);
                    realkey = keystr.section('/', -1);
                }

                CMA::saveRealBlockData(tmpmodel, RoleDataDefinition::OutputSignal, realkey, item,
                                       CMA::SAVEDATA_CheckedVariable);
                CMA::saveRealBlockData(tmpmodel, RoleDataDefinition::DiscreteStateVariable, realkey, item,
                                       CMA::SAVEDATA_CheckedVariable);
                CMA::saveRealBlockData(tmpmodel, RoleDataDefinition::ContinueStateVariable, realkey, item,
                                       CMA::SAVEDATA_CheckedVariable);
                CMA::saveRealBlockData(tmpmodel, RoleDataDefinition::Parameter, realkey, item,
                                       CMA::SAVEDATA_CheckedVariable);
            }
            // 处理模型嵌套情况，除了顶层画板外，其它子模块需要用 / 组合变量名称
            QString dictvar = NPS::getDictBoardBlockVarStr(curDict, model, "", keystr);
            if (!dictvar.isEmpty()) {
                PropertyServerMng::getInstance().m_dataDictionaryServer->DelDataDictionary(dictvar);
            }

            bvaluechange = true;
        }
    }
    return bvaluechange;
}

void VarSelectWidget::updateElecCheckedVariableInfo(const QMap<QString, Kcc::ElecSys::ElecParamDef> &allvars,
                                                    QSharedPointer<Kcc::BlockDefinition::ElectricalBlock> peblock)
{
    m_elecModel = peblock;
    if (m_tableWidget == nullptr || m_pHeaderView == nullptr) {
        return;
    }
    if (allvars.isEmpty()) {
        m_tableWidget->setRowCount(0);
        m_pHeaderView->setHeaderCheckStatus(TableHeaderView::NoChecked, false);
        return;
    }
    QList<CustomModelItem> dataList = CMA::getPropertyModelItemList(peblock, RoleDataDefinition::ResultSaveVariables);
    QStringList keysavedlist = getSavedVariableList(dataList);
    if (m_rememberOldBlockInfo) {
        m_oldBlockInfo = OldBlockVarInfo(peblock->getPrototypeName(), peblock->getParentModelName(), peblock->getName(),
                                         keysavedlist);
        m_rememberOldBlockInfo = false;
    }
    m_oldBlockInfo.tmpInitList = keysavedlist;
    QStringList sortkeylist = allvars.keys();
    qSort(sortkeylist.begin(), sortkeylist.end(),
          [&sortkeylist](const QString &lhs, const QString &rhs) -> bool { return lhs < rhs; });
    QFont font;
    font.setBold(false);
    int nSelectCount = 0;
    m_tableWidget->setRowCount(sortkeylist.size());
    for (int row = 0; row < sortkeylist.size(); ++row) {
        QTableWidgetItem *itemName = createTableWidgetItem(sortkeylist[row]);
        QTableWidgetItem *itemInfo = createTableWidgetItem(allvars[sortkeylist[row]].description);
        if (keysavedlist.contains(sortkeylist[row])) {
            itemName->setCheckState(Qt::Checked);
            ++nSelectCount;
        } else {
            itemName->setCheckState(Qt::Unchecked);
        }
        m_tableWidget->setItem(row, 0, itemName);
        m_tableWidget->setItem(row, 1, itemInfo);
    }

    if (nSelectCount == allvars.size() && nSelectCount > 0) { // 检测是否全选
        m_pHeaderView->setHeaderCheckStatus(TableHeaderView::AllChecked, true);
    } else if (0 < nSelectCount && nSelectCount < allvars.size()) {
        m_pHeaderView->setHeaderCheckStatus(TableHeaderView::PartChecked, false);
    } else {
        m_pHeaderView->setHeaderCheckStatus(TableHeaderView::NoChecked, false);
    }
}

void VarSelectWidget::updateCheckedVariableInfo(QSharedPointer<Kcc::BlockDefinition::Model> model,
                                                const QString &groupname, const bool &isInitial)
{
    m_keyMaptoUUID.clear();
    if (m_tableWidget == nullptr || m_pHeaderView == nullptr) {
        return;
    }
    QList<CustomModelItem> listdata;
    if (!groupname.isEmpty()) {
        listdata = CMA::getPropertyModelItemList(model, groupname, false, "", QStringList() << CMA::EXCLUED_NOTNUMBER);
    } else if (ControlBlock::Type == model->getModelType() || CombineBoardModel::Type == model->getModelType()) {
        if (model->getPrototypeName() == PROTOTYPENAME_FMU) {
            listdata = CMA::getPropertyModelItemList(model, RoleDataDefinition::OutputSignal, false, "",
                                                     QStringList() << CMA::EXCLUED_NOTNUMBER);
        } else if (NPS::PROTOTYPENAME_SCOPE == model->getPrototypeName()) {
            listdata = CMA::getPropertyModelItemList(model, QStringList() << RoleDataDefinition::InputSignal, false, "",
                                                     QStringList() << CMA::EXCLUED_NOTNUMBER);
        } else {
            listdata = CMA::getPropertyModelItemList(
                    model,
                    QStringList() << RoleDataDefinition::OutputSignal << RoleDataDefinition::DiscreteStateVariable
                                  << RoleDataDefinition::ContinueStateVariable << RoleDataDefinition::Parameter,
                    false, "", QStringList() << CMA::EXCLUED_NOTNUMBER);
        }
    }
    updateCheckedVariableInfo(listdata, model->getPrototypeName(), model->getParentModelName(), model->getName(),
                              isInitial);
}

void VarSelectWidget::updateCheckedVariableInfo(const QList<CustomModelItem> &listdata, const QString &prototype,
                                                const QString &boardname, const QString &blockname,
                                                const bool &isInitial)
{
    if (listdata.size() <= 0) {
        m_tableWidget->setRowCount(0);
        m_pHeaderView->setHeaderCheckStatus(TableHeaderView::NoChecked, false);
        return;
    }
    m_prototype = prototype;
    int nSelectCount = 0;
    QFont font;
    font.setBold(false);
    QStringList keysavedlist;
    m_tableWidget->setRowCount(listdata.size());
    bool titleCheck = false;
    for (int i = 0; i < listdata.size(); ++i) {
        if (listdata[i].bcanEdit) {
            titleCheck = true;
        }
        QTableWidgetItem *itemName = createTableWidgetItem(listdata[i].keywords, !listdata[i].bcanEdit);
        QTableWidgetItem *itemInfo = createTableWidgetItem(
                listdata[i].otherPropertyMap.value(KEY_DISCRIPTION).toString(), !listdata[i].bcanEdit);
        // item.keywords.section('/', -1)
        if (listdata[i].otherPropertyMap.value(KEY_VARIABLECHECKED).toBool()) {
            keysavedlist.append(listdata[i].keywords);
            itemName->setCheckState(Qt::Checked);
            ++nSelectCount;
        } else {
            itemName->setCheckState(Qt::Unchecked);
        }
        m_tableWidget->setItem(i, 0, itemName);
        m_tableWidget->setItem(i, 1, itemInfo);
        m_keyMaptoUUID.insert(listdata[i].keywords, listdata[i].otherPropertyMap[KEY_UUID].toString());

        if (m_prototype == NPS::PROTOTYPENAME_SCOPE) {
            itemName->setCheckState(Qt::Checked);
        }
    }

    if (isInitial) { // 属性窗口初始化时，才需要缓存模块初始信息
        if (m_rememberOldBlockInfo) {
            m_oldBlockInfo = OldBlockVarInfo(prototype, boardname, blockname, keysavedlist);
            m_rememberOldBlockInfo = false;
        }
        m_oldBlockInfo.tmpInitList = keysavedlist;
    }

    if (nSelectCount == listdata.size() && nSelectCount > 0) { // 检测是否全选
        m_pHeaderView->setHeaderCheckStatus(TableHeaderView::AllChecked, true);
    } else if (0 < nSelectCount && nSelectCount < listdata.size()) {
        m_pHeaderView->setHeaderCheckStatus(TableHeaderView::PartChecked, false);
    } else {
        m_pHeaderView->setHeaderCheckStatus(TableHeaderView::NoChecked, false);
    }
    if (NPS::PROTOTYPENAME_SCOPE == m_prototype) {
        m_pHeaderView->setHeaderCheckStatus(TableHeaderView::AllChecked, true);
    }
    // m_pHeaderView->setCheckBoxEnableStatus(titleCheck);
}

QList<CustomModelItem> VarSelectWidget::getModelItemList()
{
    if (m_tableWidget == nullptr) {
        return QList<CustomModelItem>();
    }
    QList<CustomModelItem> modellist;
    QString keyword = "";
    QVariantMap otherPropertyMap;
    for (int i = 0; i < m_tableWidget->rowCount(); ++i) {
        keyword = m_tableWidget->item(i, 0)->data(Qt::UserRole).toString();
        otherPropertyMap.clear();
        otherPropertyMap.insert(KEY_DISCRIPTION, m_tableWidget->item(i, 1)->data(Qt::UserRole).toString());
        if (m_tableWidget->item(i, 0)->checkState() == Qt::Checked) {
            otherPropertyMap.insert(KEY_VARIABLECHECKED, true);
            modellist.append(CustomModelItem(keyword, keyword, true, RoleDataDefinition::ControlTypeCheckbox, true, "",
                                             otherPropertyMap));
        } else {
            otherPropertyMap.insert(KEY_VARIABLECHECKED, false);
            modellist.append(CustomModelItem(keyword, keyword, false, RoleDataDefinition::ControlTypeCheckbox, true, "",
                                             otherPropertyMap));
        }
    }
    return modellist;
}

void VarSelectWidget::onColumnAllSelected(int col, bool selected)
{
    for (int row = 0; row != m_tableWidget->rowCount(); ++row) {
        if (selected) {
            m_tableWidget->item(row, col)->setCheckState(Qt::Checked);
            m_pHeaderView->setHeaderCheckStatus(TableHeaderView::AllChecked, true);
        } else {
            m_tableWidget->item(row, col)->setCheckState(Qt::Unchecked);
            m_pHeaderView->setHeaderCheckStatus(TableHeaderView::NoChecked, false);
        }
    }
}

void VarSelectWidget::onItemClicked(QTableWidgetItem *item)
{
    Q_UNUSED(item);
    int nCheckedCount = 0;
    int nUnCheckedCount = 0;

    for (int row = 0; row < m_tableWidget->rowCount(); ++row) {
        if (m_tableWidget->item(row, 0)->checkState() == Qt::Checked) {
            nCheckedCount++;
            m_pHeaderView->setHeaderCheckStatus(TableHeaderView::PartChecked, false);
        } else {
            nUnCheckedCount++;
            m_pHeaderView->setHeaderCheckStatus(TableHeaderView::PartChecked, false);
        }
    }

    // 检测是否全选或全取消
    if (nCheckedCount == m_tableWidget->rowCount()) {
        m_pHeaderView->setHeaderCheckStatus(TableHeaderView::AllChecked, true);
    } else if (nUnCheckedCount == m_tableWidget->rowCount()) {
        m_pHeaderView->setHeaderCheckStatus(TableHeaderView::NoChecked, false);
    }
}

QStringList VarSelectWidget::getSavedVariableList(const QList<CustomModelItem> listdata)
{
    QStringList keysavedlist;
    for (CustomModelItem item : listdata) {
        if (item.isValid() && item.otherPropertyMap[KEY_VARIABLECHECKED].toBool()) {
            keysavedlist.append(item.keywords);
        }
    }
    return keysavedlist;
}

QTableWidgetItem *VarSelectWidget::createTableWidgetItem(const QString &namestr, bool bReadOnly)
{
    QTableWidgetItem *itemName = new QTableWidgetItem(namestr);
    itemName->setToolTip(namestr);
    itemName->setData(Qt::UserRole, namestr);
    if (!m_bcanOperation || bReadOnly) {
        itemName->setFlags(Qt::NoItemFlags);
        itemName->setTextColor(QColor(NPS::Color_NotEditable));
    }
    return itemName;
}

// bool VarSelectWidget::saveOutputVars(PBlock pblock)
// {
// if (pblock == nullptr) {
//     return false;
// }

// QString blockname = pblock->getName();
// bool bvaluechange = false;
// if (m_dataDictionaryServer == nullptr) {
//     return bvaluechange;
// }
// using namespace Kcc::DataManager;
// PIDataManagerServer pDataMgr = RequestServer<IDataManagerServer>();
// auto curDict = pDataMgr->GetRealTimeSimulationName();

// // 删除旧的数据
// if (pblock->getPrototypeName() != m_oldBlockInfo.prototypeName || blockname != m_oldBlockInfo.blockName) {
//     foreach (QString str, m_oldBlockInfo.oldOutPutList) {
//         auto var = QString("%1.%2.%3.%4")
//                            .arg(curDict)
//                            .arg(m_oldBlockInfo.boardName)
//                            .arg(m_oldBlockInfo.blockName)
//                            .arg(str);
//         m_dataDictionaryServer->DelDataDictionary(var);
//     }
// }

// QMap<QString, Block::SimuOutputVarParameter> curselectVarsMap;
// for (int i = 0; i < m_tableWidget->rowCount(); i++) {
//     QString strKey;
//     QString strName(m_tableWidget->item(i, 0)->data(Qt::UserRole).toString());
//     if (pblock->getBlockType() == Block::BlockType::BlockElec) {
//         strKey = m_tableWidget->item(i, 0)->data(Qt::UserRole).toString();
//     } else {
//         strKey = m_tableWidget->item(i, 0)->data(Qt::UserRole + 1).toString();
//     }

//     if (m_tableWidget->item(i, 0)->checkState() == Qt::Checked) {
//         Block::SimuOutputVarParameter simuparam;
//         simuparam.keyWords = strKey;
//         simuparam.selectSaveTime = QDateTime::currentDateTime();
//         simuparam.varName = strName;
//         curselectVarsMap.insert(strKey, simuparam);
//         auto var = QString("%1.%2.%3.%4").arg(curDict).arg(m_oldBlockInfo.boardName).arg(blockname).arg(strName);
//         if (!pblock->getSimuOutputVarMap().contains(strKey)) {
//             pblock->setSimuOutputVarParameter(strKey, simuparam);
//             m_dataDictionaryServer->AddDataDictionary(var);
//             bvaluechange = true;
//         }
//     } else {
//         if (pblock->getSimuOutputVarMap().contains(strKey)) {
//             pblock->removeSimuOutputVarParameter(strKey);
//             auto var =
//                     QString("%1.%2.%3.%4").arg(curDict).arg(m_oldBlockInfo.boardName).arg(blockname).arg(strName);
//             m_dataDictionaryServer->DelDataDictionary(var);
//             bvaluechange = true;
//         }
//     }
// }
// // 有类似母线单项三项变化了，参数个数变化，会导致block中的simuoutput残留。再次去除。
// foreach (QString keyword, pblock->getSimuOutputVarMap().keys()) {
//     if (!curselectVarsMap.contains(keyword)) {
//         pblock->removeSimuOutputVarParameter(keyword);
//         auto var = QString("%1.%2.%3.%4").arg(curDict).arg(m_oldBlockInfo.boardName).arg(blockname).arg(keyword);
//         m_dataDictionaryServer->DelDataDictionary(var);
//         bvaluechange = true;
//     }
// }
//     return bvaluechange;
// }

void VarSelectWidget::setCWidgetReadOnly(bool bReadOnly)
{
    if (!m_bcanOperation || m_pHeaderView == nullptr || m_tableWidget == nullptr) {
        return;
    }
    bool isReadOnly = bReadOnly;
    if (NPS::PROTOTYPENAME_SCOPE == m_prototype) {
        isReadOnly = true;
    }
    m_pHeaderView->setCheckBoxEnableStatus(!isReadOnly);
    QTableWidgetItem *itemName = new QTableWidgetItem("inititem");
    for (int row = 0; row < m_tableWidget->rowCount(); ++row) {
        for (int col = 0; col < m_tableWidget->columnCount(); ++col) {
            if (m_tableWidget->item(row, col) == nullptr) {
                continue;
            }
            if (isReadOnly) {
                m_tableWidget->item(row, col)->setFlags(Qt::NoItemFlags);
                m_tableWidget->item(row, col)->setTextColor(QColor(NPS::Color_NotEditable));
            } else {
                m_tableWidget->item(row, col)->setFlags(itemName->flags());
                m_tableWidget->item(row, col)->setTextColor(itemName->textColor());
            }
        }
    }
}

QStringList VarSelectWidget::getNewSavedList()
{
    if (m_tableWidget == nullptr) {
        return QStringList();
    }
    QStringList savedlist;
    QString keyword;
    for (int i = 0; i < m_tableWidget->rowCount(); ++i) {
        keyword = m_tableWidget->item(i, 0)->data(Qt::UserRole).toString();
        if (m_tableWidget->item(i, 0)->checkState() == Qt::Checked) {
            savedlist.append(keyword);
        }
    }
    return savedlist;
}

void VarSelectWidget::setCanOperation(bool canope)
{
    m_bcanOperation = canope;
    if (m_pHeaderView != nullptr) {
        m_pHeaderView->setCheckBoxEnableStatus(canope);
    }
}
