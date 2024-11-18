#include "FaultSimuWidget.h"

#include "CommonModelAssistant.h"
#include "CoreLib/ServerManager.h"
#include "GlobalAssistant.h"
#include "GraphicsModelingKernel/CanvasContext.h"
#include "KLModelDefinitionCore/KLModelDefinitionCore.h"
#include "PropertyServerMng.h"

using namespace Kcc::BlockDefinition;

static const QString KEYWORD_TITLE_ELEMENTNAME = QObject::tr("Element Name");     // 模块名
static const QString KEYWORD_TITLE_BOARDNAME = QObject::tr("Drawing Board Name"); // 画板名称
static const QString KEYWORD_TITLE_OUTOFSERVICE = QObject::tr("Out of Service");  // 挂起

USE_LOGOUT_("FaultSimuWidget")

FaultSimuWidget::FaultSimuWidget(QWidget *parent) : CWidget(parent), m_tableView(nullptr)
{
    initUI();
}

FaultSimuWidget::~FaultSimuWidget() { }

void FaultSimuWidget::onRecieveSimulationServerManagerMsg(unsigned int code, const NotifyStruct &param)
{
    if (Notify_DrawingBoardRunning == code) {
        m_tableModel->setBoardRunningSts(true);
    } else if (Notify_DrawingBoardStopped == code) {
        m_tableModel->setBoardRunningSts(false);
    }
}

void FaultSimuWidget::initUI()
{
    QGridLayout *pGridLayout = new QGridLayout(this);
    pGridLayout->setMargin(10);
    m_tableView = new TableView(this);
    m_tableView->setObjectName("FaultSimuWidget_TableView");
    m_tableView->setAccessibleName("FaultSimuWidget_TableView");
    m_tableView->verticalHeader()->setVisible(false);
    m_tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    m_tableView->horizontalHeader()->setStretchLastSection(true);

    m_tableView->setItemDelegate(new CustomTableItemDelegate(this));
    bool isBoardRunning = false;
    if (nullptr != PropertyServerMng::getInstance().m_simulationManagerServer) {
        ISimulationManagerServer::RunningState runstate =
                PropertyServerMng::getInstance().m_simulationManagerServer->GetSimulationState();
        if (ISimulationManagerServer::Disabled == runstate || ISimulationManagerServer::Stopped == runstate) {
            isBoardRunning = false;
        } else {
            isBoardRunning = true;
        }
    }
    m_tableModel = new FaultSimuModel(isBoardRunning, m_tableView);
    m_tableModel->setModelData(getBreakerList());
    m_tableView->setModel(m_tableModel);
    QStringList headerList = m_tableModel->getHeaderList();
    for (int i = 0; i < headerList.size(); ++i) {
        QFontMetrics fontWidth(m_tableView->horizontalHeader()->font());
        m_tableView->setColumnWidth(i, qRound64(fontWidth.width(headerList[i]) * 1.1));
    }
    pGridLayout->addWidget(m_tableView, 0, 0, 1, 1);
    if (PropertyServerMng::getInstance().m_simulationManagerServerIF != nullptr) {
        PropertyServerMng::getInstance().m_simulationManagerServerIF->connectNotify(
                Notify_DrawingBoardRunning, this,
                SLOT(onRecieveSimulationServerManagerMsg(unsigned int, const NotifyStruct &)));
        PropertyServerMng::getInstance().m_simulationManagerServerIF->connectNotify(
                Notify_DrawingBoardStopped, this,
                SLOT(onRecieveSimulationServerManagerMsg(unsigned int, const NotifyStruct &)));
    }
}

QList<QSharedPointer<Kcc::BlockDefinition::Model>> FaultSimuWidget::getBreakerList()
{
    PKLProject curProject = CMA::getCurProject();
    if (curProject == nullptr) {
        return QList<PModel>();
    }
    QList<PModel> models = curProject->getAllModel(ElecBoardModel::Type);
    QVariantMap boardactivemap;
    if (curProject->getProjectConfig(KL_PRO::BOARD_ISACTIVATE).canConvert<QVariantMap>()) {
        boardactivemap = curProject->getProjectConfig(KL_PRO::BOARD_ISACTIVATE).value<QVariantMap>();
    }
    QList<PModel> breakerModels;
    for (PModel eleboard : models) {
        if (eleboard == nullptr || !boardactivemap.value(eleboard->getUUID(), false).toBool()) {
            continue;
        }
        for (PModel cmodel : eleboard->getChildModels()) {
            if (cmodel != nullptr && cmodel->getParentModel() != nullptr
                && cmodel->getPrototypeName() == NPS::PROTOTYPENAME_CIRCUITBREAKER
                && cmodel->getParentModel()->getAdjacentConnector(cmodel->getUUID()).size() > 0) {
                breakerModels.append(cmodel);
            }
        }
    }
    return breakerModels;
}

FaultSimuModel::FaultSimuModel(bool isboardRunning, QObject *parent) : m_isBoardRunning(isboardRunning) { }

FaultSimuModel::~FaultSimuModel() { }

void FaultSimuModel::setBoardRunningSts(bool boardRunning)
{
    beginResetModel();
    m_isBoardRunning = boardRunning;
    endResetModel();
}

int FaultSimuModel::rowCount(const QModelIndex &parent) const
{
    return m_data.count();
}

int FaultSimuModel::columnCount(const QModelIndex &parent) const
{
    return m_columnItems.size();
}

QVariant FaultSimuModel::data(const QModelIndex &index, int role) const
{
    if (!isDataValid(index)) {
        return QVariant();
    }
    if ((Qt::DisplayRole == role || Qt::EditRole == role || Qt::ToolTipRole == role)
        && index.data(NPS::ModelDataTypeRole).toString() != RoleDataDefinition::ControlTypeCheckbox) {
        return NPS::RealValueMaptoControlValue(index.data(NPS::ModelDataRealValueRole),
                                               index.data(NPS::ModelDataTypeRole).toString(),
                                               index.data(NPS::ModelDataRangeRole));
    } else if (Qt::CheckStateRole == role
               && index.data(NPS::ModelDataTypeRole).toString() == RoleDataDefinition::ControlTypeCheckbox) {
        return NPS::RealValueMaptoControlValue(index.data(NPS::ModelDataRealValueRole),
                                               RoleDataDefinition::ControlTypeCheckbox, "");
    } else if (NPS::ModelDataRealValueRole == role) {
        if (KEYWORD_TITLE_ELEMENTNAME == m_columnItems[index.column()].Keywords) {
            return m_data[index.row()]->getName();
        } else if (KEYWORD_TITLE_BOARDNAME == m_columnItems[index.column()].Keywords) {
            return m_data[index.row()]->getParentModelName();
        } else if (KEYWORD_TITLE_OUTOFSERVICE == m_columnItems[index.column()].Keywords) {
            return isModelCubicOff(m_data[index.row()]);
        } else {
            return m_data[index.row()]->getValueByRoleData(RoleDataDefinition::ElectricalParameter,
                                                           RoleDataDefinition::NameRole,
                                                           m_columnItems[index.column()].Keywords);
        }
    } else if (NPS::ModelDataKeywordsRole == role) {
        return m_columnItems[index.column()].Keywords;
    } else if (NPS::ModelDataTypeRole == role) {
        return m_columnItems[index.column()].DataType;
    } else if (NPS::ModelDataRangeRole == role) {
        return m_columnItems[index.column()].DataRange;
    } else if (NPS::ModelDataEditRole == role) {
        if (m_isBoardRunning) {
            return false;
        }
        if (KEYWORD_TITLE_ELEMENTNAME == m_columnItems[index.column()].Keywords) {
            return false;
        } else if (KEYWORD_TITLE_BOARDNAME == m_columnItems[index.column()].Keywords) {
            return false;
        } else if (KEYWORD_TITLE_OUTOFSERVICE == m_columnItems[index.column()].Keywords) {
            if (m_data[index.row()]->getParentModel() != nullptr
                && m_data[index.row()]->getParentModel()->getCanvasContext() != nullptr) {
                return !m_data[index.row()]->getParentModel()->getCanvasContext()->getLocked();
            }
            return true;
        }
        PVariable pvar = m_data[index.row()]->getVariableByRoleData(RoleDataDefinition::ElectricalParameter,
                                                                    RoleDataDefinition::NameRole,
                                                                    m_columnItems[index.column()].Keywords);
        if (pvar == nullptr) {
            return false;
        } else {
            return Variable::VisiableWritable == pvar->getShowMode();
        }
    } else if (Qt::TextColorRole == role) {
        if (!index.data(NPS::ModelDataEditRole).toBool()) {
            return QColor(NPS::Color_NotEditable);
        }
    }
    return QVariant();
}

QVariant FaultSimuModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (section >= m_columnItems.count()) {
        return QVariant();
    }
    if (orientation == Qt::Horizontal) {
        if (Qt::DisplayRole == role) {
            return m_columnItems[section].TitleName;
        } else if (Qt::UserRole == role) {
            return m_columnItems[section].TitleName;
        }
    }
    return QVariant();
}

Qt::ItemFlags FaultSimuModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }

    if (index.data(NPS::ModelDataTypeRole).toString() == RoleDataDefinition::ControlTypeCheckbox) {
        if (index.data(NPS::ModelDataEditRole).toBool()) {
            return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;
        } else {
            return Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;
        }
    } else if (index.data(NPS::ModelDataEditRole).toBool()) {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    }
    return QAbstractTableModel::flags(index);
}

bool FaultSimuModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!isDataValid(index)) {
        return false;
    }
    if (Qt::CheckStateRole != role && Qt::EditRole != role) {
        return false;
    }

    if (Qt::CheckStateRole == role) {
        bool oldValue = index.data(NPS::ModelDataRealValueRole).toBool();
        bool newvalue = (Qt::CheckState(value.toInt()) == Qt::Checked);
        setCubicSts(m_data[index.row()], newvalue);
        LOGOUT(NPS::outputLog(QObject::tr("Electrical Model"), m_data[index.row()]->getName(),
                              m_columnItems[index.column()].TitleName, oldValue, newvalue,
                              index.data(NPS::ModelDataKeywordsRole).toString(), m_columnItems[index.column()].DataType,
                              m_columnItems[index.column()].DataRange));
        return true;
    } else if (Qt::EditRole == role) {
        QVariant realValue = value;
        QString datatype = index.data(NPS::ModelDataTypeRole).toString();
        if ((NPS::isNumberDataType(datatype) && realValue.toString().trimmed().isEmpty())) {
            LOGOUT(NPS::LOG_VALUENULLSTRING(TITLE_FAULT_SIMU, index.sibling(index.row(), 0).data().toString(),
                                            headerData(index.column(), Qt::Horizontal, Qt::UserRole).toString()),
                   LOG_WARNING);
            return false;
        }
        QVariant oldvalue = index.data(NPS::ModelDataRealValueRole);
        if (realValue == oldvalue) {
            return false;
        }
        if (m_data[index.row()] == nullptr) {
            return false;
        }
        PVariable pvar = m_data[index.row()]->getVariableByRoleData(RoleDataDefinition::ElectricalParameter,
                                                                    RoleDataDefinition::NameRole,
                                                                    index.data(NPS::ModelDataKeywordsRole).toString());
        if (pvar == nullptr) {
            return false;
        }
        m_data[index.row()]->setVariableData(pvar, RoleDataDefinition::ValueRole, realValue);
        LOGOUT(NPS::outputLog(QObject::tr("Electrical Model"), m_data[index.row()]->getName(),
                              pvar->getDisplayName().isEmpty() ? pvar->getName() : pvar->getDisplayName(), oldvalue,
                              realValue, index.data(NPS::ModelDataKeywordsRole).toString(), pvar->getControlType(),
                              pvar->getControlValue()));
        return true;
    }
    return false;
}

void FaultSimuModel::setModelData(const QList<QSharedPointer<Kcc::BlockDefinition::Model>> &modellist)
{
    beginResetModel();
    m_data.clear();
    m_columnItems.clear();
    m_data = modellist;
    m_columnItems.append(FaultSimuModelItem(KEYWORD_TITLE_ELEMENTNAME, KEYWORD_TITLE_ELEMENTNAME,
                                            RoleDataDefinition::ControlTypeTextbox));
    m_columnItems.append(FaultSimuModelItem(KEYWORD_TITLE_BOARDNAME, KEYWORD_TITLE_BOARDNAME,
                                            RoleDataDefinition::ControlTypeTextbox));

    PElectricalBlock pemodel = PropertyServerMng::getInstance()
                                       .m_projectManagerServer->CreateBlockModel(NPS::PROTOTYPENAME_CIRCUITBREAKER)
                                       .dynamicCast<ElectricalBlock>();
    QList<PVariable> varlist = CMA::getVarGroupList(pemodel, RoleDataDefinition::ElectricalParameter);
    for (PVariable pvar : varlist) {
        if (pvar == nullptr) {
            continue;
        }
        m_columnItems.append(FaultSimuModelItem(
                pvar->getName(), pvar->getDisplayName().isEmpty() ? pvar->getName() : pvar->getDisplayName(),
                pvar->getControlType(), pvar->getControlValue()));
    }
    m_columnItems.append(FaultSimuModelItem(KEYWORD_TITLE_OUTOFSERVICE, KEYWORD_TITLE_OUTOFSERVICE,
                                            RoleDataDefinition::ControlTypeCheckbox));
    endResetModel();
}

QStringList FaultSimuModel::getHeaderList() const
{
    QStringList headList;
    for (FaultSimuModelItem item : m_columnItems) {
        headList.append(item.TitleName);
    }
    return headList;
}

bool FaultSimuModel::isDataValid(const QModelIndex &index) const
{
    if (!index.isValid() || index.column() >= m_columnItems.size() || index.row() >= m_data.count()
        || m_data[index.row()] == nullptr) {
        return false;
    }
    return true;
}

bool FaultSimuModel::isModelCubicOff(QSharedPointer<Kcc::BlockDefinition::Model> model) const
{
    if (model == nullptr || model->getParentModel() == nullptr) {
        return true;
    }

    QList<PBlockConnector> adjConnecters = model->getParentModel()->getAdjacentConnector(model->getUUID());
    if (adjConnecters.size() <= 0) {
        return true;
    }
    for (PBlockConnector con : adjConnecters) {
        if (con == nullptr || !con->getIsCubicOn()) {
            return true;
        }
    }
    return false;
}

void FaultSimuModel::setCubicSts(QSharedPointer<Kcc::BlockDefinition::Model> model, bool cubicOff)
{
    if (model == nullptr || model->getParentModel() == nullptr) {
        return;
    }
    QList<PBlockConnector> adjConnecters = model->getParentModel()->getAdjacentConnector(model->getUUID());
    if (adjConnecters.size() <= 0) {
        return;
    }
    PCanvasContext canvas = model->getParentModel()->getCanvasContext();
    if (canvas == nullptr) {
        return;
    }

    for (PBlockConnector con : adjConnecters) {
        if (con != nullptr) {
            con->setIsCubicOn(!cubicOff);
            auto conwire = canvas->getConnectorWireContext(con->getUUID());
            if (conwire != nullptr) {
                conwire->setConnected(!cubicOff);
            }
        }
    }
    if (PropertyServerMng::getInstance().m_pElecModelCheckServer != nullptr) {
        PropertyServerMng::getInstance().m_pElecModelCheckServer->VerifyDrawingBoardStatus(
                model->getParentModel().dynamicCast<DrawingBoardClass>());
    }
}
