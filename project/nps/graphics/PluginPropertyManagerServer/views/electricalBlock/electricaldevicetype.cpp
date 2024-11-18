#include "electricaldevicetype.h"

#include <QDebug>
#include <QMenu>
#include <QPainter>
#include <QSplitter>
#include <QStackedWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <qmath.h>

#include "CommonModelAssistant.h"
#include "CoreLib/ServerManager.h"
#include "GlobalAssistant.h"
#include "KLModelDefinitionCore/KLModelDefinitionCore.h"
#include "PropertyServerMng.h"

using namespace Kcc::BlockDefinition;

static const int TableCol_Width = 110;

USE_LOGOUT_("ElectricalDeviceType")

ElectricalDeviceType::ElectricalDeviceType(PElectricalBlock block, QWidget *parent)
    : CWidget(parent),
      m_pElectricalBlock(block),
      m_currentDevicetype(nullptr),
      m_originalDevicetype(nullptr),
      m_customTableWidget(nullptr)
{
    ui.setupUi(this);
    initUI();
}

ElectricalDeviceType::~ElectricalDeviceType() { }

bool ElectricalDeviceType::saveData()
{
    bool bIsSuccessSave = false;
    // 电气元件选择设备类型
    if (m_pElectricalBlock->getDeviceModel() == nullptr && m_currentDevicetype != nullptr) {
        m_pElectricalBlock->setDeviceModel(m_currentDevicetype);
        LOGOUT(NPS::outputLog(CMA::LogTag_ElectricalComponent, m_pElectricalBlock->getName(), DEVICE_TYPE,
                              QObject::tr("non"), m_currentDevicetype->getName()));
        bIsSuccessSave = true;
    } else if (m_pElectricalBlock->getDeviceModel() != nullptr && m_currentDevicetype == nullptr) {
        auto deviceModel = m_pElectricalBlock->getDeviceModel();
        m_pElectricalBlock->clearConnectedModel(ModelConnDeviceType);
        LOGOUT(NPS::outputLog(CMA::LogTag_ElectricalComponent, m_pElectricalBlock->getName(), DEVICE_TYPE,
                              deviceModel->getName(), QObject::tr("non")));
        bIsSuccessSave = true;
    } else if (m_pElectricalBlock->getDeviceModel() != nullptr && m_currentDevicetype != nullptr) {
        if (m_pElectricalBlock->getDeviceModel() != m_currentDevicetype) {
            LOGOUT(NPS::outputLog(CMA::LogTag_ElectricalComponent, m_pElectricalBlock->getName(), DEVICE_TYPE,
                                  m_pElectricalBlock->getDeviceModel()->getName(), m_currentDevicetype->getName()));
            m_pElectricalBlock->setDeviceModel(m_currentDevicetype);
            bIsSuccessSave = true;
        } else {
            if (m_oldName != m_pElectricalBlock->getDeviceModel()->getName()) {
                bIsSuccessSave = true;
            }
            if (m_oldList != m_customTableWidget->getNewListData()) {
                bIsSuccessSave = true;
            }
        }
    }

    // 线路元件连接节点
    auto modelConnectionList = m_pElectricalBlock->getConnectedModel(ModelLineNodeLink);
    if (1 == modelConnectionList.size() && m_pCurLineComponent != nullptr) {
        auto oldConnectionNode = modelConnectionList.first();
        if (oldConnectionNode != nullptr) {
            auto oldConnectionNodeObject = oldConnectionNode.toStrongRef();
            if (oldConnectionNodeObject != m_pCurLineComponent) {
                m_pElectricalBlock->disconnectModel(oldConnectionNodeObject, ModelLineNodeLink);
                m_pElectricalBlock->connectModel(m_pCurLineComponent, ModelLineNodeLink, true);
                LOGOUT(NPS::outputLog(CMA::LogTag_ElectricalComponent, m_pElectricalBlock->getName(), NODE_OBJECT,
                                      oldConnectionNodeObject->getName(), m_pCurLineComponent->getName()));
                bIsSuccessSave = true;
            }
        }
    } else if (1 == modelConnectionList.size() && m_pCurLineComponent == nullptr) {
        auto oldConnectionNode = modelConnectionList.first();
        if (oldConnectionNode != nullptr) {
            auto oldConnectionNodeObject = oldConnectionNode.toStrongRef();
            m_pElectricalBlock->disconnectModel(m_pCurLineComponent, ModelLineNodeLink);
            LOGOUT(NPS::outputLog(CMA::LogTag_ElectricalComponent, m_pElectricalBlock->getName(), NODE_OBJECT,
                                  oldConnectionNodeObject->getName(), QObject::tr("non")));
            bIsSuccessSave = true;
        }
    } else if (0 == modelConnectionList.size() && m_pCurLineComponent != nullptr) {
        m_pElectricalBlock->clearConnectedModel(ModelLineNodeLink);
        m_pElectricalBlock->connectModel(m_pCurLineComponent, ModelLineNodeLink, true);
        LOGOUT(NPS::outputLog(CMA::LogTag_ElectricalComponent, m_pElectricalBlock->getName(), NODE_OBJECT,
                              QObject::tr("non"), m_pCurLineComponent->getName()));
        bIsSuccessSave = true;
    }

    return bIsSuccessSave;
}

void ElectricalDeviceType::onEditClicked(bool checked)
{
    // 原始设备类型编辑的时候做新建操作
    if (m_currentDevicetype == m_originalDevicetype && m_originalDevicetype != nullptr) {
        openDeviceTypeDlg(NEW_DEVICE_TYPE, PDeviceModel(nullptr), true);
    } else if (m_currentDevicetype != nullptr) {
        openDeviceTypeDlg(EDIT_DEVICE_TYPE, m_currentDevicetype, false);
    }
}

void ElectricalDeviceType::onTypenameChanged(const QString &text)
{
    if (text.isEmpty() || m_currentDevicetype == nullptr || (ui.typename_2 != nullptr && !ui.typename_2->isEnabled())) {
        ui.editButton->setEnabled(false);
    } else {
        ui.editButton->setEnabled(true);
    }
}

void ElectricalDeviceType::onSelectDevieTypeClicked()
{
    openDeviceListDlg(SELECT_DEVICE_TYPE);
}

void ElectricalDeviceType::onNewProjTypeClicked()
{
    openDeviceTypeDlg(NEW_DEVICE_TYPE, PDeviceModel(nullptr), true);
}

void ElectricalDeviceType::onRemoveTypeClicked()
{
    m_currentDevicetype = PDeviceModel(nullptr);
    if (m_pCurLineComponent) {
        m_pCurLineComponent->clearConnectedModel(ModelConnDeviceType);
    }
    updateViewData();
    setEnabelSelectNodeStatus();
    emit deviceTypeChanged(m_currentDevicetype);
}

void ElectricalDeviceType::onTypeButtonClicked(bool checked)
{
    m_menu->exec(QPoint(QCursor::pos().x(), QCursor::pos().y()));
}

void ElectricalDeviceType::onGotoClicked(bool checked)
{
    if (m_pCurLineComponent == nullptr) {
        return;
    }
    auto targetDrawingBoard = m_pCurLineComponent->getParentModel();
    if (targetDrawingBoard) {
        PropertyServerMng::getInstance().m_pGraphicsModelingServer->openDrawingBoard(targetDrawingBoard,
                                                                                     m_pCurLineComponent->getUUID());
    } else {
        // 跳转失败，请检查画板或节点是否存在！
        LOGOUT(tr("Goto failed, please check if the drawing board or node exists!"), Kcc::LOG_ERROR);
    }
}

void ElectricalDeviceType::onSelectNodeClicked()
{
    if (m_pElectricalBlock == nullptr) {
        return;
    }

    QString nodeName = QString();
    QString nodeDrawingBoardName = QString();
    auto modelConnectionList = m_pElectricalBlock->getConnectedModel(ModelLineNodeLink);
    if (1 == modelConnectionList.size() && modelConnectionList.first() != nullptr) {
        auto connectionNode = modelConnectionList.first().toStrongRef();
        if (connectionNode) {
            nodeName = connectionNode->getName();
            nodeDrawingBoardName = connectionNode->getParentModelName();
        }
    }

    m_pSelectNodeWidget = new SelectNodeWidget(m_pElectricalBlock->getParentModelName());
    m_pSelectNodeWidget->setInitNodeInfo(nodeDrawingBoardName, nodeName);
    KCustomDialog dlg(SELECT_NODE, m_pSelectNodeWidget, KBaseDlgBox::Ok | KBaseDlgBox::Cancel, KBaseDlgBox::Ok, this);
    dlg.resize(DLGLIST_WIDTH, DLGLIST_HEIGHT);
    if (KBaseDlgBox::Ok == dlg.exec()) {
        auto model = m_pSelectNodeWidget->getSelectLineComponent();
        if (model == nullptr) {
            return;
        }
        m_pCurLineComponent = model.dynamicCast<ElectricalBlock>();
        updateViewData();
    }
    setEnabelSelectNodeStatus();
}

void ElectricalDeviceType::onRemoveNodeClicked()
{
    if (m_pElectricalBlock && m_pCurLineComponent) {
        m_pElectricalBlock->clearConnectedModel(ModelLineNodeLink);
        m_pCurLineComponent = PElectricalBlock(nullptr);
    }
    updateViewData();
    setEnabelSelectNodeStatus();
}

void ElectricalDeviceType::setCWidgetReadOnly(bool bReadOnly)
{
    if (ui.type != nullptr) {
        ui.type->setDisabled(bReadOnly);
    }
    if (ui.typename_2 != nullptr) {
        ui.typename_2->setDisabled(bReadOnly);
    }
    if (ui.typebutton != nullptr) {
        ui.typebutton->setDisabled(bReadOnly);
    }
    if (ui.editButton != nullptr) {
        ui.editButton->setDisabled(bReadOnly);
    }

    if (m_customTableWidget != nullptr) {
        m_customTableWidget->setCWidgetReadOnly(bReadOnly);
    }
}

bool ElectricalDeviceType::checkLegitimacy(QString &errorinfo)
{
    return true;
}

void ElectricalDeviceType::initUI()
{
    if (m_pElectricalBlock == nullptr) {
        return;
    }
    ui.gridLayout->setMargin(0);
    // 是否关联画板信息
    m_oldName = "";
    if (PropertyServerMng::getInstance().m_projectManagerServer != nullptr) {
        //////////////////////跨电网关联新功能临时代码///////////////////////
        QString strPrototypeName = m_pElectricalBlock->getPrototypeName();
        if (strPrototypeName == NPS::PROTOTYPENAME_PI_LINKLINE) {
            strPrototypeName = NPS::PROTOTYPENAME_PI_LINE;
        }
        ////////////////////////////////////////////////////////////////
        m_originalDevicetype =
                PropertyServerMng::getInstance().m_projectManagerServer->GetSystemDeviceModel(strPrototypeName);
    }

    QString boardUUID = m_pElectricalBlock->getParentModelUUID();
    if (!m_pElectricalBlock->getParentModelUUID().isEmpty()) {
        creatTypeMenu();
        if (m_pElectricalBlock->getDeviceModel() != nullptr) {
            m_currentDevicetype = m_pElectricalBlock->getDeviceModel();
            m_oldName = m_currentDevicetype->getName();
        }
    }

    auto modelConnectionList = m_pElectricalBlock->getConnectedModel(ModelLineNodeLink);
    if (1 == modelConnectionList.size()) {
        if (modelConnectionList.first() != nullptr) {
            m_pCurLineComponent = modelConnectionList.first().toStrongRef().dynamicCast<ElectricalBlock>();
        }
    }

    m_customTableWidget = new CustomTableWidget(ui.stackedWidget);
    ui.stackedWidget->addWidget(m_customTableWidget);
    setEnabelSelectNodeStatus();
    updateViewData();
    m_oldList = m_customTableWidget->getNewListData();
    ui.stackedWidget->setCurrentWidget(m_customTableWidget);

    connect(ui.typename_2, SIGNAL(textChanged(const QString &)), this, SLOT(onTypenameChanged(const QString &)));

    connect(ui.editButton, SIGNAL(clicked(bool)), this, SLOT(onEditClicked(bool)));
    connect(m_pActSelectDeviceType, SIGNAL(triggered()), this, SLOT(onSelectDevieTypeClicked()));
    connect(m_pActNewProj, SIGNAL(triggered()), this, SLOT(onNewProjTypeClicked()));
    connect(m_pActRemoveType, SIGNAL(triggered()), this, SLOT(onRemoveTypeClicked()));
    connect(ui.typebutton, SIGNAL(clicked(bool)), this, SLOT(onTypeButtonClicked(bool)));

    connect(m_pActSelectNode, SIGNAL(triggered()), this, SLOT(onSelectNodeClicked()));
    connect(m_pActRemoveNode, SIGNAL(triggered()), this, SLOT(onRemoveNodeClicked()));
    connect(ui.gotoButton, SIGNAL(clicked(bool)), this, SLOT(onGotoClicked(bool)));
}

QString ElectricalDeviceType::getPath(const QString &devicename)
{
    if (devicename.trimmed().isEmpty() || m_pElectricalBlock == nullptr) {
        return QString();
    }
    if (PropertyServerMng::getInstance().m_projectManagerServer == nullptr) {
        return "";
    }

    //////////////////////跨电网关联新功能临时代码///////////////////////
    QString strPrototypeName = m_pElectricalBlock->getPrototypeName();
    if (strPrototypeName == NPS::PROTOTYPENAME_PI_LINKLINE) {
        strPrototypeName = NPS::PROTOTYPENAME_PI_LINE;
    }
    ////////////////////////////////////////////////////////////////

    PDeviceModel sysmodel =
            PropertyServerMng::getInstance().m_projectManagerServer->GetSystemDeviceModel(strPrototypeName);
    if (sysmodel != nullptr && sysmodel->getName() == devicename) {
        return devicename;
    } else {
        QString projectdir = PropertyServerMng::getInstance().m_projectManagerServer->GetCurrentProjectRelativeDir();
        return QString("%1/DeviceModel/%2").arg(projectdir).arg(devicename);
    }
}

QString ElectricalDeviceType::getNodePath(QSharedPointer<Kcc::BlockDefinition::ElectricalBlock> curLineComponent)
{
    if (curLineComponent == nullptr) {
        return QString();
    }

    auto parentModel = curLineComponent->getParentModel();
    if (parentModel == nullptr) {
        return QString();
    }

    QSharedPointer<Kcc::BlockDefinition::DrawingBoardClass> drawingBoard = parentModel.dynamicCast<DrawingBoardClass>();
    if (drawingBoard == nullptr) {
        return QString();
    }

    QString strNodeName = "";
    QList<PModel> modelList = drawingBoard->getAdjacentChild(curLineComponent->getUUID());

    if (modelList.isEmpty()) {
        strNodeName = "";
    } else {
        if (modelList.at(0)) {
            strNodeName = modelList.at(0)->getName();
        }
    }

    QString projectdir = PropertyServerMng::getInstance().m_projectManagerServer->GetCurrentProjectRelativeDir();
    return QString("%1/%2/%3   %4")
            .arg(projectdir)
            .arg(curLineComponent->getParentModelName())
            .arg(curLineComponent->getName())
            .arg(strNodeName);
}

QString ElectricalDeviceType::getNewName()
{
    if (m_currentDevicetype == nullptr) {
        return QString();
    } else {
        return m_currentDevicetype->getName();
    }
}

void ElectricalDeviceType::updateViewData()
{
    if (m_customTableWidget == nullptr) {
        return;
    }

    if (m_currentDevicetype == nullptr) {
        ui.typename_2->setElideText("");
        m_customTableWidget->setListData(QList<CustomModelItem>());
    } else {
        ui.typename_2->setElideText(getPath(m_currentDevicetype->getName()));
        m_customTableWidget->setListData(
                CMA::getPropertyModelItemList(m_currentDevicetype, RoleDataDefinition::DeviceTypeParameter, true));
    }

    if (m_pCurLineComponent == nullptr) {
        ui.nodePathLineText->setElideText("");
    } else {
        ui.nodePathLineText->setElideText(getNodePath(m_pCurLineComponent));
    }
}

void ElectricalDeviceType::openDeviceListDlg(const QString &strtype)
{
    if (m_pElectricalBlock == nullptr) {
        return;
    }

    //////////////////////跨电网关联新功能临时代码///////////////////////
    QString strPrototypeName = m_pElectricalBlock->getPrototypeName();
    if (strPrototypeName == NPS::PROTOTYPENAME_PI_LINKLINE) {
        strPrototypeName = NPS::PROTOTYPENAME_PI_LINE;
    }
    ////////////////////////////////////////////////////////////////

    DeviceTypeListWidget *pDeviceTypeListWidget = new DeviceTypeListWidget;
    pDeviceTypeListWidget->updateDeviceListModelData(
            strtype, PropertyServerMng::getInstance().m_projectManagerServer->GetDeviceModels(strPrototypeName),
            PropertyServerMng::getInstance().m_projectManagerServer->GetSystemDeviceModel(strPrototypeName));
    KCustomDialog dlg(strtype, pDeviceTypeListWidget, KBaseDlgBox::Ok | KBaseDlgBox::Cancel, KBaseDlgBox::Ok, this);
    dlg.resize(DLGLIST_WIDTH, DLGLIST_HEIGHT);
    if (KBaseDlgBox::Ok == dlg.exec()) {
        if (pDeviceTypeListWidget->getSelectDeviceType() == nullptr) {
            return;
        }
        if (pDeviceTypeListWidget->getWidgetType() == SELECT_DEVICE_TYPE) {
            m_currentDevicetype = pDeviceTypeListWidget->getSelectDeviceType();
            updateViewData();
            emit deviceTypeChanged(m_currentDevicetype);
        }
    }

    setEnabelSelectNodeStatus();
}

void ElectricalDeviceType::openDeviceTypeDlg(const QString &title,
                                             QSharedPointer<Kcc::BlockDefinition::DeviceModel> devicemodel,
                                             bool isnewtype)
{
    if (PropertyServerMng::getInstance().propertyServer == nullptr) {
        return;
    }
    if (isnewtype) {

        //////////////////////跨电网关联新功能临时代码///////////////////////
        QString strPrototypeName = m_pElectricalBlock->getPrototypeName();
        if (strPrototypeName == NPS::PROTOTYPENAME_PI_LINKLINE) {
            strPrototypeName = NPS::PROTOTYPENAME_PI_LINE;
        }
        ////////////////////////////////////////////////////////////////

        PDeviceModel pdevmodel = PropertyServerMng::getInstance().propertyServer->CreateDeviceModel(strPrototypeName);
        if (pdevmodel != nullptr) {
            m_currentDevicetype = pdevmodel;
            updateViewData();
            emit deviceTypeChanged(m_currentDevicetype);
        }
    } else if (m_currentDevicetype != nullptr
               && PropertyServerMng::getInstance().propertyServer->EditDeviceModel(m_currentDevicetype)) {
        updateViewData();
        emit deviceTypeChanged(m_currentDevicetype);
    }
}

void ElectricalDeviceType::openWarningDlg(const QString &tips)
{
    KMessageBox::warning(tips, KMessageBox::Ok | KMessageBox::Cancel, KMessageBox::Ok);
}

void ElectricalDeviceType::creatTypeMenu()
{
    QIcon icon = QApplication::style()->standardIcon(QStyle::SP_TitleBarUnshadeButton);
    QPixmap pixmap = icon.pixmap(QSize(20, 20));

    m_menu = new QMenu(this);
    m_pActSelectDeviceType = new QAction(SELECT_DEVICE_TYPE, this);
    m_pActNewProj = new QAction(NEW_DEVICE_TYPE, this);
    m_pActRemoveType = new QAction(REMOVE_DEVICE_TYPE, this);

    m_menu->addAction(m_pActSelectDeviceType);
    m_menu->addSeparator();
    m_menu->addAction(m_pActNewProj);
    m_menu->addSeparator();
    m_menu->addAction(m_pActRemoveType);
    ui.typebutton->setMenu(m_menu);
    ui.typebutton->setIcon(QIcon(pixmap.scaled(QSize(100, 100))));
    ui.typebutton->setStyleSheet("QPushButton::menu-indicator{image:none;}");

    m_menuNode = new QMenu(this);
    m_pActSelectNode = new QAction(SELECT_NODE, this);
    m_pActRemoveNode = new QAction(REMOVE_NODE, this);
    m_menuNode->addAction(m_pActSelectNode);
    m_menuNode->addSeparator();
    m_menuNode->addAction(m_pActRemoveNode);
    ui.nodeButton->setMenu(m_menuNode);
    ui.nodeButton->setIcon(QIcon(pixmap.scaled(QSize(100, 100))));
    ui.nodeButton->setStyleSheet("QPushButton::menu-indicator{image:none;}");

    QString strPrototypeName = m_pElectricalBlock->getPrototypeName();
    if (strPrototypeName == "PiTypeTransmissionLinkLine") {
        ui.nodeLabel->setHidden(false);
        ui.nodeButton->setHidden(false);
        ui.nodePathLineText->setHidden(false);
        ui.gotoButton->setHidden(false);
    } else {
        ui.nodeLabel->setHidden(true);
        ui.nodeButton->setHidden(true);
        ui.nodePathLineText->setHidden(true);
        ui.gotoButton->setHidden(true);
    }
}

void ElectricalDeviceType::onSyncLineComponentInfo()
{
    if (m_pElectricalBlock == nullptr || m_pCurLineComponent == nullptr) {
        return;
    }

    // 同步设备类型
    auto currentDevicetype = m_pElectricalBlock->getDeviceModel();
    if (currentDevicetype != nullptr) {
        m_pCurLineComponent->setDeviceModel(currentDevicetype);
    }

    // 同步参数列表
    auto variableList = m_pElectricalBlock->getVariableGroupList();
    foreach (auto var, variableList) {
        if (var) {
            QString groupType = var->getGroupType();
            if (groupType == Kcc::BlockDefinition::RoleDataDefinition::PortGroup) {
                continue;
            }
            auto dstVar = m_pCurLineComponent->getVariableGroup(var->getGroupType());
            if (dstVar) {
                dstVar->copyFromGroup(var);
            }
        }
    }
}

void ElectricalDeviceType::setEnabelSelectNodeStatus()
{
    if (m_currentDevicetype) {
        ui.nodeButton->setEnabled(true);
    } else {
        ui.nodeButton->setEnabled(false);
    }

    if (m_pCurLineComponent) {
        ui.gotoButton->setEnabled(true);
    } else {
        ui.gotoButton->setEnabled(false);
    }
}

// list device type
DeviceTypeListWidget::DeviceTypeListWidget(QWidget *parent /*= nullptr*/)
    : QWidget(parent),
      m_widgetType(""),
      m_model(nullptr),
      m_sortFilterProxyModel(nullptr),
      m_tableview(nullptr),
      m_comboBox(nullptr),
      m_LineEdit(nullptr),
      m_tableSelectedType(nullptr),
      m_bFirstInit(true)
{
    this->resize(DLGLIST_WIDTH, DLGLIST_HEIGHT);
    QGridLayout *gridLayout = new QGridLayout(this);
    gridLayout->setContentsMargins(10, 10, 10, 0);
    // 追加筛选功能
    QHBoxLayout *horizontalLayout = new QHBoxLayout(this);
    horizontalLayout->setMargin(0);
    QLabel *label = new QLabel(this);
    label->setText(tr("Filter Columns")); // 筛选列
    QSizePolicy sizePolicy2(QSizePolicy::Fixed, QSizePolicy::Preferred);
    sizePolicy2.setHorizontalStretch(0);
    sizePolicy2.setVerticalStretch(0);
    sizePolicy2.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
    label->setSizePolicy(sizePolicy2);
    // label->setMinimumSize(QSize(100, 0));
    // label->setMaximumSize(QSize(100, 16777215));

    horizontalLayout->addWidget(label);

    m_comboBox = new QComboBox(this);
    m_comboBox->setView(new QListView());
    QSizePolicy sizePolicy3(QSizePolicy::Preferred, QSizePolicy::Fixed);
    sizePolicy3.setHorizontalStretch(1);
    sizePolicy3.setVerticalStretch(0);
    sizePolicy3.setHeightForWidth(m_comboBox->sizePolicy().hasHeightForWidth());
    m_comboBox->setSizePolicy(sizePolicy3);

    horizontalLayout->addWidget(m_comboBox);

    QLabel *label_2 = new QLabel(this);
    label_2->setText(tr("Filter Content")); // 筛选内容
    sizePolicy2.setHeightForWidth(label_2->sizePolicy().hasHeightForWidth());
    label_2->setSizePolicy(sizePolicy2);
    // label_2->setMinimumSize(QSize(100, 0));
    // label_2->setMaximumSize(QSize(100, 16777215));

    horizontalLayout->addWidget(label_2);

    m_LineEdit = new KLineEdit(this);
    m_LineEdit->setMinimumWidth(200);
    QSizePolicy sizePolicyL(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    sizePolicyL.setHorizontalStretch(1);
    sizePolicyL.setVerticalStretch(0);
    sizePolicyL.setHeightForWidth(m_LineEdit->sizePolicy().hasHeightForWidth());
    m_LineEdit->setSizePolicy(sizePolicyL);
    horizontalLayout->addWidget(m_LineEdit, 1);

    gridLayout->addLayout(horizontalLayout, 0, 0, 1, 1);

    // 表格功能
    m_tableview = new TableView(this);
    m_tableview->horizontalHeader()->setStretchLastSection(true);
    m_tableview->horizontalHeader()->setHighlightSections(false);
    // m_tableview->verticalHeader()->setFixedWidth(36);
    m_tableview->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableview->setSortingEnabled(true);
    /*tableView->setColumnWidth(0,TABLE_ITEM_COLUMN);
    tableView->setColumnWidth(1,TABLE_ITEM_COLUMN);*/

    gridLayout->addWidget(m_tableview, 1, 0, 1, 1);
    this->setLayout(gridLayout);

    m_model = new DeviceTypeModel(m_tableview);
    m_sortFilterProxyModel = new CustomSortFilterProxyModel(m_tableview);

    connect(m_comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onSearchColBoxChanged(int)));
    connect(m_LineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(onSearchTextChanged(const QString &)));
    connect(m_tableview, SIGNAL(clicked(QModelIndex)), this, SLOT(onTableViewClicked(QModelIndex)));
}

DeviceTypeListWidget::~DeviceTypeListWidget() { }

void DeviceTypeListWidget::updateDeviceListModelData(
        const QString &widgettype, const QList<QSharedPointer<Kcc::BlockDefinition::DeviceModel>> &devicelist,
        QSharedPointer<Kcc::BlockDefinition::DeviceModel> sysdevice)
{
    if (m_model == nullptr || m_sortFilterProxyModel == nullptr) {
        return;
    }
    m_widgetType = widgettype;
    m_tableSelectedType = PDeviceModel(nullptr);
    m_model->updateDeviceData(devicelist, sysdevice);
    m_sortFilterProxyModel->setSourceModel(m_model);
    m_tableview->setModel(m_sortFilterProxyModel);
    updateFilterContents();
    setTableColumFixedWidth(TableCol_Width);
}

void DeviceTypeListWidget::onSearchColBoxChanged(int col)
{
    if (m_sortFilterProxyModel != nullptr) {
        m_sortFilterProxyModel->setFilterColumn(col);
    }
}

void DeviceTypeListWidget::onSearchTextChanged(const QString &strtext)
{
    if (m_sortFilterProxyModel != nullptr) {
        m_sortFilterProxyModel->setFilterString(strtext);
    }
}

void DeviceTypeListWidget::onTableViewClicked(const QModelIndex &index)
{
    if (!index.isValid()) {
        m_tableSelectedType = PDeviceModel(nullptr);
    }
    if (m_sortFilterProxyModel == nullptr || m_model == nullptr) {
        return;
    }
    QModelIndex sourceindex = m_sortFilterProxyModel->mapToSource(index);
    m_tableSelectedType = m_model->getDeviceModel(sourceindex);
}

void DeviceTypeListWidget::updateFilterContents()
{
    if (m_comboBox == nullptr || m_LineEdit == nullptr || m_tableview == nullptr) {
        return;
    }
    if (m_tableview->model() == nullptr) {
        m_comboBox->clear();
        m_LineEdit->setText("");
        return;
    }
    QStringList listcolumnbox = QStringList();
    int columncount = m_tableview->model()->columnCount();
    for (int i = 0; i < columncount; ++i) {
        listcolumnbox << m_tableview->model()->headerData(i, Qt::Horizontal).toString().replace("\n", "");
    }

    m_comboBox->clear();
    m_comboBox->addItems(listcolumnbox);
    for (int i = 0; i < listcolumnbox.size(); ++i) {
        m_comboBox->setItemData(i, listcolumnbox[i], Qt::ToolTipRole);
    }

    m_LineEdit->setText("");
}

void DeviceTypeListWidget::setTableColumFixedWidth(const int &width)
{
    if (m_tableview->model() == nullptr || m_tableview->horizontalHeader() == nullptr) {
        return;
    }
    int viewwidth = m_tableview->viewport()->width();
    if (m_bFirstInit) {
        viewwidth = 773;
        m_bFirstInit = false;
    }
    int totalcol = m_tableview->model()->columnCount();
    if (totalcol > 0 && totalcol * width < viewwidth) {
        int colwidth = viewwidth / totalcol;
        for (int i = 0; i < totalcol; ++i) {
            m_tableview->setColumnWidth(i, colwidth);
        }
        return;
    }
    for (int i = 0; i < totalcol; ++i) {
        m_tableview->setColumnWidth(i, width);
    }
}

SelectNodeWidget::SelectNodeWidget(const QString &curDrawingBoardName, QWidget *parent /*= nullptr*/)
    : QWidget(parent),
      m_widgetType(""),
      m_pSortFilterProxyModel(nullptr),
      m_pTableWidget(nullptr),
      m_pComboBox(nullptr),
      m_pLineEdit(nullptr),
      m_bFirstInit(true),
      m_pTreeModel(nullptr),
      m_pNodeLineModel(nullptr)
{
    initUI();
    initData(curDrawingBoardName);
}

SelectNodeWidget::~SelectNodeWidget() { }

void SelectNodeWidget::setInitNodeInfo(const QString &nodeDrawingBoardName, const QString &nodeName)
{
    if (nodeDrawingBoardName.isEmpty() && nodeName.isEmpty()) {
        return;
    }

    // 遍历树状模型中的所有项
    for (int row = 0; row < m_pTreeModel->rowCount(); ++row) {
        QStandardItem *item = m_pTreeModel->item(row);
        if (item && item->text() == nodeDrawingBoardName) {
            // 找到匹配的项，将其设置为当前项
            m_pTreeView->setCurrentIndex(m_pTreeModel->indexFromItem(item));
            m_pTreeView->clicked(m_pTreeModel->indexFromItem(item));
            break;
        }
    }

    // 遍历表格中的项，查找匹配的项
    for (int row = 0; row < m_pTableWidget->rowCount(); ++row) {
        for (int column = 0; column < m_pTableWidget->columnCount(); ++column) {
            QTableWidgetItem *item = m_pTableWidget->item(row, column);
            if (item && item->text() == nodeName) {
                // 设置当前项为找到的项
                m_pTableWidget->setCurrentItem(item);
                break;
            }
        }
    }
}

void SelectNodeWidget::initData(const QString &curDrawingBoardName)
{
    if (m_pTreeModel == nullptr || PropertyServerMng::getInstance().m_projectManagerServer == nullptr) {
        return;
    }

    m_pTreeModel->clear();

    QList<PModel> alldrawboards = PropertyServerMng::getInstance().m_projectManagerServer->GetAllBoardModel();
    if (alldrawboards.size() <= 0) {
        return;
    }

    foreach (PModel pDrawboard, alldrawboards) {
        if (pDrawboard == nullptr || pDrawboard->getName() == curDrawingBoardName) {
            continue;
        }

        if (pDrawboard->getModelType() == ElecBoardModel::Type) {
            initBoardData(pDrawboard);
        }
    }
    m_pTreeModel->sort(0);
}

void SelectNodeWidget::initBoardData(QSharedPointer<Kcc::BlockDefinition::Model> drawboard)
{
    PDrawingBoardClass pDrawboard = drawboard.dynamicCast<DrawingBoardClass>();
    if (pDrawboard == nullptr) {
        return;
    }
    m_lineComponentsMap.clear();
    QStandardItem *boarditem = new QStandardItem();
    boarditem->setText(pDrawboard->getName());
    m_pTreeModel->appendRow(boarditem);

    foreach (auto pBlock, pDrawboard->getChildModels().values()) {
        if (pBlock->getPrototypeName() == "PiTypeTransmissionLinkLine") {
            m_lineComponentsMap.insert(pBlock->getName(), pBlock);
        }
    }

    if (!m_lineComponentsMap.isEmpty()) {
        m_nodeInfoMap.insert(pDrawboard->getName(), m_lineComponentsMap);
    }
}

void SelectNodeWidget::initUI()
{
    this->resize(DLGLIST_WIDTH, DLGLIST_HEIGHT);
    QGridLayout *gridLayout = new QGridLayout(this);
    gridLayout->setContentsMargins(4, 10, 10, 10);

    m_pTreeView = new TreeView(this);
    m_pTreeView->setIndentation(1);
    m_pTreeView->setMaximumWidth(245);
    m_pTreeView->setHeaderHidden(true);
    m_pTreeModel = new QStandardItemModel(m_pTreeView);
    m_pTreeView->setModel(m_pTreeModel);
    gridLayout->addWidget(m_pTreeView, 0, 0, 10, 1);

    /*// TODO 追加筛选功能
    QHBoxLayout *horizontalLayout = new QHBoxLayout(this);
    horizontalLayout->setMargin(0);
    QLabel *label = new QLabel(this);
    label->setText(tr("Filter Columns")); // 筛选列
    QSizePolicy sizePolicy2(QSizePolicy::Fixed, QSizePolicy::Preferred);
    sizePolicy2.setHorizontalStretch(0);
    sizePolicy2.setVerticalStretch(0);
    sizePolicy2.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
    label->setSizePolicy(sizePolicy2);
    // label->setMinimumSize(QSize(100, 0));
    // label->setMaximumSize(QSize(100, 16777215));

    horizontalLayout->addWidget(label);

    m_pComboBox = new QComboBox(this);
    m_pComboBox->setView(new QListView());
    QSizePolicy sizePolicy3(QSizePolicy::Preferred, QSizePolicy::Fixed);
    sizePolicy3.setHorizontalStretch(1);
    sizePolicy3.setVerticalStretch(0);
    sizePolicy3.setHeightForWidth(m_pComboBox->sizePolicy().hasHeightForWidth());
    m_pComboBox->setSizePolicy(sizePolicy3);

    horizontalLayout->addWidget(m_pComboBox);

    QLabel *label_2 = new QLabel(this);
    label_2->setText(tr("Filter Content")); // 筛选内容
    sizePolicy2.setHeightForWidth(label_2->sizePolicy().hasHeightForWidth());
    label_2->setSizePolicy(sizePolicy2);
    // label_2->setMinimumSize(QSize(100, 0));
    // label_2->setMaximumSize(QSize(100, 16777215));

    horizontalLayout->addWidget(label_2);

    m_pLineEdit = new KLineEdit(this);
    m_pLineEdit->setMinimumWidth(200);
    QSizePolicy sizePolicyL(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    sizePolicyL.setHorizontalStretch(1);
    sizePolicyL.setVerticalStretch(0);
    sizePolicyL.setHeightForWidth(m_pLineEdit->sizePolicy().hasHeightForWidth());
    m_pLineEdit->setSizePolicy(sizePolicyL);
    horizontalLayout->addWidget(m_pLineEdit, 1);

    gridLayout->addLayout(horizontalLayout, 0, 1, 1, 1);
*/
    // 表格功能
    m_pTableWidget = new QTableWidget(this);
    m_pTableWidget->setColumnCount(2);
    m_pTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_pTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_pTableWidget->setHorizontalHeaderLabels(QStringList()
                                              << CMA::HEADER_LINE_COMPONENT_NAME << CMA::HEADER_GRID_NODE_NAME);
    m_pTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_pTableWidget->verticalHeader()->setVisible(false);
    m_pTableWidget->verticalHeader()->setDefaultSectionSize(26);
    gridLayout->addWidget(m_pTableWidget, 0, 1, 10, 2);
    this->setLayout(gridLayout);

    m_pSortFilterProxyModel = new CustomSortFilterProxyModel(m_pTableWidget);

    connect(m_pComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onSearchColBoxChanged(int)));
    connect(m_pLineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(onSearchTextChanged(const QString &)));
    connect(m_pTableWidget, &QTableWidget::cellClicked, this, &SelectNodeWidget::onTableViewClicked);
    connect(m_pTreeView, &QTreeView::clicked, this, &SelectNodeWidget::onClickedTreeItem);
}

void SelectNodeWidget::onClickedTreeItem(const QModelIndex &index)
{
    if (!index.isValid()) {
        return;
    }
    QString strBoardName = index.data().toString();
    if (!strBoardName.isEmpty()) {
        int nRow = 0;
        auto compenentInfoMap = m_nodeInfoMap[strBoardName];
        for (auto model : compenentInfoMap) {
            if (!model) {
                continue;
            }
            auto parentModel = model->getParentModel();
            if (parentModel == nullptr) {
                continue;
            }

            QSharedPointer<Kcc::BlockDefinition::DrawingBoardClass> drawingBoard =
                    parentModel.dynamicCast<DrawingBoardClass>();
            if (drawingBoard == nullptr) {
                continue;
            }

            QList<PModel> modelList = drawingBoard->getAdjacentChild(model->getUUID());
            m_pTableWidget->setRowCount(nRow + 1);
            QTableWidgetItem *blockName = new QTableWidgetItem(model->getName());
            m_pTableWidget->setItem(nRow, 0, blockName);
            if (!modelList.isEmpty() && 1 == modelList.size() && modelList.at(0) != nullptr) {
                QTableWidgetItem *nodeName = new QTableWidgetItem(modelList.at(0)->getName());
                m_pTableWidget->setItem(nRow, 1, nodeName);
            }
            nRow++;
        }
    }
}

void SelectNodeWidget::onTableViewClicked(const int &row, const int &col)
{
    // 获取对应行第一列的QTableWidgetItem对象
    QTableWidgetItem *itemLine = m_pTableWidget->item(row, 0);

    if (itemLine == nullptr) {
        return;
    }

    // 获取单元格中的文本
    QString text = itemLine->text();
    if (!text.isEmpty()) {
        m_pNodeLineModel = m_lineComponentsMap.value(text);
    }
}
