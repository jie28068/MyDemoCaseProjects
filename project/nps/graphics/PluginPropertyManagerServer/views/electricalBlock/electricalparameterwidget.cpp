#include "electricalparameterwidget.h"
#include "CommonModelAssistant.h"
#include "CoreLib/ServerManager.h"
#include "GlobalAssistant.h"
#include "PropertyManagerServer.h"
#include "PropertyServerMng.h"
#include "QPHelper.h"
#include <QRegExpValidator>
#include <QSvgRenderer>
#include <qmath.h>

static const int MODE_INVISIBLE = 0;
static const int MODE_READONLY = 1;
static const int MODE_EDIT = 2;
static const double CIRCUITBREAKER_ABSMAX = 0.1;

static const QString PROTOTYPE_WINDGENERATOR = "WindGenerator";               // 风力发电机/直驱风机
static const QString PROTOTYPE_DFIG = "DFIG";                                 // 双馈风机
static const QString PROTOTYPE_ENERGYSTORAGEBATTERY = "EnergyStorageBattery"; // 储能电池

USE_LOGOUT_("ElectricalParameterWidget")
ElectricalParameterWidget::ElectricalParameterWidget(QSharedPointer<Kcc::BlockDefinition::ElectricalBlock> block,
                                                     QWidget *parent)
    : CWidget(parent),
      m_pElectricalBlock(block),
      m_tableWidget(nullptr),
      m_iconLabel(nullptr),
      m_lineName(nullptr),
      m_labelName(nullptr),
      m_ctrSysReferenceLabel(nullptr),
      m_ctrSysReferencePath(nullptr),
      m_jumpBtn(nullptr)
{
    initUI();
}

ElectricalParameterWidget::~ElectricalParameterWidget() { }

QList<CustomModelItem> ElectricalParameterWidget::getTableList()
{
    if (m_tableWidget != nullptr)
        return m_tableWidget->getNewListData();
    return QList<CustomModelItem>();
}

QString ElectricalParameterWidget::getName()
{
    if (m_lineName) {
        return m_lineName->text().trimmed();
    }
    return "";
}

QSharedPointer<Kcc::BlockDefinition::ElectricalBlock> ElectricalParameterWidget::getElecBlock()
{
    return m_pElectricalBlock;
}

const int ElectricalParameterWidget::getCurPhaseNo()
{
    if (m_pElectricalBlock == nullptr) {
        return 0;
    }

    if (NPS::PROTOTYPENAME_BUSBAR == m_pElectricalBlock->getPrototypeName()
        || NPS::PROTOTYPENAME_GROUNDBUSBAR == m_pElectricalBlock->getPrototypeName()
        || NPS::PROTOTYPENAME_DOTBUSBAR == m_pElectricalBlock->getPrototypeName()) {
        CustomModelItem busItem = m_tableWidget->getItemData(NPS::KEYWORDS_BUSBAR_AC);
        switch (busItem.value.toInt()) {
        case 0:
        case 3:
            return 1;
        case 1:
            return 3;
        case 2:
            return 2;
        default:
            break;
        }
    }
    return m_pElectricalBlock->getPhaseNumber();
}

void ElectricalParameterWidget::onBusBarItemEditFinished()
{
    if (m_tableWidget != nullptr) {
        QList<CustomModelItem> listdata = getBusBarPropertyList()
                + CMA::getPropertyModelItemList(m_pElectricalBlock, RoleDataDefinition::ElectricalParameter);
        updateElecModelItemList(listdata);
        m_tableWidget->setListData(listdata);
        if (listdata.size() > 0) {
            if (PropertyServerMng::getInstance().m_pIElecSysServer != nullptr) {
                emit phaseNoChanged(PropertyServerMng::getInstance().m_pIElecSysServer->GetOutputParamMap(
                                            m_pElectricalBlock->getPrototypeName(), getCurPhaseNo()),
                                    m_pElectricalBlock);
            }
        }
        emit electricalParamChanged(listdata);
    }
}

void ElectricalParameterWidget::onJumpToConnectModel()
{
    if (m_pElectricalBlock == nullptr || PropertyServerMng::getInstance().m_ProjectManagerViewServer == nullptr) {
        return;
    }
    QList<QWeakPointer<Model>> slotModels = m_pElectricalBlock->getConnectedModel(ModelConnSlot);
    if (slotModels.size() > 0 && slotModels[0] != nullptr) {
        emit jumpToModel();
        PropertyServerMng::getInstance().m_ProjectManagerViewServer->JumpToReferenceModel(slotModels[0].toStrongRef());
        return;
    }
    QList<QWeakPointer<Model>> ElecModels = m_pElectricalBlock->getConnectedModel(ModelConnElectricInterface);
    if (ElecModels.size() > 0 && ElecModels[0] != nullptr) {
        emit jumpToModel();
        PropertyServerMng::getInstance().m_ProjectManagerViewServer->JumpToReferenceModel(ElecModels[0].toStrongRef());
        return;
    }
    QList<QWeakPointer<Model>> measureModels = m_pElectricalBlock->getConnectedModel(ModelConnElectricMesure);
    if (measureModels.size() > 0 && measureModels[0] != nullptr) {
        emit jumpToModel();
        PropertyServerMng::getInstance().m_ProjectManagerViewServer->JumpToReferenceModel(
                measureModels[0].toStrongRef());
        return;
    }
}

void ElectricalParameterWidget::onModelDataChanged(const CustomModelItem &olditem, const CustomModelItem &newitem)
{
    if (m_pElectricalBlock == nullptr || m_tableWidget == nullptr) {
        return;
    }
    if ((NPS::PROTOTYPENAME_SHUNTFILTER == m_pElectricalBlock->getPrototypeName()
         || NPS::PROTOTYPENAME_SERIESREACTOR == m_pElectricalBlock->getPrototypeName())
        && NPS::KEYWORDS_SHUNTFILTER_SHUNTTYPE == newitem.keywords) {
        QList<CustomModelItem> newlist = m_tableWidget->getNewListData();
        updateElecModelItemList(newlist);
        m_tableWidget->setListData(newlist);
    }
    updateIconLabelPix();
    emit electricalParamChanged(m_tableWidget->getNewListData());
    if ((NPS::PROTOTYPENAME_BUSBAR == m_pElectricalBlock->getPrototypeName()
         || NPS::PROTOTYPENAME_DOTBUSBAR == m_pElectricalBlock->getPrototypeName())
        && PropertyServerMng::getInstance().m_pIElecSysServer != nullptr) {
        if (newitem.keywords == NPS::KEYWORDS_BUSBAR_AC) {
            emit phaseNoChanged(PropertyServerMng::getInstance().m_pIElecSysServer->GetOutputParamMap(
                                        m_pElectricalBlock->getPrototypeName(), getCurPhaseNo()),
                                m_pElectricalBlock);
        }
    }
}

void ElectricalParameterWidget::initUI()
{
    if (m_pElectricalBlock == nullptr) {
        return;
    }
    this->setObjectName("ElectricalParameterWidget");
    QWidget *scrollWidget = new QWidget(this);
    QGridLayout *scrollLayout = new QGridLayout(scrollWidget);
    scrollLayout->setMargin(0);
    scrollLayout->setVerticalSpacing(15);
    scrollLayout->setHorizontalSpacing(10);
    m_gridLayout = new QGridLayout(this);
    m_gridLayout->setMargin(0);
    m_gridLayout->setVerticalSpacing(15);
    m_gridLayout->setHorizontalSpacing(10);
    m_bustbarWidgetList.clear();
    QString boardUUID = m_pElectricalBlock->getParentModelUUID();
    m_iconLabel = new QLabel(scrollWidget);
    QSizePolicy iconsizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    iconsizePolicy.setHorizontalStretch(0);
    iconsizePolicy.setVerticalStretch(0);
    iconsizePolicy.setHeightForWidth(m_iconLabel->sizePolicy().hasHeightForWidth());
    m_iconLabel->setSizePolicy(iconsizePolicy);
    QSpacerItem *horizontalSpacerLeft = new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Minimum);
    QSpacerItem *horizontalSpacerRight = new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Minimum);
    QHBoxLayout *iconLayout = new QHBoxLayout(scrollWidget);
    iconLayout->addSpacerItem(horizontalSpacerLeft);
    iconLayout->addWidget(m_iconLabel);
    iconLayout->addSpacerItem(horizontalSpacerRight);
    m_labelName = new QLabel(scrollWidget);
    m_labelName->setText(QObject::tr("Name"));
    m_lineName = new KLineEdit(scrollWidget);
    m_lineName->setValidator(new QRegExpValidator(QRegExp(NPS::REG_FILE_NAME), scrollWidget));
    QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(m_lineName->sizePolicy().hasHeightForWidth());
    m_lineName->setSizePolicy(sizePolicy);
    m_lineName->setMinimumSize(QSize(0, 0));
    m_ctrSysReferenceLabel = new QLabel(scrollWidget);
    m_ctrSysReferenceLabel->setText(QObject::tr("Control System Reference")); // 控制系统引用
    m_ctrSysReferencePath = new QLabel(scrollWidget);
    m_jumpBtn = new QToolButton(scrollWidget);
    m_jumpBtn->setIcon(QIcon(":/checkbox/Resources/JumpToConnectModel.png"));
    QList<QWeakPointer<Model>> ElecModels = m_pElectricalBlock->getConnectedModel(ModelConnElectricInterface);
    QList<QWeakPointer<Model>> slotModels = m_pElectricalBlock->getConnectedModel(ModelConnSlot);
    QList<QWeakPointer<Model>> measureModels = m_pElectricalBlock->getConnectedModel(ModelConnElectricMesure);
    if (slotModels.size() > 0) {
        m_jumpBtn->setEnabled(true);
        m_ctrSysReferencePath->setText(getModelBoardPath(slotModels[0].toStrongRef()));
    } else if (ElecModels.size() > 0) {
        m_jumpBtn->setEnabled(true);
        m_ctrSysReferencePath->setText(getModelBoardPath(ElecModels[0].toStrongRef()));
    } else if (measureModels.size() > 0) {
        m_jumpBtn->setEnabled(true);
        m_ctrSysReferencePath->setText(getModelBoardPath(measureModels[0].toStrongRef()));
    } else {
        m_jumpBtn->setEnabled(false);
        m_jumpBtn->hide();
        m_ctrSysReferencePath->hide();
        m_ctrSysReferenceLabel->hide();
        m_ctrSysReferencePath->setText("");
    }

    connect(m_jumpBtn, SIGNAL(released()), this, SLOT(onJumpToConnectModel()));
    m_tableWidget = new CustomTableWidget(scrollWidget);
    if (!boardUUID.isEmpty()) {
        initName = m_pElectricalBlock->getName();
        scrollLayout->addLayout(iconLayout, 0, 0, 1, 3);
        scrollLayout->addWidget(m_labelName, 1, 0, 1, 1);

        m_lineName->setEnabled(true);
        m_lineName->setText(initName);

        scrollLayout->addWidget(m_lineName, 1, 1, 1, 1);
        scrollLayout->addWidget(m_ctrSysReferenceLabel, 2, 0, 1, 1);
        scrollLayout->addWidget(m_ctrSysReferencePath, 2, 1, 1, 1);
        scrollLayout->addWidget(m_jumpBtn, 2, 2, 1, 1);

        m_busbarlayout = new QGridLayout(scrollWidget);
        m_busbarlayout->setMargin(0);
        m_busbarlayout->setVerticalSpacing(10);

        addBusbarItem();
        scrollLayout->addLayout(m_busbarlayout, 3, 0, 1, 3);
        QList<CustomModelItem> listdata = getBusBarPropertyList()
                + CMA::getPropertyModelItemList(m_pElectricalBlock, RoleDataDefinition::ElectricalParameter, false, "",
                                                QStringList() << NPS::KEYWORD_ELECTRICALPARAM_NAME);
        updateElecModelItemList(listdata);
        m_tableWidget->setListData(listdata);
        scrollLayout->addWidget(m_tableWidget, 4, 0, 1, 3);
    } else {
        QList<CustomModelItem> listdata =
                CMA::getPropertyModelItemList(m_pElectricalBlock, RoleDataDefinition::ElectricalParameter, false, "",
                                              QStringList() << NPS::KEYWORD_ELECTRICALPARAM_NAME);
        updateElecModelItemList(listdata);
        m_tableWidget->setListData(listdata);
        scrollLayout->addWidget(m_tableWidget, 0, 0, 1, 1);
        m_lineName->setEnabled(false);
    }
    updateIconLabelPix();
    connect(m_tableWidget, SIGNAL(tableDataItemChanged(const CustomModelItem &, const CustomModelItem &)), this,
            SLOT(onModelDataChanged(const CustomModelItem &, const CustomModelItem &)));
    scrollWidget->setLayout(scrollLayout);
    QScrollArea *scrollarea = new QScrollArea(this);
    scrollarea->setWidgetResizable(true);
    scrollarea->setGeometry(0, 0, scrollWidget->width(), scrollWidget->height());
    scrollarea->setWidget(scrollWidget);
    m_gridLayout->addWidget(scrollarea, 0, 0, 1, 1);
    this->setLayout(m_gridLayout);
}

void ElectricalParameterWidget::addBusbarItem()
{
    if (m_pElectricalBlock == nullptr
        || m_pElectricalBlock->getVariableGroup(RoleDataDefinition::PortGroup) == nullptr) {
        return;
    }
    if (m_pElectricalBlock->getPrototypeName() == NPS::PROTOTYPENAME_BUSBAR
        || m_pElectricalBlock->getPrototypeName() == NPS::PROTOTYPENAME_DOTBUSBAR) {
        return;
    }

    PModel pParentModel = m_pElectricalBlock->getParentModel();
    if (pParentModel.isNull()) {
        return;
    }
    QStringList portNameList;
    QList<PBlockConnector> adjConnectors = pParentModel->getAdjacentConnector(m_pElectricalBlock->getUUID());
    QList<PVariable> portlist = CMA::getVarGroupList(m_pElectricalBlock, RoleDataDefinition::PortGroup);
    for (PVariable portVar : portlist) {
        if (portVar == nullptr) {
            continue;
        }
        for (PBlockConnector pcon : adjConnectors) {
            if (pcon->getDstModel() != nullptr && (pcon->getDstModel()->getUUID() == m_pElectricalBlock->getUUID())
                && portVar->getUUID() == pcon->getDstPortUuid()) {
                // dstblock就是连着当前端口，另一端就是连接的模块
                if (pcon->getSrcModel() != nullptr
                    && addBusbarToLayout((pcon->getSrcModel()).dynamicCast<ElectricalBlock>(),
                                         portVar->getDisplayName().isEmpty() ? portVar->getName()
                                                                             : portVar->getDisplayName(),
                                         portNameList)) {
                    break;
                }
            } else if (pcon->getSrcModel() != nullptr
                       && (pcon->getSrcModel()->getUUID() == m_pElectricalBlock->getUUID())
                       && portVar->getUUID() == pcon->getSrcPortUuid()) {
                // srcBlock就是连着当前端口，另一端就是连接的模块
                if (pcon->getDstModel() != nullptr
                    && addBusbarToLayout((pcon->getDstModel()).dynamicCast<ElectricalBlock>(),
                                         portVar->getDisplayName().isEmpty() ? portVar->getName()
                                                                             : portVar->getDisplayName(),
                                         portNameList)) {
                    break;
                }
            }
        }
    }
}

bool ElectricalParameterWidget::addBusbarToLayout(QSharedPointer<Kcc::BlockDefinition::ElectricalBlock> block,
                                                  const QString &portInfoName, QStringList &portNameList)
{
    if (block == nullptr) {
        return false;
    }

    if (portNameList.contains(portInfoName)) {
        return false;
    } else {
        portNameList.append(portInfoName);
    }
    BusbarItem *item = new BusbarItem(block, portInfoName, getPath(block), this);
    connect(item, SIGNAL(busBarItemEditFinished()), this, SLOT(onBusBarItemEditFinished()));
    m_bustbarWidgetList.append(item);
    m_busbarlayout->addWidget(item, m_bustbarWidgetList.size() - 1, 0, 1, 3);
    return true;
}

QList<CustomModelItem> ElectricalParameterWidget::getBusBarPropertyList()
{
    QList<CustomModelItem> listdata;
    // 非母线的元件，显示关联的母线数据
    if (m_pElectricalBlock->getPrototypeName() == NPS::PROTOTYPENAME_BUSBAR
        || m_pElectricalBlock->getPrototypeName() == NPS::PROTOTYPENAME_DOTBUSBAR) {
        return listdata;
    }
    QStringList addBlockNameList;
    for (BusbarItem *busitem : m_bustbarWidgetList) {
        if (busitem != nullptr && busitem->getBusbarBlock() != nullptr) {
            if (addBlockNameList.contains(busitem->getBusbarBlock()->getName())) {
                continue;
            } else {
                addBlockNameList.append(busitem->getBusbarBlock()->getName());
            }
            listdata +=
                    CMA::getPropertyModelItemList(busitem->getBusbarBlock(), RoleDataDefinition::ElectricalParameter,
                                                  true, busitem->getBusbarBlock()->getName());
        }
    }
    return listdata;
}

QString ElectricalParameterWidget::getPath(QSharedPointer<Kcc::BlockDefinition::ElectricalBlock> busbarblock)
{
    if (PropertyServerMng::getInstance().m_projectManagerServer == nullptr || busbarblock == nullptr) {
        return QString("");
    }
    QString strProjpath = PropertyServerMng::getInstance().m_projectManagerServer->GetCurrentProjectRelativeDir();
    PModel boardmodel = PropertyServerMng::getInstance().m_projectManagerServer->GetBoardModelByUUID(
            busbarblock->getParentModelUUID());
    if (boardmodel != nullptr) {
        return QObject::tr("%1/%2/%3").arg(strProjpath).arg(boardmodel->getName()).arg(busbarblock->getName());
    } else {
        return QObject::tr("%1/%2").arg(strProjpath).arg(busbarblock->getName());
    }
}

CustomModelItem ElectricalParameterWidget::getItemByKeywords(const QString &keyword,
                                                             const QList<CustomModelItem> listdata)
{
    QList<CustomModelItem> realList = listdata;
    if (listdata.isEmpty()) {
        if (m_tableWidget != nullptr) {
            realList = m_tableWidget->getNewListData();
        }
    }
    for (CustomModelItem item : realList) {
        if (keyword == item.keywords) {
            return item;
        }
    }
    return CustomModelItem();
}

void ElectricalParameterWidget::setModelItemEditable(QList<CustomModelItem> &listdata,
                                                     const QMap<QString, bool> editMap)
{
    for (CustomModelItem &item : listdata) {
        if (editMap.contains(item.keywords)) {
            item.bcanEdit = editMap[item.keywords];
        }
    }
}

void ElectricalParameterWidget::updateElecModelItemList(QList<CustomModelItem> &listdata)
{
    if (m_pElectricalBlock == nullptr) {
        return;
    }

    if (NPS::PROTOTYPENAME_SHUNTFILTER == m_pElectricalBlock->getPrototypeName()
        || NPS::PROTOTYPENAME_SERIESREACTOR == m_pElectricalBlock->getPrototypeName()) {
        CustomModelItem shunttype = getItemByKeywords(NPS::KEYWORDS_SHUNTFILTER_SHUNTTYPE, listdata);
        if (!shunttype.isValid()) {
            return;
        }
        QMap<QString, bool> editMap;
        switch (shunttype.value.toInt()) {
        case NPS::SHUNTTYPE_RLC: {
            editMap.insert(NPS::KEYWORDS_SHUNTFILTER_R, true);
            editMap.insert(NPS::KEYWORDS_SHUNTFILTER_L, true);
            editMap.insert(NPS::KEYWORDS_SHUNTFILTER_C, true);
            editMap.insert(NPS::KEYWORDS_SHUNTFILTER_RP, false);
            editMap.insert(NPS::KEYWORDS_SHUNTFILTER_CS, false);
            break;
        }
        case NPS::SHUNTTYPE_RL: {
            editMap.insert(NPS::KEYWORDS_SHUNTFILTER_R, true);
            editMap.insert(NPS::KEYWORDS_SHUNTFILTER_L, true);
            editMap.insert(NPS::KEYWORDS_SHUNTFILTER_C, false);
            editMap.insert(NPS::KEYWORDS_SHUNTFILTER_RP, false);
            editMap.insert(NPS::KEYWORDS_SHUNTFILTER_CS, false);
            break;
        }
        case NPS::SHUNTTYPE_RC: {
            editMap.insert(NPS::KEYWORDS_SHUNTFILTER_R, true);
            editMap.insert(NPS::KEYWORDS_SHUNTFILTER_L, false);
            editMap.insert(NPS::KEYWORDS_SHUNTFILTER_C, true);
            editMap.insert(NPS::KEYWORDS_SHUNTFILTER_RP, false);
            editMap.insert(NPS::KEYWORDS_SHUNTFILTER_CS, false);
            break;
        }
        case NPS::SHUNTTYPE_RLC_R: {
            editMap.insert(NPS::KEYWORDS_SHUNTFILTER_R, true);
            editMap.insert(NPS::KEYWORDS_SHUNTFILTER_L, true);
            editMap.insert(NPS::KEYWORDS_SHUNTFILTER_C, true);
            editMap.insert(NPS::KEYWORDS_SHUNTFILTER_RP, true);
            editMap.insert(NPS::KEYWORDS_SHUNTFILTER_CS, false);
            break;
        }
        case NPS::SHUNTTYPE_RLCC_R: {
            editMap.insert(NPS::KEYWORDS_SHUNTFILTER_R, true);
            editMap.insert(NPS::KEYWORDS_SHUNTFILTER_L, true);
            editMap.insert(NPS::KEYWORDS_SHUNTFILTER_C, true);
            editMap.insert(NPS::KEYWORDS_SHUNTFILTER_RP, true);
            editMap.insert(NPS::KEYWORDS_SHUNTFILTER_CS, true);
            break;
        }
        default:
            break;
        }
        setModelItemEditable(listdata, editMap);
    } else if (NPS::PROTOTYPENAME_CIRCUITBREAKER == m_pElectricalBlock->getPrototypeName()) {
        for (CustomModelItem &item : listdata) {
            if (NPS::KEYWORDS_CIRCUITBREAKER_STATUS0 == item.keywords
                || NPS::KEYWORDS_CIRCUITBREAKER_STATUS1 == item.keywords) {
                item.valueType = NPS::DataType_BreakerTimeNodeList;
            }
        }
    }
}

QString ElectricalParameterWidget::getModelBoardPath(QSharedPointer<Kcc::BlockDefinition::Model> model)
{
    if (PropertyServerMng::getInstance().m_projectManagerServer == nullptr || model == nullptr) {
        return QString("");
    }
    QString relativePath = PropertyServerMng::getInstance().m_projectManagerServer->GetCurrentProjectRelativeDir();
    QString boardBlockPath = model->getName();
    PModel parentModel = model->getParentModel();
    while (parentModel != nullptr) {
        boardBlockPath = parentModel->getName() + "/" + boardBlockPath;
        parentModel = parentModel->getParentModel();
    }
    return QString("%1/%2").arg(relativePath).arg(boardBlockPath);
}

void ElectricalParameterWidget::updateIconLabelPix()
{
    if (m_iconLabel == nullptr) {
        return;
    }

    if (PROTOTYPE_WINDGENERATOR == m_pElectricalBlock->getPrototypeName()
        || PROTOTYPE_DFIG == m_pElectricalBlock->getPrototypeName()
        || PROTOTYPE_ENERGYSTORAGEBATTERY == m_pElectricalBlock->getPrototypeName()) {
        QPixmap pix(QString(":/TheoryGraph/Resources/TheoryGraph/%1-theory-%2.png")
                            .arg(m_pElectricalBlock->getPrototypeName())
                            .arg(QPHelper::getLanguage() == "zh_CN" ? "cn" : "en"));
        m_iconLabel->setPixmap(pix);
    } else if (NPS::PROTOTYPENAME_SERIESREACTOR == m_pElectricalBlock->getPrototypeName()
               || NPS::PROTOTYPENAME_SHUNTFILTER == m_pElectricalBlock->getPrototypeName()) {
        CustomModelItem shunttype = getItemByKeywords(NPS::KEYWORDS_SHUNTFILTER_SHUNTTYPE);
        m_iconLabel->setPixmap(QPixmap());
        if (shunttype.isValid()) {
            QString SuffixStr;
            switch (shunttype.value.toInt()) {
            case NPS::SHUNTTYPE_RLC: {
                SuffixStr = "R-L-C";
                break;
            }
            case NPS::SHUNTTYPE_RL: {
                SuffixStr = "R-L";
                break;
            }
            case NPS::SHUNTTYPE_RC: {
                SuffixStr = "R-C";
                break;
            }
            case NPS::SHUNTTYPE_RLC_R: {
                SuffixStr = "R-L-C|R";
                break;
            }
            case NPS::SHUNTTYPE_RLCC_R: {
                SuffixStr = "R-L-C-C|R";
                break;
            }
            default:
                break;
            }
            if (!SuffixStr.isEmpty()) {
                QSvgRenderer svgRenderer(m_pElectricalBlock->getResource()[SuffixStr].toByteArray());
                QPixmap Image(svgRenderer.defaultSize());
                Image.fill(Qt::transparent);
                QPainter painter(&Image);
                svgRenderer.render(&painter);
                m_iconLabel->setPixmap(Image);
            }
        }
        // return QPixmap::fromImage(qvariant_cast<QImage>(m_pElectricalBlock->getResource()[SuffixStr]));
    } else {
        m_iconLabel->hide();
    }
}

void ElectricalParameterWidget::resetName()
{
    m_lineName->setText(initName);
}

bool ElectricalParameterWidget::saveData()
{
    if (m_pElectricalBlock == nullptr) {
        return false;
    }
    bool bchange = false;

    if (CMA::saveCustomModeListDataToModel(m_pElectricalBlock, m_tableWidget->getNewListData(),
                                           RoleDataDefinition::ElectricalParameter)) {
        bchange = true;
    }
    int curPhaseNo = getCurPhaseNo();
    if (m_pElectricalBlock->getPhaseNumber() != curPhaseNo) {
        m_pElectricalBlock->setPhaseNumber(curPhaseNo);
    }
    QString oldname = m_pElectricalBlock->getName();
    if (oldname != getName()) {
        m_pElectricalBlock->setName(getName());
        LOGOUT(NPS::outputLog(CMA::LogTag_ElectricalComponent, oldname, QObject::tr("Name"), oldname, getName()));
        bchange = true;
    }
    return bchange;
}

bool ElectricalParameterWidget::checkExcludeName(QString &errorinfo)
{
    if (m_tableWidget != nullptr && !m_tableWidget->checkLegitimacy(errorinfo)) {
        return false;
    }
    return true;
}

bool ElectricalParameterWidget::checkLegitimacy(QString &errorinfo)
{
    errorinfo = "";
    if (getName() != m_pElectricalBlock->getName() && !CMA::checkNameValid(getName(), m_pElectricalBlock, errorinfo)) {
        return false;
    }

    if (!checkExcludeName(errorinfo)) {
        return false;
    }

    return true;
}

void ElectricalParameterWidget::setCWidgetReadOnly(bool bReadOnly)
{
    if (m_labelName != nullptr) {
        m_labelName->setDisabled(bReadOnly);
    }
    if (m_lineName != nullptr) {
        m_lineName->setDisabled(bReadOnly);
    }
    if (m_ctrSysReferenceLabel != nullptr) {
        m_ctrSysReferenceLabel->setDisabled(bReadOnly);
    }
    if (m_ctrSysReferencePath != nullptr) {
        m_ctrSysReferencePath->setDisabled(bReadOnly);
    }
    if (m_jumpBtn != nullptr) {
        m_jumpBtn->setDisabled(bReadOnly);
    }
    for (BusbarItem *busitem : m_bustbarWidgetList) {
        if (busitem != nullptr) {
            busitem->setCWidgetReadOnly(bReadOnly);
        }
    }

    if (m_tableWidget != nullptr) {
        m_tableWidget->setCWidgetReadOnly(bReadOnly);
    }
}

// 母线item
BusbarItem::BusbarItem(QSharedPointer<Kcc::BlockDefinition::ElectricalBlock> block, const QString &name,
                       const QString &path, QWidget *parent /*= nullptr*/)
    : CWidget(parent), m_pBusbarName(nullptr), m_pEditBusbar(nullptr), m_pBusbarPath(nullptr), m_block(block)
{
    m_pBusbarName = new QLabel(this);
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(m_pBusbarName->sizePolicy().hasHeightForWidth());
    m_pBusbarName->setSizePolicy(sizePolicy);
    m_pBusbarName->setMinimumWidth(60);
    setBusbarName(name);

    m_pEditBusbar = new QPushButton(this);
    m_pEditBusbar->setText(QObject::tr("Edit"));
    m_pEditBusbar->setMinimumWidth(30);
    m_pEditBusbar->setMaximumWidth(30);
    if (path.isEmpty()) {
        m_pEditBusbar->setEnabled(false);
    }

    m_pBusbarPath = new ElideLineText(this);
    m_pBusbarPath->setElideText(path);

    QGridLayout *gridLayout = new QGridLayout(this);
    gridLayout->addWidget(m_pBusbarName, 0, 0, 1, 2);
    gridLayout->addWidget(m_pEditBusbar, 0, 1, 1, 1);
    gridLayout->addWidget(m_pBusbarPath, 0, 2, 1, 3);
    gridLayout->setMargin(0);
    gridLayout->setVerticalSpacing(15);
    gridLayout->setHorizontalSpacing(10);
    this->setLayout(gridLayout);
    connect(m_pEditBusbar, SIGNAL(clicked(bool)), this, SLOT(onEditBusbarClicked(bool)));
}

BusbarItem::~BusbarItem() { }

void BusbarItem::setBusbarName(const QString &name)
{
    QFontMetrics fontWidth(this->font());
    QString elidnote = fontWidth.elidedText(name, Qt::ElideRight, m_pBusbarName->width() - 36);
    m_pBusbarName->setText(elidnote);
    m_pBusbarName->setToolTip(name);
}

QSharedPointer<Kcc::BlockDefinition::ElectricalBlock> BusbarItem::getBusbarBlock()
{
    return m_block;
}

void BusbarItem::resizeEvent(QResizeEvent *event)
{
    if (m_pBusbarName != nullptr) {
        setBusbarName(m_pBusbarName->toolTip());
    }
}

void BusbarItem::onEditBusbarClicked(bool checked)
{
    if (m_block.isNull()) {
        return;
    }
    QSharedPointer<IPropertyManagerServer> propertyServer = RequestServer<IPropertyManagerServer>();
    if (!propertyServer) {
        return;
    }
    if (propertyServer->ShowBlockProperty(m_block)) {
        QString busbarname = m_block->getName();
        QString oldpath = m_pBusbarPath->text();
        QStringList tmplist = oldpath.split("/");
        tmplist[tmplist.size() - 1] = busbarname;
        QString newpath = tmplist.join("/");
        m_pBusbarPath->setElideText(newpath);
        emit busBarItemEditFinished();
    }
}