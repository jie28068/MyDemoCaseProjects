#include "ComplexInstanceWidget.h"
#include "CommonModelAssistant.h"
#include "CoreLib/ServerManager.h"
#include "GlobalAssistant.h"
#include "KLWidgets/KCustomDialog.h"
#include "KLWidgets/KMessageBox.h"
#include "PropertyServerMng.h"
#include "controlblocktypeselectview.h"
#include <QGridLayout>
#include <QUuid>

#include "CoreLib/ServerManager.h"
USE_LOGOUT_("ComplexInstanceWidget")

static const int BLOCK_ID_ROLE = Qt::UserRole + 1;
static const int SLOT_ID_ROLE = Qt::UserRole + 2;
static const int IS_SM_ROLE = Qt::UserRole + 3;

static const QString SM_PROTOTYPE_NAME = "SynchronousMachine";
static const QString EI_PROTOTYPE_NAME = "ElectricalInterface";
static const QString IS_CHOOSE_ELEMENT = "IsChooseElement";

static const QString LABEL_TEXT_INSTANCENAME = QObject::tr("Instance Name");           // 实例名称
static const QString LABEL_TEXT_INSTANCEPROTOTYPE = QObject::tr("Instance Prototype"); // 实例原型
static const QString TABLE_TITLE_SLOT = QObject::tr("Slot");                           // 插槽
static const QString TABLE_TITLE_ELEMENT = QObject::tr("Element");                     // 元件
static const int LABEL_WIDTH = 160;

ComplexInstanceWidget::ComplexInstanceWidget(QSharedPointer<Kcc::BlockDefinition::Model> complexModel,
                                             ComplexInstanceWidget::Operation ope, const QString &cpxProtoName,
                                             QWidget *parent)
    : CWidget(parent),
      m_complexModel(complexModel),
      m_InstanceOpe(ope),
      m_cpxProtoModelNameInit(cpxProtoName),
      m_cacheComplexInstance(nullptr),
      m_leditInstanceName(nullptr),
      m_cboxComplexProto(nullptr),
      m_tableView(nullptr),
      m_customTableModel(nullptr),
      m_selectInstanceDlg(nullptr),
      m_clearElement(nullptr)
{
    initUI();
}

ComplexInstanceWidget::~ComplexInstanceWidget() { }

QSharedPointer<Kcc::BlockDefinition::Model> ComplexInstanceWidget::getComplexInstance()
{
    return m_complexModel;
}

bool ComplexInstanceWidget::saveData()
{
    bool bchange = false;
    PModel realComplexInstance(nullptr);
    if (ComplexInstanceWidget::Operation_NewInstance == m_InstanceOpe) {
        PModel prototypeModel = getCurComplexPrototype();
        if (prototypeModel == nullptr) {
            return bchange;
        }
        realComplexInstance = prototypeModel->createInstance();
        renameComplexInstanceSlot(realComplexInstance, prototypeModel);
        m_complexModel = realComplexInstance;
        bchange = true;
    } else if (ComplexInstanceWidget::Operation_EditInstance == m_InstanceOpe) {
        realComplexInstance = m_complexModel;
    }
    if (realComplexInstance == nullptr) {
        return bchange;
    }

    for (PModel cmodel : realComplexInstance->getChildModels().values()) {
        if (cmodel == nullptr) {
            continue;
        }
        PSlotBlock pslot = cmodel.dynamicCast<SlotBlock>();
        if (pslot == nullptr) {
            continue;
        }
        if (pslot->getTargetModel() != m_SlotTargetModelMap[pslot->getName()]) {
            if (m_SlotTargetModelMap[pslot->getName()] == nullptr) {
                pslot->clearConnectedModel(ModelConnSlot);
            } else {
                pslot->setTargetModel(m_SlotTargetModelMap[pslot->getName()]);
            }
            bchange = true;
        }
    }
    if (getInstanceName() != realComplexInstance->getName()) {
        realComplexInstance->setName(getInstanceName());
        bchange = true;
    }
    if (bchange) {
        realComplexInstance->setModifyTime(QDateTime::currentDateTime());
    }

    return bchange;
}

bool ComplexInstanceWidget::checkLegitimacy(QString &errorinfo)
{
    QString name = getInstanceName();
    if (name.isEmpty()) {
        errorinfo = NPS::ERRORINFO_BLOCKNAME_EMPTY;
        return false;
    }

    PModel model = getCurComplexPrototype();
    if (model == nullptr) {
        errorinfo = tr("%1 cannot be empty.").arg(LABEL_TEXT_INSTANCEPROTOTYPE);
        return false;
    }
    if (m_complexModel != nullptr && m_complexModel->getName() == name) {
        return true;
    }
    if (name.length() > BLOCK_NAME_MAX_LENGHT) {
        errorinfo = NPS::ERRORINFO_NAME_EXCEEDLENGTH;
        return false;
    }
    if (model->getName() == name) {
        errorinfo = NPS::ERRORINFO_REPEAT;
        return false;
    }
    // 实例不能和同类型的重名
    QList<PModel> smodels = CMA::getProjectModels(model->getModelType());
    for (PModel smodel : smodels) {
        if (smodel == nullptr) {
            continue;
        }
        for (PModel instanceModel : smodel->getInstanceList()) {
            if (instanceModel != nullptr
                && (QString::compare(instanceModel->getName(), name, Qt::CaseInsensitive) == 0)) {
                errorinfo = NPS::ERRORINFO_REPEAT;
                return false;
            }
        }
    }
    return true;
}

void ComplexInstanceWidget::onTableDoubleClicked(const QModelIndex &index)
{
    if (!index.isValid() || index.column() != 1) {
        return;
    }
    PSlotBlock pslot = getSlotByIndex(index);
    if (pslot != nullptr && m_SlotTargetModelMap[pslot->getName()] != nullptr) {
        QString oldName = pslot->getName();
        if (PropertyServerMng::getInstance().propertyServer != nullptr
            && PropertyServerMng::getInstance().propertyServer->ShowBlockProperty(
                    m_SlotTargetModelMap[pslot->getName()])) {
            setSlotElement(index, m_SlotTargetModelMap[pslot->getName()]);
        }
        return;
    }

    if (PropertyServerMng::getInstance().propertyServer != nullptr) {
        PModel model = PropertyServerMng::getInstance().propertyServer->selectBlock(
                QObject::tr("Select Module Instance"), ControlBlockTypeSelectView::SelectType_AllInstance,
                pslot != nullptr ? pslot->getFilterInfoVariable() : PVariable(nullptr));
        if (model != nullptr && connectedComfirm(model) && phasecomfirm(pslot, model)) {
            setSlotElement(index, model);
        }
    }
}

void ComplexInstanceWidget::onTableMenuRequested(const QPoint &point)
{
    if (m_tableView == nullptr) {
        return;
    }

    QModelIndex index = m_tableView->indexAt(point);
    if (!index.isValid() || index.column() != 1) {
        return;
    }
    QMenu *menu = new QMenu;
    if (m_clearElement == nullptr) {
        m_clearElement = new QAction(QObject::tr("Clear Element")); // 清除元件
        connect(m_clearElement, SIGNAL(triggered()), this, SLOT(onClearElement()));
    }

    menu->addAction(m_clearElement);
    menu->setObjectName("uniformStyleMenu_cV");
    menu->exec(cursor().pos());
}

void ComplexInstanceWidget::onComplexProtoChanged(const QString &textstr)
{
    if (m_InstanceOpe != ComplexInstanceWidget::Operation_NewInstance) {
        return;
    }
    m_customTableModel->setModelData(getModelList(getCurComplexPrototype()),
                                     QStringList() << TABLE_TITLE_SLOT << TABLE_TITLE_ELEMENT);
}

void ComplexInstanceWidget::onClearElement()
{
    if (m_tableView == nullptr) {
        return;
    }
    QModelIndex index = m_tableView->currentIndex();
    if (!index.isValid()) {
        return;
    }
    QString slotname = index.data().toString();
    setSlotElement(index, PModel(nullptr));
}

void ComplexInstanceWidget::initUI()
{
    m_SlotTargetModelMap.clear();
    if (m_complexModel == nullptr && m_InstanceOpe != ComplexInstanceWidget::Operation_NewInstance)
        return;

    QGridLayout *pGridLayout = new QGridLayout(this);
    QLabel *plabelInstanceName = new QLabel(this);
    plabelInstanceName->setFixedWidth(LABEL_WIDTH);
    plabelInstanceName->setText(LABEL_TEXT_INSTANCENAME);
    m_leditInstanceName = new KLineEdit(this);
    m_leditInstanceName->setValidator(new QRegExpValidator(QRegExp(NPS::REG_FILE_NAME)));

    QLabel *plabelComplexProto = new QLabel(this);
    plabelComplexProto->setFixedWidth(LABEL_WIDTH);
    plabelComplexProto->setText(LABEL_TEXT_INSTANCEPROTOTYPE);
    m_cboxComplexProto = new QComboBox(this);
    m_cboxComplexProto->setView(new QListView());
    m_tableView = new TableView(this);
    m_tableView->verticalHeader()->setVisible(false);
    m_tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    m_tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_tableView->setItemDelegate(new CustomTableItemDelegate(this));
    m_customTableModel = new PropertyTableModel(m_tableView);
    m_customTableModel->setModelDataColorUse(false);
    m_tableView->setModel(m_customTableModel);
    QObject::connect(m_tableView, SIGNAL(doubleClicked(const QModelIndex &)), this,
                     SLOT(onTableDoubleClicked(const QModelIndex &)));
    QObject::connect(m_tableView, SIGNAL(customContextMenuRequested(const QPoint &)), this,
                     SLOT(onTableMenuRequested(const QPoint &)));
    QObject::connect(m_cboxComplexProto, SIGNAL(currentTextChanged(const QString &)), this,
                     SLOT(onComplexProtoChanged(const QString &)));
    // data
    m_allComplexBoards =
            PropertyServerMng::getInstance().m_projectManagerServer->GetAllBoardModel(ComplexBoardModel::Type);
    if (m_InstanceOpe == ComplexInstanceWidget::Operation_NewInstance) {
        m_leditInstanceName->setEnabled(true);
        m_leditInstanceName->setText("");
        if (m_cpxProtoModelNameInit.isEmpty()) {
            m_cboxComplexProto->setEnabled(true);
            QStringList namelist;
            for (PModel model : m_allComplexBoards) {
                if (model != nullptr) {
                    namelist.append(model->getName());
                }
            }
            m_cboxComplexProto->addItems(namelist);
            m_cboxComplexProto->setCurrentIndex(-1);
            m_customTableModel->setModelData(QList<CustomModelItem>(),
                                             QStringList() << TABLE_TITLE_SLOT << TABLE_TITLE_ELEMENT);
        } else {
            m_cboxComplexProto->setEnabled(false);
            m_cboxComplexProto->addItems(QStringList() << m_cpxProtoModelNameInit);
            m_cboxComplexProto->setCurrentText(m_cpxProtoModelNameInit);
            m_customTableModel->setModelData(getModelList(getCurComplexPrototype()),
                                             QStringList() << TABLE_TITLE_SLOT << TABLE_TITLE_ELEMENT);
        }

    } else {
        m_leditInstanceName->setEnabled(true);
        m_cboxComplexProto->setEnabled(false);
        m_leditInstanceName->setText(m_complexModel->getName());
        m_cboxComplexProto->addItems(QStringList() << m_complexModel->getPrototypeName());
        m_cboxComplexProto->setCurrentIndex(0);
        m_customTableModel->setModelData(getModelList(m_complexModel),
                                         QStringList() << TABLE_TITLE_SLOT << TABLE_TITLE_ELEMENT);
    }

    pGridLayout->addWidget(plabelInstanceName, 0, 0, 1, 1);
    pGridLayout->addWidget(m_leditInstanceName, 0, 1, 1, 3);
    pGridLayout->addWidget(plabelComplexProto, 1, 0, 1, 1);
    pGridLayout->addWidget(m_cboxComplexProto, 1, 1, 1, 3);
    pGridLayout->addWidget(m_tableView, 2, 0, 1, 4);

    this->setLayout(pGridLayout);
}

QSharedPointer<Kcc::BlockDefinition::Model> ComplexInstanceWidget::getCurComplexPrototype()
{
    if (m_cboxComplexProto == nullptr) {
        return PModel(nullptr);
    }
    QString ProtoComplexName = m_cboxComplexProto->currentText();

    if (ProtoComplexName.isEmpty()) {
        return PModel(nullptr);
    }
    for (PModel model : m_allComplexBoards) {
        if (model != nullptr && model->getName() == ProtoComplexName) {
            return model;
        }
    }

    return PModel(nullptr);
}

QList<CustomModelItem> ComplexInstanceWidget::getModelList(QSharedPointer<Kcc::BlockDefinition::Model> model)
{
    m_SlotTargetModelMap.clear();
    if (model == nullptr) {
        return QList<CustomModelItem>();
    }
    QList<CustomModelItem> listdata;
    for (PModel cmodel : model->getChildModels().values()) {
        if (model == nullptr) {
            continue;
        }
        PSlotBlock pslot = cmodel.dynamicCast<SlotBlock>();
        if (pslot == nullptr) {
            continue;
        }
        listdata.append(CustomModelItem(pslot->getName(), pslot->getName(),
                                        pslot->getTargetModel() == nullptr ? "" : pslot->getTargetModel()->getName(),
                                        RoleDataDefinition::ControlTypeTextbox, false));
        m_SlotTargetModelMap.insert(pslot->getName(), pslot->getTargetModel());
    }
    qSort(listdata.begin(), listdata.end(),
          [&listdata](const CustomModelItem &lhs, const CustomModelItem &rhs) -> bool { return lhs.name < rhs.name; });
    return listdata;
}

QSharedPointer<Kcc::BlockDefinition::SlotBlock> ComplexInstanceWidget::getSlotByIndex(const QModelIndex &index)
{
    if (!index.isValid()) {
        return PSlotBlock(nullptr);
    }

    PModel uiComplexModel(nullptr);
    if (ComplexInstanceWidget::Operation_NewInstance == m_InstanceOpe) {
        uiComplexModel = getCurComplexPrototype();
    } else if (ComplexInstanceWidget::Operation_EditInstance == m_InstanceOpe) {
        uiComplexModel = m_complexModel;
    }
    if (uiComplexModel == nullptr) {
        return PSlotBlock(nullptr);
    }
    QString slotname = index.sibling(index.row(), 0).data().toString();
    if (slotname.isEmpty()) {
        return PSlotBlock(nullptr);
    }
    for (PModel cmodel : uiComplexModel->getChildModels().values()) {
        if (cmodel == nullptr || cmodel->getName() != slotname) {
            continue;
        }
        PSlotBlock pslot = cmodel.dynamicCast<SlotBlock>();
        if (pslot != nullptr) {
            return pslot;
        }
    }
    return PSlotBlock(nullptr);
}

void ComplexInstanceWidget::setSlotElement(const QModelIndex &index,
                                           QSharedPointer<Kcc::BlockDefinition::Model> elementModel)
{
    if (!index.isValid()) {
        return;
    }
    PSlotBlock pslot = getSlotByIndex(index);
    if (pslot == nullptr) {
        return;
    }
    m_SlotTargetModelMap[pslot->getName()] = elementModel;
    QString instanceName;
    if (elementModel != nullptr) {
        instanceName = elementModel->getName();
    }

    if (m_customTableModel != nullptr) {
        m_customTableModel->updateValue(index.data(NPS::ModelDataKeywordsRole).toString(), instanceName);
    }
}

QString ComplexInstanceWidget::getInstanceName()
{
    if (m_leditInstanceName != nullptr) {
        return m_leditInstanceName->text().trimmed();
    }
    return QString();
}

void ComplexInstanceWidget::renameComplexInstanceSlot(QSharedPointer<Kcc::BlockDefinition::Model> instanceModel,
                                                      QSharedPointer<Kcc::BlockDefinition::Model> protoModel)
{
    if (protoModel == nullptr || instanceModel == nullptr) {
        return;
    }

    for (PModel instanceSlot : instanceModel->getChildModels()) {
        if (instanceSlot == nullptr) {
            continue;
        }
        for (PModel protoSlot : protoModel->getChildModels()) {
            if (instanceSlot->getModelUuid() == protoSlot->getModelUuid()) {
                instanceSlot->setName(protoSlot->getName());
                break;
            }
        }
    }
}

bool ComplexInstanceWidget::connectedComfirm(QSharedPointer<Kcc::BlockDefinition::Model> model)
{
    if (model == nullptr) {
        return false;
    }
    bool curselect = false;
    QString preSelectSlotName = "";
    for (QString slotname : m_SlotTargetModelMap.keys()) {
        if (model == m_SlotTargetModelMap[slotname]) {
            preSelectSlotName = slotname;
            curselect = true;
        }
    }
    if (curselect) {
        // 当前模块已被其他模块连接，是否继续选择？
        if (KMessageBox::question(tr("The current model is already connected to another model. Are you sure to "
                                     "continue selecting ?"),
                                  KMessageBox::Yes | KMessageBox::No)
            == KMessageBox::Yes) {
            m_SlotTargetModelMap.remove(preSelectSlotName);
            if (m_customTableModel != nullptr) {
                m_customTableModel->updateValue(preSelectSlotName, "");
            }
            return true;
        } else {
            return false;
        }
    }
    if (model->getConnectedModel(ModelConnSlot).size() > 0
        || model->getConnectedModel(ModelConnElectricInterface).size() > 0) {
        // 当前模块已被其他模块连接，是否继续选择？
        if (KMessageBox::question(tr("The current model is already connected to another model. Are you sure to "
                                     "continue selecting ?"),
                                  KMessageBox::Yes | KMessageBox::No)
            != KMessageBox::Yes) {
            return false;
        }
    }
    return true;
}

bool ComplexInstanceWidget::phasecomfirm(QSharedPointer<Kcc::BlockDefinition::SlotBlock> slot,
                                         QSharedPointer<Kcc::BlockDefinition::Model> model)
{
    if (slot == nullptr || model == nullptr) {
        return false;
    }
    if (ElectricalBlock::Type != model->getModelType()
        || PropertyServerMng::getInstance().m_ModelManagerServer == nullptr) {
        return true;
    }

    PModel selectModel = PropertyServerMng::getInstance().m_ModelManagerServer->GetToolkitModel(
            slot->getFilterInfo(RoleDataDefinition::FilerProtoTypeRole).toString(), KL_TOOLKIT::ELECTRICAL_TOOLKIT);
    int phaseInit = slot->getFilterInfo(RoleDataDefinition::FilerPhaseNoRole).toInt();
    if (selectModel != nullptr && phaseInit == 0) {
        phaseInit = model->getValueByRoleData(RoleDataDefinition::ElectricalGroup, RoleDataDefinition::VariableTypeRole,
                                              RoleDataDefinition::PhaseNumber)
                            .toUInt();
    }
    if (phaseInit != 0
        && phaseInit
                != model->getValueByRoleData(RoleDataDefinition::ElectricalGroup, RoleDataDefinition::VariableTypeRole,
                                             RoleDataDefinition::PhaseNumber)
                           .toUInt()) {
        // 插槽输入输出端口与选择模块端口不符，是否继续选择
        if (KMessageBox::question(tr("The slot input and output ports do not match the selected module ports. "
                                     "Do you want to continue selecting?"),
                                  KMessageBox::Yes | KMessageBox::No)
            != KMessageBox::Yes) {
            return false;
        }
    }
    return true;
}
