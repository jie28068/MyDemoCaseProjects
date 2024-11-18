#include "CurrentSimuWidget.h"
#include "CommonModelAssistant.h"
#include "CoreLib/ServerManager.h"
#include "GlobalAssistant.h"
#include "PropertyServerMng.h"
#include "PropertyTableModel.h"
#include "QPHelper.h"
#include "server/PropertyManagerServer/IPropertyManagerServer.h"
#include "server/SimuNPSAdapter/ISimuNPSDataDictionaryServer.h"

#include <QElapsedTimer>
using namespace Kcc::SimuNPSAdapter;
using namespace Kcc::PropertyManager;
using namespace Kcc::BlockDefinition;

USE_LOGOUT_("CurrentSimuWidget")

static const int TREEROLE_BLOCKPROTOTYPE = Qt::DisplayRole + 500;
static const int TREEROLE_ITEMTYPE = Qt::DisplayRole + 501;  // item类型是画板还是原型
static const int TREEROLE_BOARDNAME = Qt::DisplayRole + 502; // 给blockitem用，所属画板名
static const int TREEROLE_NODEPATH = Qt::DisplayRole + 503;  // 树形列表中item的节点路径

static const QString TREE_ITEMTYPE_BOARD = QString("Board");         // 当前item为画板
static const QString TREE_ITEMTYPE_PROTOTYPE = QString("Prototype"); // 当前item为原型名

static const QString TABLE_TITLE_MONITOR = QObject::tr("Monitor");               // 监控
static const QString TABLE_TITLE_BLOCKNAME = QObject::tr("Module Name");         // 模块名
static const QString TABLE_TITLE_VARNAME = QObject::tr("Variable Name");         // 变量名
static const QString TABLE_TITLE_VARTYPE = QObject::tr("Variable Type");         // 变量类型
static const QString TABLE_TITLE_VARALIAS = QObject::tr("Variable Alias");       // 变量别名
static const QString TABLE_TITLE_DYNAMICADJ = QObject::tr("Dynamic Adjustment"); // 动态调参
static const QString TABLE_TITLE_MODIFYTIME = QObject::tr("Checked Time");       // 勾选时间

CurrentSimuWidget::CurrentSimuWidget(const QString &projname, QWidget *parent)
    : QMainWindow(parent),
      m_ptreeModel(nullptr),
      m_pTableModel(nullptr),
      m_tableProxyModel(nullptr),
      m_itemLabel(nullptr),
      m_projectName(projname),
      m_isElecboardActived(false)
{
    ui.setupUi(this);
    InitUI();
    ui.searchBox->installEventFilter(this);
}

CurrentSimuWidget::~CurrentSimuWidget() { }

void CurrentSimuWidget::initData()
{
    if (m_pTableModel == nullptr || m_ptreeModel == nullptr || m_tableProxyModel == nullptr
        || PropertyServerMng::getInstance().m_projectManagerServer == nullptr) {
        return;
    }
    // // TODO:别名功能
    // QMap<QString, bool> boardActiveStsmap;
    // QVariant variant =
    // PropertyServerMng::getInstance().m_pModelDataMngServer->GetProjectConfig(KL_PRO::BOARD_ISACTIVATE); if
    // (!variant.isNull() && variant.type() == QVariant::Map) {
    //     boardActiveStsmap = variant.value<QMap<QString, bool>>();
    // }

    m_ptreeModel->clear();
    m_pTableModel->clear();

    if (m_itemLabel != nullptr) {
        m_itemLabel->setText(tr("%1 items").arg(QString::number(m_tableProxyModel->rowCount())));
    }

    QList<PModel> alldrawboards = PropertyServerMng::getInstance().m_projectManagerServer->GetAllBoardModel();
    if (alldrawboards.size() <= 0) {
        return;
    }

    m_boardActiveStsMap =
            PropertyServerMng::getInstance().m_projectManagerServer->GetProjectConfig(KL_PRO::BOARD_ISACTIVATE).toMap();

    foreach (PModel pDrawboard, alldrawboards) {
        if (pDrawboard == nullptr) {
            continue;
        }

        if (pDrawboard->getModelType() == ControlBoardModel::Type
            || pDrawboard->getModelType() == ElecBoardModel::Type) {
            initBoardData(pDrawboard);
        } else if (pDrawboard->getModelType() == ComplexBoardModel::Type) {
            QList<PModel> complexInstancesList = pDrawboard->getInstanceList();
            for (auto complexInstance : complexInstancesList) {
                initBoardData(complexInstance);
            }
        }
    }

    m_ptreeModel->sort(0);
    m_pTableModel->initData(m_allVarMap);
    ui.colCombobox->clear();
    ui.colCombobox->addItems(m_pTableModel->getHeaderList());
    ui.colCombobox->setCurrentIndex(1);
    QModelIndex index = m_ptreeModel->index(0, 0);
    ui.treeView->setCurrentIndex(index);
    onClickedTreeItem(index);
}

void CurrentSimuWidget::saveVariableData()
{
    if (m_pTableModel == nullptr || PropertyServerMng::getInstance().m_projectManagerServer == nullptr
        || PropertyServerMng::getInstance().m_pGraphicsModelingServer == nullptr) {
        return;
    }

    QMap<QString, QList<VarTableItem>> newVarMap = m_pTableModel->getNewAllVarMap();
    QList<PDrawingBoardClass> needSaveDrawboardsList;
    QList<PModel> allboards = m_lstAllboards;

    foreach (QString boardnames, newVarMap.keys()) {
        QString boardname = boardnames.split("/").last();
        foreach (PModel pdrawboard, allboards) {
            if (pdrawboard != nullptr && pdrawboard->getName() == boardname) {
                QList<bool> isChangeFlags;
                foreach (VarTableItem item, newVarMap.value(boardnames)) {
                    if (item.boardNameKey.keywords == pdrawboard->getUUID()) {
                        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
                        bool ischange = saveDrawboardData(pdrawboard, boardnames, newVarMap);
                        isChangeFlags.append(ischange);
                        QApplication::restoreOverrideCursor();
                        break;
                    }
                }

                if (isChangeFlags.contains(true)) {
                    // 保存画板数据
                    if (PropertyServerMng::getInstance().m_pGraphicsModelingServer->hasDrawingBoardById(
                                pdrawboard->getUUID())) {
                        PIPropertyManagerServer pPropertyServer = RequestServer<IPropertyManagerServer>();
                        if (pPropertyServer == nullptr) {
                            LOGOUT(tr("IPropertyManagerServer unregistered"), LOG_ERROR);
                            return;
                        }
                        QMap<QString, QVariant> paramas;
                        paramas[PMKEY::MODEL_UUID] = pdrawboard->getUUID();
                        paramas[PMKEY::MODEL_TYPE] = pdrawboard->getModelType();
                        pPropertyServer->sendNotify(Notify_BlockSimuOutParamAliasChanged, paramas);
                    }
                    CMA::saveModel(pdrawboard); // fixme
                                                // 此接口保存的pDrawboard如果为实例，则实际保存的是实例对应的模板
                                                // 20230616
                }
            }
        }
    }
}

void CurrentSimuWidget::InitUI()
{
    setObjectName("CurrentSimuWidget");
    ui.colCombobox->setAccessibleName("CurrentSimuWidget_ColCombobox");
    ui.searchBox->setAccessibleName("CurrentSimuWidget_SearchBox");

    m_itemLabel = new QLabel;
    ui.statusbar->addWidget(m_itemLabel);
    ui.statusbar->setSizeGripEnabled(false);
    // m_rowlabel->setFont(font);

    m_ptreeModel = new QStandardItemModel(ui.treeView);

    m_pTableModel = new VarTableModel(m_projectName, ui.tableView);

    m_tableProxyModel = new CustomSortFilterModel(ui.tableView);
    m_tableProxyModel->setSourceModel(m_pTableModel);
    m_tableProxyModel->setDynamicSortFilter(true);

    ui.tableView->setMouseTracking(false); // 关闭表格鼠标跟踪更能功能
    ui.tableView->setSortingEnabled(false); // 关闭表格的排序功能，只通过表头选中不同的列进行设true打开
    ui.tableView->setModel(m_tableProxyModel);
    ui.tableView->horizontalHeader()->setStretchLastSection(true);
    ui.tableView->horizontalHeader()->setSortIndicatorShown(false);    // 关闭表头中的排序箭头指示
    ui.tableView->setEditTriggers(QAbstractItemView::DoubleClicked);   // 双击表格项可编辑（别名）
    ui.tableView->setItemDelegateForColumn(0, new CheckBoxDelegate);   // 设置表格项的复选框代理
    ui.tableView->setItemDelegate(new CustomTableItemDelegate(this));  // 设置表格项代理
    ui.tableView->setSelectionBehavior(QAbstractItemView::SelectRows); // 单击选中整行
    ui.tableView->setSelectionMode(QAbstractItemView::SelectionMode::ExtendedSelection); // 开启多选

    m_pModel = new QStandardItemModel(ui.tableView);
    m_pModel->setHorizontalHeaderLabels(QStringList()
                                        << TABLE_TITLE_MONITOR << TABLE_TITLE_BLOCKNAME << TABLE_TITLE_VARNAME
                                        << TABLE_TITLE_VARTYPE << TABLE_TITLE_MODIFYTIME);

    m_pHeaderView = new TableHeaderView(Qt::Horizontal, ui.tableView);
    m_pHeaderView->setModel(m_pModel);
    m_pHeaderView->setColumnCheckable(0, true);
    m_pHeaderView->setCheckBoxEnableStatus(false);
    m_pHeaderView->setMaximumHeight(27);
    m_pHeaderView->setSectionResizeMode(0, QHeaderView::Fixed);
    ui.tableView->setHorizontalHeader(m_pHeaderView);

    QAbstractButton *cornerButton = ui.tableView->findChild<QAbstractButton *>(); // 获取表格左上角按钮

    ui.treeView->expandAll();
    ui.treeView->setModel(m_ptreeModel);
    ui.treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QObject::connect(ui.tableView->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(slotHeaderClicked(int)));
    QObject::connect(ui.tableView, SIGNAL(clicked(const QModelIndex &)), this,
                     SLOT(onTableItemClicked(const QModelIndex &)));
    QObject::connect(ui.tableView, SIGNAL(selectedRowsChanged(const int &)), this,
                     SLOT(onTableSelectedRowsChanged(const int &)));
    QObject::connect(ui.tableView, SIGNAL(selectedIndexsChanged(QModelIndexList &)), this,
                     SLOT(onGetSelectedIndexs(QModelIndexList &)));

    QObject::connect(m_pHeaderView, SIGNAL(columuSectionClicked(int, bool)), this,
                     SLOT(onChangeColSelectStatus(int, bool)));

    QObject::connect(ui.treeView, SIGNAL(clicked(const QModelIndex &)), this,
                     SLOT(onClickedTreeItem(const QModelIndex &)));

    QObject::connect(ui.colCombobox, SIGNAL(currentIndexChanged(int)), this, SLOT(onSearchColBoxChanged(int)));

    QObject::connect(ui.searchBox, SIGNAL(currentIndexChanged(const QString &)), this,
                     SLOT(onSearchTextChanged(const QString &)));
    QObject::connect(ui.searchBox, SIGNAL(editTextChanged(const QString &)), this,
                     SLOT(onSearchTextChanged(const QString &)));

    QObject::connect(m_pTableModel, SIGNAL(changeHeaderViewEnabled(bool)), this, SLOT(onChangeHeaderViewEnabled(bool)));

    if (cornerButton) {
        QObject::connect(cornerButton, &QAbstractButton::clicked, this, &CurrentSimuWidget::onTableSelectedAllRows);
    }
}

void CurrentSimuWidget::initBoardData(PModel drawboard)
{
    PDrawingBoardClass pDrawboard = drawboard.dynamicCast<DrawingBoardClass>();
    if (pDrawboard == nullptr) {
        return;
    }
    m_lstAllboards.append(pDrawboard);
    m_isElecboardActived = m_boardActiveStsMap.value(pDrawboard->getUUID(), false).toBool();

    QString strModelName;
    QString strLanguage = QPHelper::getLanguage();

    QStandardItem *boarditem = new QStandardItem();
    boarditem->setText(pDrawboard->getName());
    boarditem->setData(TREE_ITEMTYPE_BOARD, TREEROLE_ITEMTYPE);
    boarditem->setData(boarditem->text(), TREEROLE_BOARDNAME);
    boarditem->setData(boarditem->text(), TREEROLE_NODEPATH);
    m_ptreeModel->appendRow(boarditem);

    foreach (PModel pblock, pDrawboard->getChildModels().values()) {
        if (pblock == nullptr || Block::Type == pblock->getModelType()
            || ElectricalContainerBlock::Type == pblock->getModelType()) {
            continue;
        }

        getSlotConnectionsModel(pblock);
        if (ElectricalBlock::Type == pblock->getModelType() && pDrawboard->getModelType() != ComplexBoardModel::Type) {
            PElectricalBlock peblock = pblock.dynamicCast<ElectricalBlock>();
            if (peblock == nullptr) {
                continue;
            }

            QMap<QString, Kcc::ElecSys::ElecParamDef> outputparamMap =
                    PropertyServerMng::getInstance().m_pIElecSysServer->GetOutputParamMap(peblock->getPrototypeName(),
                                                                                          peblock->getPhaseNumber());
            if (outputparamMap.isEmpty()) {
                continue;
            }

            QString busbarprototypeNamechs =
                    PropertyServerMng::getInstance().m_projectManagerServer->GetModelPrototypeNameCHS(
                            NPS::PROTOTYPENAME_BUSBAR);
            QPixmap pixmap = PropertyServerMng::getInstance().m_ElectricalComponentServer->GetComponentIcon(
                    peblock->getPrototypeName(), BLOCKTYPE_PROTOTYPE);

            QStandardItem *subitem;
            if (peblock->getPrototypeName() == NPS::PROTOTYPENAME_DOTBUSBAR) {
                if (strLanguage != "zh_CN") {
                    busbarprototypeNamechs = NPS::PROTOTYPENAME_BUSBAR;
                }

                if (!checkedChildItemText(boarditem, busbarprototypeNamechs)) {
                    subitem = new QStandardItem(pixmap, busbarprototypeNamechs);
                    subitem->setData(TREE_ITEMTYPE_PROTOTYPE, TREEROLE_ITEMTYPE);
                    subitem->setData(NPS::PROTOTYPENAME_BUSBAR, TREEROLE_BLOCKPROTOTYPE);
                    subitem->setData(boarditem->text(), TREEROLE_BOARDNAME);
                    boarditem->appendRow(subitem);
                }
            } else {
                strModelName = peblock->getPrototypeName_CHS();
                if (!checkedChildItemText(boarditem, strModelName)) {
                    subitem = new QStandardItem(pixmap, strModelName);
                    subitem->setData(TREE_ITEMTYPE_PROTOTYPE, TREEROLE_ITEMTYPE);
                    subitem->setData(peblock->getPrototypeName(), TREEROLE_BLOCKPROTOTYPE);
                    subitem->setData(boarditem->text(), TREEROLE_BOARDNAME);
                    boarditem->appendRow(subitem);
                }
            }

            QList<VarTableItem> itemsList = getEblockModelList(pDrawboard, peblock, m_isElecboardActived);
            m_allVarMap[pDrawboard->getName()] << itemsList;
        } else if (ControlBlock::Type == pblock->getModelType()) {
            PControlBlock pcblock = pblock.dynamicCast<ControlBlock>();
            if (pcblock == nullptr) {
                continue;
            }

            QPixmap pixmap = PropertyServerMng::getInstance().m_ControlComponentServer->GetComponentIcon(
                    pcblock->getPrototypeName(), BLOCKTYPE_PROTOTYPE);

            strModelName = pcblock->getPrototypeName_Readable();
            if (!checkedChildItemText(boarditem, strModelName)) {
                QStandardItem *subitem = new QStandardItem(pixmap, strModelName);
                subitem->setData(TREE_ITEMTYPE_PROTOTYPE, TREEROLE_ITEMTYPE);
                subitem->setData(pcblock->getPrototypeName(), TREEROLE_BLOCKPROTOTYPE);
                subitem->setData(boarditem->text(), TREEROLE_BOARDNAME);
                boarditem->appendRow(subitem);
            }

            QList<VarTableItem> itemsList = getCBlockModelList(pDrawboard, pcblock);
            m_allVarMap[pDrawboard->getName()] << itemsList;
        } else if (CombineBoardModel::Type == pblock->getModelType()
                   || ElecCombineBoardModel::Type == pblock->getModelType()) {
            QString modelName = pblock->getName();
            QString path = boarditem->text() + "/" + modelName;
            QStandardItem *subitem = new QStandardItem(modelName);
            subitem->setData(TREE_ITEMTYPE_BOARD, TREEROLE_ITEMTYPE);
            subitem->setData(pblock->getPrototypeName(), TREEROLE_BLOCKPROTOTYPE);
            subitem->setData(boarditem->text(), TREEROLE_BOARDNAME);
            subitem->setData(path, TREEROLE_NODEPATH);
            boarditem->appendRow(subitem);

            initCombineModelSimuParameters(pblock, subitem, m_allVarMap, path);
        }
    }
    boarditem->sortChildren(0);
}

QList<VarTableItem>
CurrentSimuWidget::getCBlockModelList(QSharedPointer<Kcc::BlockDefinition::DrawingBoardClass> drawboard,
                                      QSharedPointer<Kcc::BlockDefinition::ControlBlock> cblock)
{
    PDrawingBoardClass pdrawboard = drawboard;
    PControlBlock pcblock = cblock;
    if (pdrawboard == nullptr || pcblock == nullptr) {
        return QList<VarTableItem>();
    }
    QList<VarTableItem> listDataInput = QList<VarTableItem>();
    if (pcblock->getPrototypeName() == NPS::PROTOTYPENAME_SCOPE) {
        listDataInput = getCBlockParam(pdrawboard, pcblock, pcblock->getInputVariableGroup(),
                                       CurrentSimuWidget::CBlockParamType_InPut);
    }
    QList<VarTableItem> listDataOutput = getCBlockParam(pdrawboard, pcblock, pcblock->getOutputVariableGroup(),
                                                        CurrentSimuWidget::CBlockParamType_OutPut);
    QList<VarTableItem> listDataDisState = getCBlockParam(pdrawboard, pcblock, pcblock->getDiscreteStateVariableGroup(),
                                                          CurrentSimuWidget::CBlockParamType_DisState);
    PVariableGroup pConStateGp = pcblock->getContinueStateVariableGroup();
    QList<VarTableItem> listDataConState;
    if (pConStateGp) {
        listDataConState = getCBlockParam(pdrawboard, pcblock, pcblock->getContinueStateVariableGroup(),
                                          CurrentSimuWidget::CBlockParamType_ConState);
    }
    QList<VarTableItem> listDataParam;
    if (cblock->getPrototypeName() != NPS::PROTOTYPENAME_FMU) {
        listDataParam = getCBlockParam(pdrawboard, pcblock, pcblock->getControlVariableGroup(),
                                       CurrentSimuWidget::CBlockParamType_Param);
    }
    return listDataInput + listDataOutput + listDataDisState + listDataConState + listDataParam;
}

QList<VarTableItem>
CurrentSimuWidget::getEblockModelList(QSharedPointer<Kcc::BlockDefinition::DrawingBoardClass> drawboard,
                                      QSharedPointer<Kcc::BlockDefinition::ElectricalBlock> eblock, bool bBoardActive)
{
    PDrawingBoardClass pdrawboard = drawboard;
    PElectricalBlock peblock = eblock;
    if (PropertyServerMng::getInstance().m_pIElecSysServer == nullptr || pdrawboard == nullptr || peblock == nullptr) {
        return QList<VarTableItem>();
    }
    if (NPS::PROTOTYPENAME_GROUND == peblock->getPrototypeName()
        || NPS::PROTOTYPENAME_CIRCUITBREAKER == peblock->getPrototypeName()) {
        // 等IElecSysServer的GetOutputParamMap()接口改完需要去除。
        return QList<VarTableItem>();
    }

    bool bCanSetVar = true;
    QList<VarTableItem> listdata;
    QList<QString> simuoutVarNameList;
    QMap<QString, PVariable> simuoutMap;
    QMap<QString, ElecParamDef> outputParamMap = PropertyServerMng::getInstance().m_pIElecSysServer->GetOutputParamMap(
            peblock->getPrototypeName(), peblock->getPhaseNumber());
    // QMap<QString, DrawingBoardClass::VarAliasInfo> boardAliasDataMap = pdrawboard->getAliasDataMap();
    QList<PVariable> simuoutList = peblock->getResultSaveVariableList();
    foreach (PVariable var, simuoutList) {
        simuoutVarNameList.append(var->getName());
        simuoutMap[var->getName()] = var;
    }

    if (peblock->getState() != Block::StateNormal) {
        bCanSetVar = false;
    }

    foreach (QString keywords, outputParamMap.keys()) {
        listdata.append(VarTableItem(
                NameKeyItem(pdrawboard->getUUID(), pdrawboard->getName()),
                NameKeyItem(peblock->getObjectUuid(), peblock->getName()), NameKeyItem(keywords, keywords), "",
                // getBlockAliasName(peblock->getParameter(Block::uuid).toString(), keywords,
                // boardAliasDataMap),
                PARAMTYPE_OUTPUT, simuoutVarNameList.contains(keywords), "",
                // getSupportParamAdj(peblock->getParameter(Block::uuid).toString(), keywords,
                // pdrawboard->getParamAdjSupportList()),
                peblock->getPrototypeName() == NPS::PROTOTYPENAME_DOTBUSBAR ? NPS::PROTOTYPENAME_BUSBAR
                                                                            : peblock->getPrototypeName(),
                bBoardActive,
                simuoutVarNameList.contains(keywords)
                        ? simuoutMap[keywords]->getData(RoleDataDefinition::VariableCheckedTimeRole).value<QDateTime>()
                        : QDateTime(),
                bCanSetVar));
    }
    return listdata;
}

QList<VarTableItem>
CurrentSimuWidget::getCBlockParam(QSharedPointer<Kcc::BlockDefinition::DrawingBoardClass> drawboard,
                                  QSharedPointer<Kcc::BlockDefinition::ControlBlock> cblock,
                                  const QSharedPointer<Kcc::BlockDefinition::VariableGroup> &parameters,
                                  CBlockParamType cbparamtype)
{
    bool bCanSetVar = false;
    PDrawingBoardClass pdrawboard = drawboard;
    PControlBlock pcblock = cblock;
    PVariableGroup pParameters = parameters;
    if (pcblock == nullptr || pdrawboard == nullptr || pParameters == nullptr) {
        return QList<VarTableItem>();
    }
    QString strparatype = "";
    switch (cbparamtype) {
    case CurrentSimuWidget::CBlockParamType_OutPut:
        strparatype = PARAMTYPE_OUTPUT;
        break;
    case CurrentSimuWidget::CBlockParamType_DisState:
        strparatype = PARAMTYPE_DISSTATE;
        break;
    case CurrentSimuWidget::CBlockParamType_ConState:
        strparatype = PARAMTYPE_CONSTATE;
        break;
    case CurrentSimuWidget::CBlockParamType_Param:
        strparatype = PARAMTYPE_PARAM;
        break;
    case CurrentSimuWidget::CBlockParamType_InPut:
        strparatype = PARAMTYPE_INPUT;
        break;
    default:
        break;
    }
    QList<VarTableItem> listdata;
    QList<QString> simuoutVarNameList;
    QMap<QString, PVariable> simuoutMap;
    ////按index排序
    // QStringList paramkeylist = parameters.keys();
    // qSort(paramkeylist.begin(), paramkeylist.end(), [&](const QString& lhs, const QString& rhs) -> bool {
    //	return parameters.value(lhs).index < parameters.value(rhs).index;
    // });

    if (pcblock->getState() == Block::StateNormal || pcblock->getState() == Block::StateWarring) {
        bCanSetVar = true;
    }

    QList<PVariable> simuoutList = pcblock->getResultSaveVariableList();
    if (pcblock->getPrototypeName() == NPS::PROTOTYPENAME_SCOPE) {
        simuoutList.clear();
        bCanSetVar = false;
        simuoutList = pcblock->findVariableInGroup(RoleDataDefinition::InputSignal, RoleDataDefinition::NameRole, "yi");
    }

    foreach (PVariable var, simuoutList) {
        simuoutVarNameList.append(var->getName());
        simuoutMap[var->getName()] = var;
    }

    // QMap<QString, DrawingBoardClass::VarAliasInfo> boardAliasDataMap = pdrawboard->getAliasDataMap();
    foreach (PVariable variable, pParameters->getVariableMap().values()) {
        if (variable == nullptr || !CMA::NUMBERLIST.contains(variable->getDataType())) {
            continue;
        }
        QString namestr = "";
        QString variableName = variable->getName();
        keywordsMaptoName(variableName, pcblock, namestr);
        listdata.append(VarTableItem(
                NameKeyItem(pdrawboard->getUUID(), pdrawboard->getName()),
                NameKeyItem(pcblock->getUUID(), pcblock->getName()), NameKeyItem(variableName, namestr), " ",
                // getBlockAliasName(pcblock->getParameter(Block::uuid).toString(), keywords, boardAliasDataMap),
                strparatype, simuoutVarNameList.contains(variableName), " ",
                // getSupportParamAdj(pcblock->getParameter(Block::uuid).toString(), keywords,
                //                    pdrawboard->getParamAdjSupportList()),
                pcblock->getPrototypeName(), true,
                simuoutVarNameList.contains(variableName)
                        ? simuoutMap[variableName]
                                  ->getData(RoleDataDefinition::VariableCheckedTimeRole)
                                  .value<QDateTime>()
                        : QDateTime(),
                bCanSetVar));
    }
    ////构造型模块按名称排序
    // if (pcblock->drawingBoard != nullptr){
    //	qSort(listdata.begin(), listdata.end(), [&](const VarTableItem& lhs, const VarTableItem& rhs) -> bool {
    //		return lhs.varNameKey.name < rhs.varNameKey.name;
    //	});
    // }
    return listdata;
}

// QString CurrentSimuWidget::getBlockAliasName(const QString &blockuuid, const QString &varkey,
//                                              const QMap<QString, DrawingBoardClass::VarAliasInfo> &aliasmap)
// {
//     if (blockuuid.isEmpty() || varkey.isEmpty()) {
//         return QString("");
//     }
//     DrawingBoardClass::BlockKeyParameter varparam;
//     varparam.blockUUID = blockuuid;
//     varparam.varId = varkey;
//     foreach (QString aliasname, aliasmap.keys()) {
//         if (aliasmap[aliasname].mappingInfoList.contains(varparam)) {
//             return aliasname;
//         }
//     }
//     return QString("");
// }

// bool CurrentSimuWidget::getSupportParamAdj(const QString &blockuuid, const QString &varkey,
//                                            const QList<DrawingBoardClass::BlockKeyParameter> &paramadjlist)
// {
//     if (blockuuid.isEmpty() || varkey.isEmpty()) {
//         return false;
//     }
//     foreach (DrawingBoardClass::BlockKeyParameter tmpitem, paramadjlist) {
//         if (blockuuid == tmpitem.blockUUID && varkey == tmpitem.varId) {
//             return true;
//         }
//     }
//     return false;
// }

void CurrentSimuWidget::keywordsMaptoName(QString keywords, QSharedPointer<Kcc::BlockDefinition::ControlBlock> cblock,
                                          QString &varrealname)
{
    varrealname = keywords;
    PControlBlock pcblock = cblock;
    if (pcblock == nullptr || keywords.isEmpty()) {
        return;
    }
    if (keywords.contains("/")) {
        QStringList strList = keywords.split("/");
        varrealname = "";
        tranKeyToName(strList, varrealname, pcblock);
    }
}

void CurrentSimuWidget::tranKeyToName(QStringList paramkeylist, QString &varrealname,
                                      QSharedPointer<Kcc::BlockDefinition::ControlBlock> cblock)
{
    PControlBlock pcblock = cblock;
    if (pcblock == nullptr || PropertyServerMng::getInstance().m_projectManagerServer == nullptr) {
        return;
    }
    if (paramkeylist.size() < 1) {
        return;
    } else if (paramkeylist.size() == 1) {
        varrealname += paramkeylist.last();
        return;
    }
    if (pcblock->getParentModel() != nullptr) {
        QList<PModel> drawingBoardList = PropertyServerMng::getInstance().m_projectManagerServer->GetAllBoardModel();
        foreach (PModel drawingBoard, drawingBoardList) {
            if (pcblock->getParentModelUUID() == drawingBoard->getUUID()
                && drawingBoard->getChildModel(paramkeylist[0]) != nullptr) {
                PControlBlock pblock = drawingBoard->getChildModel(paramkeylist[0]).dynamicCast<ControlBlock>();
                if (pblock != nullptr) {
                    paramkeylist.removeFirst();
                    varrealname += pblock->getName() + "/";
                    tranKeyToName(paramkeylist, varrealname, pblock);
                }
            }
        }
    }
}

QSharedPointer<Kcc::BlockDefinition::Variable>
CurrentSimuWidget::getVariableByName(QSharedPointer<Kcc::BlockDefinition::Model> model, QString &varrealname)
{
    PModel pModel = model;
    if (Model::Elec_Block_Type == pModel->getModelType()) {
        return pModel->getVariableByRoleData(RoleDataDefinition::ResultSaveVariables, RoleDataDefinition::NameRole,
                                             varrealname);
    } else if (Model::Control_Block_Type == pModel->getModelType()) {
        if (pModel->getVariableByRoleData(RoleDataDefinition::OutputSignal, RoleDataDefinition::NameRole,
                                          varrealname)) {
            return pModel->getVariableByRoleData(RoleDataDefinition::OutputSignal, RoleDataDefinition::NameRole,
                                                 varrealname);
        } else if (pModel->getVariableByRoleData(RoleDataDefinition::DiscreteStateVariable,
                                                 RoleDataDefinition::NameRole, varrealname)) {
            return pModel->getVariableByRoleData(RoleDataDefinition::DiscreteStateVariable,
                                                 RoleDataDefinition::NameRole, varrealname);
        } else if (pModel->getVariableByRoleData(RoleDataDefinition::ContinueStateVariable,
                                                 RoleDataDefinition::NameRole, varrealname)) {
            return pModel->getVariableByRoleData(RoleDataDefinition::ContinueStateVariable,
                                                 RoleDataDefinition::NameRole, varrealname);
        } else if (pModel->getVariableByRoleData(RoleDataDefinition::Parameter, RoleDataDefinition::NameRole,
                                                 varrealname)) {
            return pModel->getVariableByRoleData(RoleDataDefinition::Parameter, RoleDataDefinition::NameRole,
                                                 varrealname);
        }
    }
    return PVariable();
}

void CurrentSimuWidget::initCombineModelSimuParameters(QSharedPointer<Kcc::BlockDefinition::Model> combineModel,
                                                       QStandardItem *item,
                                                       QMap<QString, QList<VarTableItem>> &allVarmap, QString &path)
{
    if (combineModel == nullptr || item == nullptr) {
        return;
    }

    QString strModelName;
    QString strLanguage = QPHelper::getLanguage();
    m_lstAllboards.append(combineModel);
    PDrawingBoardClass drawboard = combineModel.dynamicCast<DrawingBoardClass>();
    if (drawboard == nullptr) {
        return;
    }

    foreach (PModel pblock, combineModel->getChildModels().values()) {
        if (ElectricalBlock::Type == pblock->getModelType()) {
            PElectricalBlock peblock = pblock.dynamicCast<ElectricalBlock>();
            if (peblock == nullptr) {
                continue;
            }

            QMap<QString, ElecParamDef> outputparamMap =
                    PropertyServerMng::getInstance().m_pIElecSysServer->GetOutputParamMap(peblock->getPrototypeName(),
                                                                                          peblock->getPhaseNumber());
            if (outputparamMap.isEmpty()) {
                continue;
            }

            QString busbarprototypeNamechs =
                    PropertyServerMng::getInstance().m_projectManagerServer->GetModelPrototypeNameCHS(
                            NPS::PROTOTYPENAME_BUSBAR);
            QPixmap pixmap = PropertyServerMng::getInstance().m_ElectricalComponentServer->GetComponentIcon(
                    peblock->getPrototypeName(), BLOCKTYPE_PROTOTYPE);

            QStandardItem *subitem;
            if (peblock->getPrototypeName() == NPS::PROTOTYPENAME_DOTBUSBAR) {
                if (strLanguage != "zh_CN") {
                    busbarprototypeNamechs = NPS::PROTOTYPENAME_BUSBAR;
                }

                if (!checkedChildItemText(item, busbarprototypeNamechs)) {
                    subitem = new QStandardItem(pixmap, busbarprototypeNamechs);
                    subitem->setData(TREE_ITEMTYPE_PROTOTYPE, TREEROLE_ITEMTYPE);
                    subitem->setData(NPS::PROTOTYPENAME_BUSBAR, TREEROLE_BLOCKPROTOTYPE);
                    subitem->setData(item->text(), TREEROLE_BOARDNAME);
                    item->appendRow(subitem);
                }
            } else {
                strModelName = peblock->getPrototypeName_CHS();
                if (!checkedChildItemText(item, strModelName)) {
                    subitem = new QStandardItem(pixmap, strModelName);
                    subitem->setData(TREE_ITEMTYPE_PROTOTYPE, TREEROLE_ITEMTYPE);
                    subitem->setData(peblock->getPrototypeName(), TREEROLE_BLOCKPROTOTYPE);
                    subitem->setData(item->text(), TREEROLE_BOARDNAME);
                    item->appendRow(subitem);
                }
            }

            QList<VarTableItem> itemsList = getEblockModelList(drawboard, peblock, m_isElecboardActived);
            m_allVarMap[drawboard->getName()] << itemsList;
        } else if (ControlBlock::Type == pblock->getModelType()) {
            PControlBlock pcblock = pblock.dynamicCast<ControlBlock>();
            if (pcblock == nullptr) {
                continue;
            }

            QPixmap pixmap = PropertyServerMng::getInstance().m_ControlComponentServer->GetComponentIcon(
                    pcblock->getPrototypeName(), BLOCKTYPE_PROTOTYPE);

            strModelName = pcblock->getPrototypeName_Readable();
            if (!checkedChildItemText(item, strModelName)) {
                QStandardItem *subitem = new QStandardItem(pixmap, strModelName);
                subitem->setData(TREE_ITEMTYPE_PROTOTYPE, TREEROLE_ITEMTYPE);
                subitem->setData(pcblock->getPrototypeName(), TREEROLE_BLOCKPROTOTYPE);
                subitem->setData(path, TREEROLE_BOARDNAME);
                item->appendRow(subitem);
            }

            QList<VarTableItem> itemsList = getCBlockModelList(drawboard, pcblock);
            allVarmap[path] << itemsList;
        } else if (CombineBoardModel::Type == pblock->getModelType()) {
            PCombineBoardModel pCombineBlock = pblock.dynamicCast<CombineBoardModel>();
            if (pCombineBlock == nullptr) {
                continue;
            }

            QString modelName = pCombineBlock->getName();
            QString subPath = path + "/" + modelName;
            QStandardItem *subitem = new QStandardItem(modelName);
            subitem->setData(TREE_ITEMTYPE_BOARD, TREEROLE_ITEMTYPE);
            subitem->setData(pCombineBlock->getPrototypeName(), TREEROLE_BLOCKPROTOTYPE);
            subitem->setData(item->text(), TREEROLE_BOARDNAME);
            subitem->setData(subPath, TREEROLE_NODEPATH);
            item->appendRow(subitem);

            initCombineModelSimuParameters(pCombineBlock, subitem, allVarmap, subPath);
        }
    }
    item->sortChildren(0);
}

bool CurrentSimuWidget::checkedChildItemText(QStandardItem *item, QString &text)
{
    if (item->hasChildren()) {
        for (int i = 0; i < item->rowCount(); ++i) {
            if (item->child(i)->text() == text) {
                QString in = item->text();
                QString in1 = item->child(i)->text();
                // 包含text
                return true;
            }
        }
    }
    return false;
}

void CurrentSimuWidget::setTableHeaderCheckStatus(QString nodeName, QString prototypeName)
{
    bool isboardActive = true;
    QList<VarTableItem> datalList = m_allVarMap.value(nodeName);

    if (datalList.isEmpty()) {
        ui.tableView->setSelectionMode(QAbstractItemView::NoSelection);
        ui.tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        m_pHeaderView->setHeaderCheckStatus(TableHeaderView::NoChecked, false);
        return;
    } else {
        int nSelectCount = 0;
        int nPrototypeVarTotal = 0;
        ui.tableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
        ui.tableView->setEditTriggers(QAbstractItemView::DoubleClicked);
        m_pHeaderView->setEnabled(true);
        foreach (VarTableItem item, datalList) {
            if (prototypeName.isEmpty()) {
                ++nPrototypeVarTotal;
            } else if (item.prototypeName == prototypeName) {
                ++nPrototypeVarTotal;
            }
        }

        for (int row = 0; row < m_tableProxyModel->rowCount(); ++row) {
            QModelIndex index = m_tableProxyModel->index(row, 0);
            QMap<int, QVariant> itemDatasMap = m_tableProxyModel->itemData(index);
            if (0 == itemDatasMap.size()) {
                return;
            }

            if (itemDatasMap.value(Qt::CheckStateRole) == Qt::Checked) {
                // if (prototypeName.isEmpty()) {
                //     ++nSelectCount;
                // } else if (item.prototypeName == prototypeName) {
                ++nSelectCount;
                //}
            }
        }

        // 检测是否全选
        if (nSelectCount == nPrototypeVarTotal && nSelectCount > 0) {
            m_pHeaderView->setHeaderCheckStatus(TableHeaderView::AllChecked, true);
        } else if (0 < nSelectCount && nSelectCount < nPrototypeVarTotal) {
            m_pHeaderView->setHeaderCheckStatus(TableHeaderView::PartChecked, false);
        } else {
            m_pHeaderView->setHeaderCheckStatus(TableHeaderView::NoChecked, false);
        }
    }

    // add by wangyuzhou 2023.09.01 偶发性禅道bug:10102 【取消激活画板后，在当前仿真中勾选变量 时偶发因为软件闪退】
    // 问题原因：当画板未激活同时表格数据很多，用户频繁点击表格中批量项时，表格的setData()函数中的处理可能会导致崩溃现象。
    // 解决：此段代码可以在用户操作层面完全避免触发上述bug发生。
    auto board = PropertyServerMng::getInstance().m_projectManagerServer->GetBoardModelByName(nodeName);
    if (!board) {
        return;
    }

    PDrawingBoardClass pBoardClass = board.dynamicCast<DrawingBoardClass>();
    if (!pBoardClass || pBoardClass->getModelType() != ElecBoardModel::Type) {
        return;
    }

    isboardActive = m_boardActiveStsMap.value(board->getUUID(), false).toBool();

    if (!isboardActive) {
        ui.tableView->setSelectionMode(QAbstractItemView::NoSelection);
        ui.tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        m_pHeaderView->setEnabled(false);
    } else {
        ui.tableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
        ui.tableView->setEditTriggers(QAbstractItemView::DoubleClicked);
        m_pHeaderView->setEnabled(true);
    }
    // end
}

void CurrentSimuWidget::updateSelectedItemsCheckStatus(const QModelIndex &index)
{
    for (const QModelIndex &indexs : m_selected) {
        Qt::CheckState state = Qt::CheckState(index.data(Qt::CheckStateRole).toInt()); // 点击单元格的状态
        m_tableProxyModel->setData(indexs, state, Qt::CheckStateRole);
    }
}

bool CurrentSimuWidget::saveDrawboardData(PModel pDrawboard, QString boardnames,
                                          QMap<QString, QList<VarTableItem>> newVarMap)
{
    QMap<QString, QList<VarTableItem>> oldVarMap = m_pTableModel->getOldAllVarMap();
    if (oldVarMap == newVarMap || oldVarMap.keys() != newVarMap.keys()) {
        return false; //
    }

    if (pDrawboard == nullptr || oldVarMap.value(boardnames) == newVarMap.value(boardnames)) {
        return false;
    }

    PISimuNPSDataDictionaryServer pDictServer = RequestServer<ISimuNPSDataDictionaryServer>();
    if (pDictServer == nullptr) {
        LOGOUT(tr("ISimuNPSDataDictionaryServer unregistered"), LOG_ERROR);
        return false;
    }

    // 处理模型嵌套情况，除了顶层画板外，其它子模块需要用 / 组合变量名称
    bool isDelDataDictionary = false;
    QStringList nameList;
    QString strBlockName = "";
    QList<PVariable> tmpoutputList;
    bool bBoardValueChanged = false;
    QMap<QString, PVariable> simuoutMap;
    QStringList deleteVarList; // 数据字典中批量删除的数据
    auto curDict = PropertyServerMng::getInstance().m_projectManagerServer->GetRealTimeSimulationName();
    foreach (VarTableItem item, newVarMap.value(boardnames)) {
        nameList.clear();
        nameList = boardnames.split("/");
        nameList.push_back(item.blockNameKey.name);
        nameList.push_back(item.varNameKey.name);
        // 如果遍历的模块对象改变，则清空上个模块对象的结果保存变量在simuoutMap中的信息
        if (item.blockNameKey.keywords != strBlockName) {
            simuoutMap.clear();
            strBlockName = item.blockNameKey.keywords;
        }

        // 保存输出参数到模块，更新数据字典。
        PModel ptmpBlock = pDrawboard->getChildModel(item.blockNameKey.keywords);
        if (ptmpBlock != nullptr) {
            tmpoutputList.clear();
            tmpoutputList = ptmpBlock->getResultSaveVariableList();
            foreach (PVariable var, tmpoutputList) {
                simuoutMap[var->getName()] = var;
            }

            QString boardName = nameList.takeFirst();
            QString blockName = nameList.takeFirst();
            QString varName = nameList.join('/');
            PVariable var = getVariableByName(ptmpBlock, item.varNameKey.keywords);
            if (!var) {
                PVariableGroup varGroup = ptmpBlock->createVariableGroup(RoleDataDefinition::ResultSaveVariables);
                var = varGroup->createVariable();
            }
            // 存在变为未选中，需要删除。
            if (simuoutMap.contains(item.varNameKey.keywords) && !item.bSelected) {
                isDelDataDictionary = true;
                ptmpBlock->setVariableData(var, RoleDataDefinition::VariableCheckedRole, false);
                ptmpBlock->setVariableData(var, RoleDataDefinition::VariableCheckedTimeRole,
                                           QDateTime::currentDateTime());
                simuoutMap.remove(item.varNameKey.keywords);
                deleteVarList.append(QString("%1.%2.%3.%4").arg(curDict).arg(boardName).arg(blockName).arg(varName));
                bBoardValueChanged = true;

                LOGOUT(NPS::LOG_STRING(
                        ElectricalBlock::Type == ptmpBlock->getModelType() ? CMA::LogTag_ElectricalComponent
                                                                           : CMA::LogTag_ControlComponent,
                        ptmpBlock->getName(), QObject::tr("%1 Variable:%2").arg(curDict).arg(var->getName()),
                        CMA::BLOCK_VARIABLE_CHECKED, CMA::BLOCK_VARIABLE_UNCHECKED));
            } else if (!simuoutMap.contains(item.varNameKey.keywords) && item.bSelected) {
                ptmpBlock->setVariableData(var, RoleDataDefinition::VariableCheckedRole, true);
                ptmpBlock->setVariableData(var, RoleDataDefinition::NameRole, item.varNameKey.keywords);
                ptmpBlock->setVariableData(var, RoleDataDefinition::DisplayNameRole, item.varNameKey.keywords);
                ptmpBlock->setVariableData(var, RoleDataDefinition::VariableCheckedTimeRole,
                                           QDateTime::currentDateTime());
                pDictServer->AddDataDictionary(
                        QString("%1.%2.%3.%4").arg(curDict).arg(boardName).arg(blockName).arg(varName));
                bBoardValueChanged = true;

                LOGOUT(NPS::LOG_STRING(
                        ElectricalBlock::Type == ptmpBlock->getModelType() ? CMA::LogTag_ElectricalComponent
                                                                           : CMA::LogTag_ControlComponent,
                        ptmpBlock->getName(), QObject::tr("%1 Variable:%2").arg(curDict).arg(var->getName()),
                        CMA::BLOCK_VARIABLE_UNCHECKED, CMA::BLOCK_VARIABLE_CHECKED));
            }
        }
    }

    if (isDelDataDictionary) {
        pDictServer->DelDataDictionary(deleteVarList);
    }
    return bBoardValueChanged;
}

void CurrentSimuWidget::getSlotConnectionsModel(PModel &pBlock)
{
    if (SlotBlock::Type == pBlock->getModelType()) {
        QList<QWeakPointer<Model>> slotModelsList = pBlock->getConnectedModel(ModelConnSlot);

        int nValidInstances = 0;
        for (QWeakPointer<Model> instanceModel : slotModelsList) {
            auto iModel = instanceModel.toStrongRef();
            if (iModel == nullptr) {
                continue;
            }
            pBlock = iModel;
            nValidInstances++;
        }

        // 插槽只能关联一个有效实例
        Q_ASSERT(nValidInstances <= 1);
    }
}

void CurrentSimuWidget::onChangeHeaderViewEnabled(bool enabled)
{
    m_pHeaderView->setCheckBoxEnableStatus(enabled);
}

void CurrentSimuWidget::onTableSelectedAllRows()
{
    if (m_itemLabel != nullptr && m_tableProxyModel != nullptr) {
        m_itemLabel->setText(tr("%1 items,%2 items selected")
                                     .arg(QString::number(m_tableProxyModel->rowCount()))
                                     .arg(QString::number(m_tableProxyModel->rowCount())));
    }
}

void CurrentSimuWidget::onGetSelectedIndexs(QModelIndexList &indexlist)
{
    for (QModelIndex index : indexlist) {
        QModelIndex indexTemp = index.sibling(index.row(), 0);
        m_selected.append(indexTemp);
    }
}

void CurrentSimuWidget::onClickedTreeItem(const QModelIndex &index)
{
    if (!index.isValid() || ui.tableView == nullptr || m_pTableModel == nullptr || m_tableProxyModel == nullptr
        || index.data().toString().isEmpty()) {
        return;
    }

    QString strNodeName;
    QString temstr = index.data(TREEROLE_ITEMTYPE).toString();
    if (TREE_ITEMTYPE_BOARD == index.data(TREEROLE_ITEMTYPE).toString()) {
        strNodeName = index.data(TREEROLE_NODEPATH).toString();
        m_strBlockPrototypeName = "";
        m_datalList = m_allVarMap.value(strNodeName);

        m_pTableModel->switchDataToBoard(strNodeName);
        m_tableProxyModel->setFilterPrototypeName(m_strBlockPrototypeName);
        setTableHeaderCheckStatus(strNodeName);
    } else if (TREE_ITEMTYPE_PROTOTYPE == index.data(TREEROLE_ITEMTYPE).toString()) {
        strNodeName = index.data(TREEROLE_BOARDNAME).toString();
        m_strBlockPrototypeName = index.data(TREEROLE_BLOCKPROTOTYPE).toString();
        m_datalList = m_allVarMap.value(strNodeName);

        m_pTableModel->switchDataToBoard(strNodeName);
        m_tableProxyModel->setFilterPrototypeName(m_strBlockPrototypeName);
        setTableHeaderCheckStatus(strNodeName, m_strBlockPrototypeName);
    }

    if (m_datalList.isEmpty()) {
        m_pHeaderView->setEnabled(false);
    }

    if (ui.tableView->verticalScrollBar() != nullptr) {
        ui.tableView->verticalScrollBar()->setValue(0);
    }

    int with = 80;
    for (int colno = 0; colno < m_tableProxyModel->columnCount(); ++colno) {
        QFontMetrics fontMetrics(TABLE_TITLE_MONITOR);
        if (fontMetrics.width(TABLE_TITLE_MONITOR) > 32) {
            with = 110;
        }
        if (TABLE_TITLE_MONITOR == m_tableProxyModel->headerData(colno, Qt::Horizontal).toString()
            || TABLE_TITLE_DYNAMICADJ == m_tableProxyModel->headerData(colno, Qt::Horizontal).toString()) {
            ui.tableView->setColumnWidth(colno, with);
        } else {
            ui.tableView->setColumnWidth(colno, with);
        }
    }

    if (m_itemLabel != nullptr) {
        m_itemLabel->setText(tr("%1 items").arg(QString::number(m_tableProxyModel->rowCount())));
    }
}

bool CurrentSimuWidget::eventFilter(QObject *obj, QEvent *evt)
{
    if (evt->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(evt);
        if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
            QComboBox *pComboBox = qobject_cast<QComboBox *>(obj);
            if (pComboBox) {
                return true; // 下拉输入框截获事件，不传递给下一个过滤器或目标对象
            }
        }
    }
    return QWidget::eventFilter(obj, evt);
}

void CurrentSimuWidget::onTableSelectedRowsChanged(const int &selectedrows)
{
    if (m_itemLabel != nullptr && m_tableProxyModel != nullptr) {
        if (selectedrows > 0) {
            m_itemLabel->setText(tr("%1 items,%2 items selected")
                                         .arg(QString::number(m_tableProxyModel->rowCount()))
                                         .arg(QString::number(selectedrows)));
        } else {
            m_itemLabel->setText(tr("%1 items").arg(QString::number(m_tableProxyModel->rowCount())));
        }
    }
}

void CurrentSimuWidget::onSearchColBoxChanged(int col)
{
    if (m_tableProxyModel != nullptr && ui.colCombobox != nullptr && ui.searchBox != nullptr) {
        m_tableProxyModel->setFilterColumn(ui.colCombobox->currentText(), col);
        if (TABLE_TITLE_MONITOR == ui.colCombobox->currentText()
            || TABLE_TITLE_DYNAMICADJ == ui.colCombobox->currentText()) {
            ui.searchBox->clear();
            ui.searchBox->addItems(QStringList() << CHECKSTATE_ALL << CHECKSTATE_CHECKED << CHECKSTATE_UNCHECKED);
            ui.searchBox->setEditable(false);
        } else {
            ui.searchBox->clear();
            ui.searchBox->setEditable(true);
        }
    }
}

void CurrentSimuWidget::onSearchTextChanged(const QString &textstr)
{
    if (m_tableProxyModel != nullptr) {
        m_tableProxyModel->setFilterString(textstr);
    }
}

void CurrentSimuWidget::onTableItemClicked(const QModelIndex &index)
{
    Q_UNUSED(index);
    int nCheckedCount = 0;
    int nUnCheckedCount = 0;
    int nPrototypeVarTotal = 0;

    QModelIndexList selected = ui.tableView->getSelectedIndexes();
    if (1 == selected.count()) {
        updateSelectedItemsCheckStatus(index);
        m_selected.clear();
    } else {
        onGetSelectedIndexs(selected);
        updateSelectedItemsCheckStatus(index);
        m_selected.clear();
    }

    if (m_datalList.isEmpty()) {
        m_pHeaderView->setHeaderCheckStatus(TableHeaderView::NoChecked, false);
        return;
    } else {
        foreach (VarTableItem item, m_datalList) {
            if (m_strBlockPrototypeName.isEmpty()) {
                ++nPrototypeVarTotal;
            } else if (item.prototypeName == m_strBlockPrototypeName) {
                ++nPrototypeVarTotal;
            }
        }

        for (int row = 0; row < m_tableProxyModel->rowCount(); ++row) {
            QModelIndex index = m_tableProxyModel->index(row, 0);
            QMap<int, QVariant> itemDatasMap = m_tableProxyModel->itemData(index);
            if (0 == itemDatasMap.size()) {
                return;
            }

            if (itemDatasMap.value(Qt::CheckStateRole) == Qt::Checked) {
                nCheckedCount++;
            } else {
                nUnCheckedCount++;
            }
        }
    }

    // 检测是否全选或全取消
    if (nCheckedCount == nPrototypeVarTotal) {
        m_pHeaderView->setHeaderCheckStatus(TableHeaderView::AllChecked, true);
    } else if (nUnCheckedCount == nPrototypeVarTotal) {
        m_pHeaderView->setHeaderCheckStatus(TableHeaderView::NoChecked, false);
    } else {
        m_pHeaderView->setHeaderCheckStatus(TableHeaderView::PartChecked, false);
    }
}

void CurrentSimuWidget::onChangeColSelectStatus(int col, bool isSelected)
{
    bool isAllChecked = isSelected;
    bool isPartCheckStateDiffer = false;
    TableHeaderView::CheckState checkState;
    QMap<int, QVariant> itemDatasMap;
    int nInitRowCount = m_tableProxyModel->rowCount();
    for (int row = 0; row < nInitRowCount; ++row) {
        QModelIndex index = m_tableProxyModel->index(row, col);
        if (isSelected) {
            if (!index.data(NPS::ModelDataEditRole).toBool()) {
                if (!index.data(Qt::CheckStateRole).toBool()) {
                    isPartCheckStateDiffer = true;
                }
                continue;
            }
            isAllChecked = true;
            itemDatasMap[Qt::CheckStateRole] = Qt::Checked;
            m_tableProxyModel->setItemData(index, itemDatasMap);
        } else {
            if (!index.data(NPS::ModelDataEditRole).toBool()) {
                continue;
            }
            isAllChecked = false;
            itemDatasMap[Qt::CheckStateRole] = Qt::Unchecked;
            m_tableProxyModel->setItemData(index, itemDatasMap);
        }
    }

    if (isPartCheckStateDiffer) {
        m_pHeaderView->setHeaderCheckStatus(TableHeaderView::PartChecked, true);
        return;
    }

    if (isAllChecked) {
        m_pHeaderView->setHeaderCheckStatus(TableHeaderView::AllChecked, true);
    } else {
        m_pHeaderView->setHeaderCheckStatus(TableHeaderView::NoChecked, false);
    }
}

void CurrentSimuWidget::slotHeaderClicked(int logicalIndex)
{
    if (0 == logicalIndex) {
        // 第一列不显示排序箭头
        ui.tableView->setSortingEnabled(false);
        ui.tableView->horizontalHeader()->setSortIndicatorShown(false);
    } else {
        ui.tableView->setSortingEnabled(true);
        ui.tableView->horizontalHeader()->setSortIndicatorShown(true);
    }
}

// 自定义tablemodel
VarTableModel::VarTableModel(const QString &projname, QObject *parent /*= nullptr*/)
    : QAbstractTableModel(parent), m_projectName(projname)
{
    // if (NPS::PROJECT_SIMUNPS == m_projectName) {
    m_headerData << TABLE_TITLE_MONITOR << TABLE_TITLE_BLOCKNAME << TABLE_TITLE_VARNAME << TABLE_TITLE_VARTYPE
                 << TABLE_TITLE_MODIFYTIME;
    init();
    // } else {
    //     m_headerData << TABLE_TITLE_MONITOR << TABLE_TITLE_BLOCKNAME << TABLE_TITLE_VARNAME <<
    //     TABLE_TITLE_VARTYPE
    //                  << TABLE_TITLE_VARALIAS << TABLE_TITLE_DYNAMICADJ << TABLE_TITLE_MODIFYTIME;
    // }
}

VarTableModel::~VarTableModel() { }

void VarTableModel::init() { }

int VarTableModel::rowCount(const QModelIndex &parent /*= QModelIndex( ) */) const
{
    return m_data.size();
}

int VarTableModel::columnCount(const QModelIndex &parent /*= QModelIndex( ) */) const
{
    return m_headerData.size();
}

QVariant VarTableModel::data(const QModelIndex &index, int role /*= Qt::DisplayRole */) const
{
    if (!index.isValid() || index.column() >= m_headerData.size() || index.row() >= m_data.size()) {
        return QVariant();
    }

    switch (role) {
    case Qt::DisplayRole:
    case Qt::EditRole:
    case Qt::ToolTipRole: {
        if (TABLE_TITLE_MONITOR == m_headerData[index.column()]
            || TABLE_TITLE_DYNAMICADJ == m_headerData[index.column()]) {
            return QVariant();
        } else if (TABLE_TITLE_BLOCKNAME == m_headerData[index.column()]) {
            return m_data[index.row()].blockNameKey.name;
        } else if (TABLE_TITLE_VARNAME == m_headerData[index.column()]) {
            return m_data[index.row()].varNameKey.name;
        } else if (TABLE_TITLE_VARTYPE == m_headerData[index.column()]) {
            return m_data[index.row()].varClassName;
        } else if (TABLE_TITLE_VARALIAS == m_headerData[index.column()]) {
            return m_data[index.row()].varAlias;
        } else if (TABLE_TITLE_MODIFYTIME == m_headerData[index.column()]) {
            return m_data[index.row()].selectSaveTime.toString("yyyy-MM-dd hh:mm:ss");
        }
    } break;
    case Qt::CheckStateRole: {
        if (TABLE_TITLE_MONITOR == m_headerData[index.column()]) {
            return m_data[index.row()].bSelected ? Qt::Checked : Qt::Unchecked;
        } else if (TABLE_TITLE_DYNAMICADJ == m_headerData[index.column()]) {
            return m_data[index.row()].bSupportParamAdj ? Qt::Checked : Qt::Unchecked;
        }
    } break;
    case Qt::TextColorRole: {
        if (!index.data(NPS::ModelDataEditRole).toBool()) {
            return QColor(NPS::Color_NotEditable);
        }
    } break;
    case NPS::ModelDataEditRole: {
        if (!m_data[index.row()].bBoardActive || !m_data[index.row()].bCanSetVar
            || PropertyServerMng::getInstance().m_pGraphicsModelingServer->getRunningStatus()) {
            return false;
        } else {
            // 表头初始默认为disenable状态
            // 只要有一个item的bCanSetVar为true，则设置表头为enable状态
            emit changeHeaderViewEnabled(true);
        }
        if (TABLE_TITLE_VARALIAS == m_headerData[index.column()]
            || TABLE_TITLE_MONITOR == m_headerData[index.column()]) {
            return true;
        } else if (TABLE_TITLE_DYNAMICADJ == m_headerData[index.column()]
                   && PARAMTYPE_PARAM == m_data[index.row()].varClassName) {
            return true;
        } else {
            return false;
        }
    } break;
    case NPS::ModelDataTypeRole: {
        if (TABLE_TITLE_MONITOR == m_headerData[index.column()]
            || TABLE_TITLE_DYNAMICADJ == m_headerData[index.column()]) {
            return RoleDataDefinition::ControlTypeCheckbox;
        } else if (TABLE_TITLE_MODIFYTIME == m_headerData[index.column()]) {
            return RoleDataDefinition::ControlTypeDate;
        } else {
            return RoleDataDefinition::ControlTypeTextbox;
        }
    } break;
    case VarTableModel::CustomRole_PrototypeName: {
        return m_data[index.row()].prototypeName;
    } break;
    default:
        break;
    }
    return QVariant();
}

QVariant VarTableModel::headerData(int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole */) const
{
    if (Qt::Horizontal == orientation) {
        if (0 < section && section < m_headerData.size() && Qt::DisplayRole == role) {
            return m_headerData[section];
        } else {
            return QVariant();
        }
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}

bool VarTableModel::setData(const QModelIndex &index, const QVariant &value, int role /*= Qt::EditRole*/)
{
    if (Qt::CheckStateRole == role) {
        bool oldvalue = (index.data(Qt::CheckStateRole).toInt() == Qt::Checked) ? true : false;
        bool newvalue = (Qt::CheckState(value.toInt()) == Qt::Checked) ? true : false;
        if (oldvalue == newvalue) {
            return true;
        }
        if (TABLE_TITLE_MONITOR == m_headerData[index.column()]) {
            m_data[index.row()].bSelected = newvalue;
            m_allVarmap[m_currentBoardName] = m_data;
            emit dataChanged(index, index);
        } else if (TABLE_TITLE_DYNAMICADJ == m_headerData[index.column()]) {
            m_data[index.row()].bSupportParamAdj = newvalue;
            m_allVarmap[m_currentBoardName] = m_data;
            emit dataChanged(index, index);
            QString curvaralias = m_data[index.row()].varAlias;
            if (!curvaralias.isEmpty()) {
                for (int row = 0; row < m_data.size(); ++row) {
                    if (row != index.row() && curvaralias == m_data[row].varAlias) {
                        m_data[row].bSupportParamAdj = newvalue;
                        m_allVarmap[m_currentBoardName] = m_data;
                        QModelIndex changedindex = index.sibling(row, index.column());
                        emit dataChanged(changedindex, changedindex);
                    }
                }
            }
        }
        return true;
    } else if (Qt::EditRole == role && index.data(NPS::ModelDataEditRole).toBool()) {
        if (index.data().toString() == value.toString()) {
            return true;
        }
        if (TABLE_TITLE_VARALIAS == m_headerData[index.column()]) {
            int varclasscol = getHeaderTitleCol(TABLE_TITLE_VARTYPE);
            if (varclasscol < 0) {
                // 未找到[%1]对应的列，无法修改数据！
                LOGOUT(tr("Could not find the corresponding column for [%1], unable to modify data!")
                               .arg(TABLE_TITLE_VARTYPE),
                       LOG_WARNING);
                return false;
            }
            const QString indexDataStr = index.sibling(index.row(), varclasscol).data().toString();
            if (PARAMTYPE_OUTPUT == indexDataStr || PARAMTYPE_DISSTATE == indexDataStr
                || PARAMTYPE_CONSTATE == indexDataStr) {
                foreach (VarTableItem itemtmp, m_data) {
                    if (value.toString() == itemtmp.varAlias && !itemtmp.varAlias.isEmpty()) {
                        LOGOUT(tr("The alias [%1] already exists, the alias for %2 cannot be duplicated! Please "
                                  "rename!") // 已存在别名[%1],%2的别名不能重复！请重新命名!
                                       .arg(value.toString())
                                       .arg(PARAMTYPE_OUTPUT),
                               LOG_WARNING);
                        return true;
                    }
                }
            } else {
                foreach (VarTableItem itemtmp, m_data) {
                    if (((PARAMTYPE_OUTPUT == itemtmp.varClassName) || (PARAMTYPE_DISSTATE == itemtmp.varClassName)
                         || (PARAMTYPE_CONSTATE == itemtmp.varClassName))
                        && value.toString() == itemtmp.varAlias && !itemtmp.varAlias.isEmpty()) {
                        LOGOUT(tr("The alias [%1] already exists, the alias for %2 cannot be duplicated with %3.Please "
                                  "rename it!") // 已存在别名[%1],%2的别名不能与%3重复！请重新命名!
                                       .arg(value.toString())
                                       .arg(index.sibling(index.row(), varclasscol).data().toString())
                                       .arg(PARAMTYPE_OUTPUT),
                               LOG_WARNING);
                        return true;
                    }
                }
            }
            m_data[index.row()].varAlias = value.toString();
            m_allVarmap[m_currentBoardName] = m_data;
            emit dataChanged(index, index);
        }
        return true;
    }
    return false;
}

Qt::ItemFlags VarTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }

    if (RoleDataDefinition::ControlTypeCheckbox == index.data(NPS::ModelDataTypeRole).toString()) {
        if (index.data(NPS::ModelDataEditRole).toBool()
            && PropertyServerMng::getInstance().m_pGraphicsModelingServer != nullptr
            && !PropertyServerMng::getInstance().m_pGraphicsModelingServer->getRunningStatus()) {
            return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;
        } else {
            return Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;
        }
    } else {
        if (index.data(NPS::ModelDataEditRole).toBool()) {
            return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
        }
    }
    return QAbstractTableModel::flags(index);
}

void VarTableModel::clear()
{
    beginResetModel();
    m_data.clear();
    m_allVarmap.clear();
    m_currentBoardName = "";
    endResetModel();
}

void VarTableModel::initData(const QMap<QString, QList<VarTableItem>> &allVarMap)
{
    m_allVarmap = allVarMap;
    m_allVarmapOld = allVarMap;
}

void VarTableModel::switchDataToBoard(const QString &boardname)
{
    beginResetModel();
    m_currentBoardName = boardname;
    m_data.clear();
    m_data = m_allVarmap.value(boardname);
    endResetModel();
}

QStringList VarTableModel::getHeaderList()
{
    return m_headerData;
}

QMap<QString, QList<VarTableItem>> VarTableModel::getOldAllVarMap()
{
    return m_allVarmapOld;
}

QMap<QString, QList<VarTableItem>> VarTableModel::getNewAllVarMap()
{
    return m_allVarmap;
}

int VarTableModel::getHeaderTitleCol(const QString &titlename) const
{
    for (int col = 0; col < m_headerData.size(); ++col) {
        if (titlename == m_headerData[col]) {
            return col;
        }
    }
    return -1;
}

bool VarTableModel::getSystemRunningStatus()
{
    return false;
}

// 筛选排序model
CustomSortFilterModel::CustomSortFilterModel(QObject *parent)
    : QSortFilterProxyModel(parent), m_strFilterString(""), m_FilterColumnStr(""), m_FilterCol(-1)
{
}

CustomSortFilterModel::~CustomSortFilterModel() { }

QVariant CustomSortFilterModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Vertical && role == Qt::DisplayRole) {
        return QString::number(section + 1);
    }
    return sourceModel()->headerData(section, orientation, role);
}

void CustomSortFilterModel::setFilterString(const QString &strFilter)
{
    m_strFilterString = strFilter;
    invalidateFilter();
}

void CustomSortFilterModel::setFilterColumn(const QString &colname, const int &colno)
{
    m_FilterColumnStr = colname;
    m_FilterCol = colno;
    invalidateFilter();
}

void CustomSortFilterModel::setFilterPrototypeName(const QString &prototypestr)
{
    m_prototypeName = prototypestr;
    invalidateFilter();
}

bool CustomSortFilterModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    QModelIndex index = sourceModel()->index(source_row, 0, source_parent);

    if (!m_prototypeName.isEmpty()
        && index.data(VarTableModel::CustomRole_PrototypeName).toString() != m_prototypeName) {
        return false;
    }

    if (!index.isValid() || m_strFilterString.isEmpty() || m_FilterColumnStr.isEmpty() || (m_FilterCol < 0)
        || (m_FilterCol >= sourceModel()->columnCount())) {
        return true;
    }
    if (TABLE_TITLE_MONITOR == m_FilterColumnStr || TABLE_TITLE_DYNAMICADJ == m_FilterColumnStr) {
        if (CHECKSTATE_ALL == m_strFilterString) {
            return true;
        }

        if (index.sibling(index.row(), m_FilterCol).data(Qt::CheckStateRole).toInt() == Qt::Checked
            && CHECKSTATE_CHECKED == m_strFilterString) {
            return true;
        } else if (index.sibling(index.row(), m_FilterCol).data(Qt::CheckStateRole).toInt() == Qt::Unchecked
                   && CHECKSTATE_UNCHECKED == m_strFilterString) {
            return true;
        }
        return false;
    }
    QString strValue = index.sibling(index.row(), m_FilterCol).data().toString();
    return strValue.contains(m_strFilterString, Qt::CaseInsensitive);
}

bool CustomSortFilterModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    // 根据数据类型比较
    if (RoleDataDefinition::ControlTypeCheckbox == left.data(NPS::ModelDataTypeRole).toString()) {
        return left.data(Qt::CheckStateRole).toBool() < right.data(Qt::CheckStateRole).toBool();
    } else if (RoleDataDefinition::ControlTypeDate == left.data(NPS::ModelDataTypeRole).toString()) {
        QDateTime timeleft = QDateTime::fromString(left.data(Qt::DisplayRole).toString(), NPS::DateFormate);
        QDateTime timeright = QDateTime::fromString(right.data(Qt::DisplayRole).toString(), NPS::DateFormate);
        return timeleft < timeright;
    }
    return left.data(Qt::DisplayRole).toString() < right.data(Qt::DisplayRole).toString();
}

/*自定义的QStyledItemDelegate*/
CheckBoxDelegate::CheckBoxDelegate(QObject *parent) : QStyledItemDelegate(parent) { }

void CheckBoxDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (option.state & QStyle::State_Selected) {
        // item 被选中,应用选中背景样式
        QStyledItemDelegate::paint(painter, option, index);
        QBrush brush = option.palette.highlight();
        painter->fillRect(option.rect, brush);
    }

    bool isEditable = static_cast<Qt::CheckState>(index.data(NPS::ModelDataEditRole).toBool());
    Qt::CheckState checkState = static_cast<Qt::CheckState>(index.data(Qt::CheckStateRole).toInt());
    QStyleOptionButton checkBoxOption;
    checkBoxOption.rect = option.rect.adjusted(3, 0, 0, 0);
    checkBoxOption.state = checkState == Qt::Checked ? QStyle::State_On : QStyle::State_Off;
    checkBoxOption.state |= isEditable ? QStyle::State_Enabled : QStyle::State_ReadOnly;
    checkBoxOption.iconSize = QSize(20, 20);

    QCheckBox *pCheckBox = new QCheckBox(dynamic_cast<QWidget *>(this->parent()));
    QApplication::style()->drawControl(QStyle::CE_CheckBox, &checkBoxOption, painter, pCheckBox);
}
