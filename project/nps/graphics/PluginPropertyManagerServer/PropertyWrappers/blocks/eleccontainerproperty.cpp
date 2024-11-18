#include "eleccontainerproperty.h"

#include "Associate.h"
#include "KLModelDefinitionCore/KLModelDefinitionCore.h"
#include "PropertyServerMng.h"
#include "blockpropertycontrol.h"
#include <QApplication>
#include <QDebug>
#include <QHeaderView>
#include <QPushButton>
#include <QSpacerItem>

// 日志
#include "CoreLib/ServerManager.h"

using namespace Kcc::BlockDefinition;
USE_LOGOUT_("ElecContainerProperty")

static const QString BUSBAR_PROTOTYPE_NAME = "Busbar";
static const QString SM_PROTOTYPE_NAME = "SynchronousMachine";
static const int HORIZONTAL_HEADER_HEIGHT = 26;
static const int FOUR_CHINESE_LENGTHS = 64;
static const int TABLEWIDGET_ITEM_WITH_1 = 150;
static const int TABLEWIDGET_ITEM_WITH_2 = 160;

ElecContainerProperty::ElecContainerProperty(QSharedPointer<Kcc::BlockDefinition::Model> model, bool isReadOnly)
    : BlockProperty(model, isReadOnly),
      m_typeWidget(nullptr),
      m_pInPortTableWidget(nullptr),
      m_pOutPortTableWidget(nullptr),
      m_strAssociateType(QString())
{
    Q_ASSERT(m_pModel != nullptr);
    m_pElecContainerBlock = m_pModel.dynamicCast<ElectricalContainerBlock>();
}

ElecContainerProperty::~ElecContainerProperty()
{
    if (m_typeWidget != nullptr) {
        delete m_typeWidget;
    }

    if (m_pInPortTableWidget != nullptr) {
        delete m_pInPortTableWidget;
    }

    if (m_pOutPortTableWidget != nullptr) {
        delete m_pOutPortTableWidget;
    }
}

void ElecContainerProperty::init()
{
    BlockProperty::init();
    PModel elecModel;
    m_typeWidget = new ElecTypeWidget(m_pModel);
    connect(m_typeWidget, &ElecTypeWidget::blockChanged, this, &ElecContainerProperty::refreshChoosePortsWidget);
    connect(m_typeWidget, &ElecTypeWidget::typeChanged, this, &ElecContainerProperty::refreshPropertyWidgetPix);
    addWidget(m_typeWidget, CMA::BLOCK_TAB_NAME_ELEC_CONTAINER);
    QList<PAssociate> associate = m_pModel->getAllAssociate();
    for (PAssociate as : associate) {
        if (as) {
            m_strAssociateType = as->type;
        }
    }
    QList<QWeakPointer<Model>> elecModelList = m_pModel->getConnectedModel(m_strAssociateType);
    if (!elecModelList.isEmpty()) {
        elecModel = elecModelList.at(0);
    } else {
        elecModel = m_typeWidget->getElecBlock();
    }

    m_pInPortTableWidget = new ElecContainerPortTableWidget(CMA::BLOCK_TAB_NAME_SELECT_INPUT, m_pModel, elecModel);
    m_pOutPortTableWidget = new ElecContainerPortTableWidget(CMA::BLOCK_TAB_NAME_SELECT_OUTPUT, m_pModel, elecModel);
    addWidget(m_pInPortTableWidget, CMA::BLOCK_TAB_NAME_SELECT_INPUT);
    addWidget(m_pOutPortTableWidget, CMA::BLOCK_TAB_NAME_SELECT_OUTPUT);
    setPropertyEditableStatus(m_readOnly);
    setPortTableWidgetDisable();
}

void ElecContainerProperty::refreshProperty() { }

ElecTypeWidget *ElecContainerProperty::getTypeWidget()
{
    return m_typeWidget;
}

QPixmap ElecContainerProperty::getBlockPixmap()
{
    if (m_pElecContainerBlock == nullptr) {
        return QPixmap();
    }

    if (!m_variant.isEmpty()) {
        return m_variant.value(Kcc::BlockDefinition::PNG_NORMAL_PIC).value<QPixmap>();
    }

    if (m_pElecContainerBlock->getElectricalBlock() == nullptr) {
        m_variant = m_pElecContainerBlock->getResource();
        return m_variant.value(Kcc::BlockDefinition::PNG_NORMAL_PIC).value<QPixmap>();
    }
    m_variant = m_pElecContainerBlock->getElectricalBlock()->getResource();
    return m_variant.value(Kcc::BlockDefinition::PNG_NORMAL_PIC).value<QPixmap>();
}

void ElecContainerProperty::onDialogExecuteResult(QDialog::DialogCode code)
{
    if (code == QDialog::Accepted) {
        if (!m_pElecContainerBlock) {
            return;
        }

        BlockProperty::onDialogExecuteResult(code);
        setElecInterfaceBlockVariables(CMA::BLOCK_TAB_NAME_SELECT_INPUT);
        setElecInterfaceBlockVariables(CMA::BLOCK_TAB_NAME_SELECT_OUTPUT);

        if (m_typeWidget != nullptr) {
            savePicToSlot(m_typeWidget->getElecBlock());
            if (!m_pElecContainerBlock->parseElecInterfaceBlock(m_typeWidget->getElecBlock())) {
                LOGOUT(tr("'1%' module error, please check the module!").arg(m_pElecContainerBlock->getName()),
                       LOG_ERROR); // '1%'模块出错，请检查该模块！
            }

            QString oldName = m_pModel->getName();
            if (oldName != m_typeWidget->getElecBlockName()) {
                m_bPropertyIsChanged = true;
                m_pModel->setName(m_typeWidget->getElecBlockName());
            }
        }

        if (m_bPropertyIsChanged) {
            m_pElecContainerBlock->setModifyTime(QDateTime::currentDateTime());
            CMA::saveModel(m_pElecContainerBlock);
        }
    }
}

bool ElecContainerProperty::checkValue(QString &errorinfo)
{
    errorinfo = "";
    m_typeWidget->setElecBlockNewInputCount(
            m_pInPortTableWidget->getCheckedVaruablesMap(CMA::BLOCK_TAB_NAME_SELECT_INPUT).size());
    if (m_typeWidget != nullptr && !m_typeWidget->checkLegitimacy(errorinfo)) {
        return false;
    }

    return true;
}

QString ElecContainerProperty::getHelpUrl()
{
    return m_pElecContainerBlock->getPrototypeName();
}

void ElecContainerProperty::refreshChoosePortsWidget(QSharedPointer<Kcc::BlockDefinition::ElectricalBlock> EleBlock)
{
    m_pInPortTableWidget->displayBlockPortsInfo(EleBlock);
    m_pOutPortTableWidget->displayBlockPortsInfo(EleBlock);

    setPortTableWidgetDisable();
}

void ElecContainerProperty::setPortTableWidgetDisable()
{
    bool isBoardCreated = false;
    if (PropertyServerMng::getInstance().m_projectManagerServer) {
        QString strInterfaceBlockBoardUUID = m_pElecContainerBlock->getParentModelUUID();
        QList<PModel> drawingBoardList = PropertyServerMng::getInstance().m_projectManagerServer->GetAllBoardModel();
        for each (PModel drawingBoardModel in drawingBoardList) {
            PDrawingBoardClass drawingBoard = drawingBoardModel.dynamicCast<DrawingBoardClass>();
            if (drawingBoard->getUUID() == strInterfaceBlockBoardUUID) {
                isBoardCreated = true;
                if ((drawingBoard->isInstance() && drawingBoard->getModelType() == ComplexBoardModel::Type)) {
                    m_pInPortTableWidget->setColumnAllSelected(0);
                    m_pOutPortTableWidget->setColumnAllSelected(0);
                }
            }
        }
        if (!isBoardCreated) {
            m_pInPortTableWidget->setColumnAllSelected(0);
            m_pOutPortTableWidget->setColumnAllSelected(0);
        }
    }
}

bool ElecContainerProperty::setElecInterfaceBlockVariables(const QString &varType)
{
    bool isChanged = false;
    PVariableGroup varGroup;
    QMap<QString, QString> oldIOVariableKeys;
    QMap<QString, QString> newIOVariableKeys;
    QMap<QString, PVariable> variablesMap;
    if (!m_typeWidget->getElecBlock()) {
        return false;
    }
    auto inputparamMap = PropertyServerMng::getInstance().m_pIElecSysServer->GetInputParamMap(
            m_typeWidget->getElecBlock()->getPrototypeName());
    auto outputparamMap = PropertyServerMng::getInstance().m_pIElecSysServer->GetOutputParamMap(
            m_typeWidget->getElecBlock()->getPrototypeName(), m_typeWidget->getElecBlock()->getPhaseNumber());

    if (!m_typeWidget) {
        return false;
    }
    auto elecBlockNew = m_typeWidget->getElecBlock();
    auto list = m_pElecContainerBlock->getConnectedModel(m_strAssociateType);
    if (!list.isEmpty()) {
        auto elecBlockOld = list.at(0);
        if (elecBlockNew != elecBlockOld) {
            isChanged = true;
        }
    }

    if (varType == CMA::BLOCK_TAB_NAME_SELECT_INPUT) {
        varGroup = m_pElecContainerBlock->getVariableGroup(RoleDataDefinition::InputSignal);
        if (!varGroup) {
            varGroup = m_pElecContainerBlock->createVariableGroup(RoleDataDefinition::InputSignal);
        } else {
            auto inputVarList = varGroup->getVariableMap().values();
            std::sort(inputVarList.begin(), inputVarList.end(),
                      [](const PVariable &p1, const PVariable &p2) -> bool { return p1->getOrder() < p2->getOrder(); });

            for (auto var : inputVarList) {
                oldIOVariableKeys[var->getName()] = inputparamMap[var->getName()].dataType;
            }
        }

        variablesMap = m_pInPortTableWidget->getCheckedVaruablesMap(varType);
        for (auto key : variablesMap.keys()) {
            newIOVariableKeys[key] = inputparamMap[key].dataType;
        }
        m_pElecContainerBlock->updataIOSignalVariables(varGroup, oldIOVariableKeys, newIOVariableKeys);
    } else if (varType == CMA::BLOCK_TAB_NAME_SELECT_OUTPUT) {
        varGroup = m_pElecContainerBlock->getVariableGroup(RoleDataDefinition::OutputSignal);
        if (!varGroup) {
            varGroup = m_pElecContainerBlock->createVariableGroup(RoleDataDefinition::OutputSignal);
        } else {
            auto outputVarList = varGroup->getVariableMap().values();
            std::sort(outputVarList.begin(), outputVarList.end(),
                      [](const PVariable &p1, const PVariable &p2) -> bool { return p1->getOrder() < p2->getOrder(); });

            for (auto var : outputVarList) {
                oldIOVariableKeys[var->getName()] = outputparamMap[var->getName()].dataType;
            }
        }

        variablesMap = m_pOutPortTableWidget->getCheckedVaruablesMap(varType);
        for (auto key : variablesMap.keys()) {
            newIOVariableKeys[key] = outputparamMap[key].dataType;
        }
        m_pElecContainerBlock->updataIOSignalVariables(varGroup, oldIOVariableKeys, newIOVariableKeys);
    }

    if (oldIOVariableKeys != newIOVariableKeys) {
        m_bPropertyIsChanged = true;
    }
    return isChanged;
}

bool ElecContainerProperty::savePicToSlot(QSharedPointer<Kcc::BlockDefinition::Model> model)
{
    if (m_pModel == nullptr) {
        return false;
    }
    if (model == nullptr || model->getResource().isEmpty()) {
        // 使用插槽的图
        model = CMA::getModelByPrototype(m_pModel->getPrototypeName());
        if (model == nullptr) {
            return false;
        }
    }
    m_pModel->setResource(model->getResource());
    return true;
}

void ElecContainerProperty::refreshPropertyWidgetPix(QString propertyName)
{
    PModel model = CMA::getModelByPrototype(propertyName);
    if (!model) {
        return;
    }
    m_variant = model->getResource();
    emit viewChanged();
}

//////////////////////////////////////////////////////////
// 自定义widget
ElecTypeWidget::ElecTypeWidget(QSharedPointer<Kcc::BlockDefinition::Model> eleInterfaceModel,
                               QWidget *parent /*= nullptr*/)
    : CWidget(parent), m_pElecContainerBlock(eleInterfaceModel), m_isInitProcess(true), m_isMesureType(false)
{
    Q_ASSERT(eleInterfaceModel);
    initUI();

    QList<PAssociate> associate = eleInterfaceModel->getAllAssociate();
    for (PAssociate as : associate) {
        if (as) {
            m_strAssociateType = as->type;
        }
    }
}

QString ElecTypeWidget::getElecBlockName() const
{
    return m_lineEdit->text();
}

void ElecTypeWidget::setElecBlockNewInputCount(const int count)
{
    if (0 == count) {
        m_isMesureType = true;
    } else {
        m_isMesureType = false;
    }
}

bool ElecTypeWidget::checkLegitimacy(QString &errorinfo)
{
    errorinfo = "";
    if (m_lineEdit != nullptr && hasBlockName(m_lineEdit->text())) {
        // 该名称在当前画板中已经存在，请重新选择类型和命名！
        errorinfo = tr("The name already exists in the current drawing board. Please reselect the type or name!");
        return false;
    }

    auto selectedElecBlock = getElecBlock();
    if (selectedElecBlock == nullptr) {
        return true;
    }

    if (m_isMesureType) {
        return true;
    }

    PModel targetConnection;
    auto modelConnectionList = selectedElecBlock->getConnectedModel(ModelConnElectricInterface);
    targetConnection = checkBlockIsConnected(modelConnectionList);
    if (!targetConnection) {
        auto modelConnectionSlotList = selectedElecBlock->getConnectedModel(ModelConnSlot);
        targetConnection = checkBlockIsConnected(modelConnectionSlotList);
    }

    if (targetConnection) {
        QString strQuestionInfo;
        if (targetConnection->getModelType() == Model::EleContainer_Block_Type) {
            // 当前所选电气元件已被电气接口模块"%1"连接，是否取消之前连接？
            strQuestionInfo = tr("The currently selected electrical component has been connected by the electrical"
                                 "interface module '%1'. Do you want to cancel the previous connection?")
                                      .arg(targetConnection->getName());
        } else if (targetConnection->getModelType() == Model::Slot_Block_Type) {
            // 当前所选电气元件已被插槽模块"%1"连接，是否取消之前连接？
            strQuestionInfo = tr("The currently selected electrical component has been connected by slot module '%1'. "
                                 "Do you want to cancel the previous connection?")
                                      .arg(targetConnection->getName());
        }
        auto button =
                KMessageBox::question(strQuestionInfo, KMessageBox::Ok | KMessageBox::Cancel, KMessageBox::Cancel);
        if (button == KMessageBox::Ok) {
            clearModelInfo(targetConnection);
            return true;
        } else {
            return false;
        }
    }
    return true;
}

QSharedPointer<Kcc::BlockDefinition::ElectricalBlock> ElecTypeWidget::getElecBlock() const
{
    return m_pElectricalBlock;
}

void ElecTypeWidget::onDrawBorderNameChanged(QString drawBorderName)
{
    QStringList strElecTypesList;
    m_strDrawBorderName = drawBorderName;

    if (!m_strDrawBorderName.isEmpty()) {
        QMap<QString, QString> allBlockNameMap = m_allBlockMap[m_strDrawBorderName];
        QMapIterator<QString, QString> iter(allBlockNameMap);
        while (iter.hasNext()) {
            iter.next();
            strElecTypesList.append(iter.key());
        }
        qSort(strElecTypesList.begin(), strElecTypesList.end());
        m_pElecTypeComboBoxTemp->clear();
        m_pElecTypeComboBoxTemp->addItems(strElecTypesList);

        if (1 == m_pElecContainerBlock->getConnectedModel().size()) {
            for (int nIndex = 0; nIndex < strElecTypesList.count(); nIndex++) {
                QString strElementPrototypeName;
                QString strElementName = m_pElecTypeComboBoxTemp->itemText(nIndex);
                strElementPrototypeName = m_allBlockMap.value(m_strDrawBorderName).value(strElementName);

                QString strPrototypeName = m_pElecContainerBlock->getConnectedModel().at(0).data()->getPrototypeName();
                if (strPrototypeName == NPS::PROTOTYPENAME_DOTBUSBAR) {
                    strPrototypeName = NPS::PROTOTYPENAME_BUSBAR;
                }

                if (strElementPrototypeName == strPrototypeName) {
                    m_pElecTypeComboBoxTemp->setCurrentIndex(nIndex);
                    onElecTypeChanged(m_pElecTypeComboBoxTemp->itemText(nIndex));
                    return;
                }
            }
        }
        m_pElecTypeComboBoxTemp->setCurrentIndex(0);
        onElecTypeChanged(m_pElecTypeComboBoxTemp->itemText(0));
    }
}

void ElecTypeWidget::onDrawBorderNameClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column);
    QString strItemName = m_pElecDrawBoardTreeWidget->getCurrentItemPathText(*item);
    QStringList strElecTypesList;
    m_strDrawBorderName = strItemName;

    if (!m_strDrawBorderName.isEmpty()) {
        QMap<QString, QString> allBlockNameMap = m_allBlockMap[m_strDrawBorderName];
        QMapIterator<QString, QString> iter(allBlockNameMap);
        while (iter.hasNext()) {
            iter.next();
            strElecTypesList.append(iter.key());
        }
        qSort(strElecTypesList.begin(), strElecTypesList.end());
        m_pElecTypeComboBoxTemp->clear();
        m_pElecTypeComboBoxTemp->addItems(strElecTypesList);

        if (1 == m_pElecContainerBlock->getConnectedModel().size()) {
            for (int nIndex = 0; nIndex < strElecTypesList.count(); nIndex++) {
                QString strElementPrototypeName;
                QString strElementName = m_pElecTypeComboBoxTemp->itemText(nIndex);
                strElementPrototypeName = m_allBlockMap.value(m_strDrawBorderName).value(strElementName);

                QString strPrototypeName = m_pElecContainerBlock->getConnectedModel().at(0).data()->getPrototypeName();
                if (strPrototypeName == NPS::PROTOTYPENAME_DOTBUSBAR) {
                    strPrototypeName = NPS::PROTOTYPENAME_BUSBAR;
                }

                if (strElementPrototypeName == strPrototypeName) {
                    m_pElecTypeComboBoxTemp->setCurrentIndex(nIndex);
                    onElecTypeChanged(m_pElecTypeComboBoxTemp->itemText(nIndex));
                    return;
                }
            }
        }
        m_pElecTypeComboBoxTemp->setCurrentIndex(0);
        onElecTypeChanged(m_pElecTypeComboBoxTemp->itemText(0));
    }
}

void ElecTypeWidget::onElecTypeChanged(QString elementType)
{
    if (m_strDrawBorderName.isEmpty()) {
        return;
    }

    QStringList namePathsList = m_strDrawBorderName.split(".");
    if (namePathsList.isEmpty()) {
        return;
    }

    QList<QString> elecElementNameList;
    QString strElementType = m_allBlockMap.value(m_strDrawBorderName).value(elementType);
    if (strElementType.isEmpty()) {
        return;
    }

    m_electricalModelMap.clear();
    m_pElecElementComboBoxTemp->clear();
    // 获取画板上所有的元件
    foreach (PModel drawingBoard, m_drawingBoardList) {
        if (drawingBoard->getName() == namePathsList.first()) {
            auto elecDrawingBoard = drawingBoard.dynamicCast<DrawingBoardClass>();
            if (elecDrawingBoard) {
                allComponentsOnDrawingBoard(elecDrawingBoard, namePathsList, strElementType);
            }
        }
    }

    elecElementNameList = m_electricalModelMap.keys();
    emit typeChanged(strElementType); // 通知属性框窗口，刷新左上角元件图片
    qSort(elecElementNameList.begin(), elecElementNameList.end());
    m_pElecElementComboBoxTemp->addItems(elecElementNameList);

    if (0 != elecElementNameList.size()) {
        if (1 == m_pElecContainerBlock->getConnectedModel(m_strAssociateType).size()) {
            for (int nIndex = 0; nIndex < m_pElecElementComboBoxTemp->count(); nIndex++) {
                if (m_pElecElementComboBoxTemp->itemText(nIndex)
                    == m_pElecContainerBlock->getConnectedModel(m_strAssociateType).at(0).data()->getName()) {
                    m_pElecElementComboBoxTemp->setCurrentIndex(nIndex);
                    onElectricalElementChanged(m_pElecElementComboBoxTemp->itemText(nIndex));
                    return;
                }
            }
            m_pElecElementComboBoxTemp->setCurrentIndex(0);
            onElectricalElementChanged(m_pElecElementComboBoxTemp->itemText(0));
        } else {
            m_pElecElementComboBoxTemp->setCurrentIndex(-1);
        }
    }
}

void ElecTypeWidget::onElectricalElementChanged(QString ElementName)
{
    PModel block = m_electricalModelMap.value(ElementName);
    if (block && block->getName() == ElementName) {
        m_pElectricalBlock = block.dynamicCast<ElectricalBlock>();
        if (m_pElectricalBlock) {
            m_pGotoPushButton->setEnabled(true);
            emit blockChanged(m_pElectricalBlock);
            emit typeChanged(m_pElectricalBlock->getPrototypeName()); // 通知属性框窗口，刷新左上角元件图片
        }

        if (!m_isInitProcess && PropertyServerMng::getInstance().m_pGraphicsModelingServer != nullptr) {
            QStringList namePathsList = m_strDrawBorderName.split(".");
            QString strPrototypeName = QString("%1%2%3%4")
                                               .arg(namePathsList.last())
                                               .arg(" ")
                                               .arg(m_pElecElementComboBoxTemp->currentText())
                                               .arg(" "); // 该名称拼接要再最后加上一个空格，用于用户区别名称
            int index = PropertyServerMng::getInstance().m_pGraphicsModelingServer->getBlockPrototypeNextIndex(
                    strPrototypeName, getBlockDrawBoard(m_pElecContainerBlock->getParentModelUUID()),
                    m_pElecContainerBlock->getUUID());
            if (index != 0) {
                strPrototypeName = QString("%1%2").arg(strPrototypeName).arg(index);
            }
            m_lineEdit->setText(strPrototypeName);
        }
    }
}

void ElecTypeWidget::onGotoPushButtonClicked()
{
    QList<PModel> elecBoardsList =
            PropertyServerMng::getInstance().m_projectManagerServer->GetAllBoardModel(ElecBoardModel::Type);
    if (elecBoardsList.isEmpty() || m_strDrawBorderName.isEmpty()) {
        return;
    }
    QStringList pathList = m_strDrawBorderName.split(".");
    if (pathList.isEmpty()) {
        return;
    }
    for (auto board : elecBoardsList) {
        if (board->getName() != pathList.at(0)) {
            continue;
        }
        PropertyServerMng::getInstance().m_pGraphicsModelingServer->openDrawingBoard(board);
        bool isSuccess = true;
        if (1 < pathList.size()) { // 大于1，则说明需跳转至电气画板中的构造型里面
            int nIndex = 1; // 用于忽略pathList中第一个元素，因为pathList中第一个元素名称就是顶层画板名称
            auto combineBoardModel = getElecCombineBoardModel(board, pathList, nIndex);
            if (!combineBoardModel) {
                return;
            }
            isSuccess = PropertyServerMng::getInstance().m_pGraphicsModelingServer->setDisplayAndHighlightModel(
                    board, m_pElectricalBlock, combineBoardModel);
        } else {
            isSuccess = PropertyServerMng::getInstance().m_pGraphicsModelingServer->setDisplayAndHighlightModel(
                    board, m_pElectricalBlock);
        }

        if (!isSuccess) {
            LOGOUT(tr("Failed to locate and redirect to the specified electrical component! Please check if the "
                      "electrical component data "
                      "exists."),
                   LOG_ERROR); // 定位跳转到指定电气元件失败！请检查是否存在该电气元件数据。
        }
        return;
    }
}

void ElecTypeWidget::initUI()
{
    m_gridLayout = new QGridLayout(this);
    m_gridLayout->setMargin(0);
    m_gridLayout->setVerticalSpacing(15);
    m_gridLayout->setHorizontalSpacing(8);

    QLabel *nameLabel = new QLabel(this);
    nameLabel->setText(tr("Name")); // 名称
    nameLabel->setFixedWidth(50);
    m_gridLayout->addWidget(nameLabel, 0, 0, 1, 1);

    m_lineEdit = new KLineEdit(this);
    QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(m_lineEdit->sizePolicy().hasHeightForWidth());
    m_lineEdit->setSizePolicy(sizePolicy);
    // m_lineEdit->setText(m_pElecContainerBlock->parameterMap[Block::name].toString());
    m_lineEdit->setText(m_pElecContainerBlock->getName());
    m_gridLayout->addWidget(m_lineEdit, 0, 1, 1, 6);

    m_pGotoPushButton = new QPushButton(this);
    m_pGotoPushButton->setEnabled(false);
    m_pGotoPushButton->setText(tr("Goto"));
    m_gridLayout->addWidget(m_pGotoPushButton, 0, 7, 1, 1);

    QWidget *pLeftWidget = new QWidget(this);
    pLeftWidget->setObjectName("ElecTypeWidget_LeftWidget");
    QWidget *pHeader = new QWidget;
    pHeader->setObjectName("ElecTypeWidget_Header");
    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addStretch();
    hLayout->addWidget(new QLabel(CMA::ELEC_DRAWBOARD_NAME)); // 电气画板
    hLayout->addStretch();
    hLayout->setMargin(0);
    pHeader->setLayout(hLayout);

    m_pElecDrawBoardTreeWidget = new CustomTreeWidget(this, true);
    m_pElecDrawBoardTreeWidget->setObjectName("ElecTypeWidget_TreeWidget");
    m_pElecDrawBoardTreeWidget->setSelectionMode(QAbstractItemView::ExtendedSelection); // 设置选择模式为单选
    QVBoxLayout *pLeftVBoxLayout = new QVBoxLayout(this);
    pLeftVBoxLayout->setContentsMargins(0, 0, 1, 1);
    pLeftVBoxLayout->setSpacing(0);
    pLeftVBoxLayout->addWidget(pHeader);
    pLeftVBoxLayout->addWidget(m_pElecDrawBoardTreeWidget);
    pLeftWidget->setLayout(pLeftVBoxLayout);
    m_gridLayout->addWidget(pLeftWidget, 1, 0, 3, 4);

    QTableWidget *pTableWidget = new QTableWidget(this);
    pTableWidget->setRowCount(2);
    pTableWidget->setColumnCount(2);
    pTableWidget->setFrameShape(QFrame::NoFrame);
    pTableWidget->horizontalHeader()->setVisible(false);
    // Qt5 fix
    pTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    pTableWidget->verticalHeader()->setVisible(false);
    pTableWidget->verticalHeader()->setDefaultSectionSize(HORIZONTAL_HEADER_HEIGHT);
    pTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    pTableWidget->setSelectionBehavior(QAbstractItemView::SelectItems);
    pTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_gridLayout->addWidget(pTableWidget, 1, 4, 3, 4);

    QTableWidgetItem *elecTypeNameItem = new QTableWidgetItem(CMA::ELEC_ELEMENT_PROTOTYPENAME); // 元件类型
    pTableWidget->setItem(0, 0, elecTypeNameItem);
    QTableWidgetItem *elecElementItem = new QTableWidgetItem(CMA::ELEC_ELEMENT_NAME); // 元件名称
    pTableWidget->setItem(1, 0, elecElementItem);
    for (int nRow = 0; nRow < pTableWidget->rowCount(); nRow++) {
        pTableWidget->item(nRow, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    }

    m_pElecTypeComboBoxTemp = new QComboBox(this);
    m_pElecTypeComboBoxTemp->setView(new QListView());
    m_pElecTypeComboBoxTemp->setObjectName("ElecTypeComb");
    m_pElecTypeComboBoxTemp->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    pTableWidget->setCellWidget(0, 1, m_pElecTypeComboBoxTemp);

    m_pElecElementComboBoxTemp = new QComboBox(this);
    m_pElecElementComboBoxTemp->setView(new QListView());
    m_pElecElementComboBoxTemp->setObjectName("ElecElementComb");
    m_pElecElementComboBoxTemp->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    pTableWidget->setCellWidget(1, 1, m_pElecElementComboBoxTemp);

    QPushButton *editButton = new QPushButton(this);
    editButton->setMinimumSize(QSize(30, 0));
    editButton->setMaximumSize(QSize(30, 16777215));
    editButton->setText("编辑");
    editButton->setEnabled(false);
    editButton->setVisible(false);
    creatTypeMenu();
    m_gridLayout->addWidget(editButton, 0, 6, 1, 1);

    // m_typepath = new ElideLineText(this);
    // m_gridLayout->addWidget(m_typepath, 3, 3, 1, 1);

    QSpacerItem *vspacer = new QSpacerItem(20, 80, QSizePolicy::Minimum, QSizePolicy::Expanding);
    m_gridLayout->addItem(vspacer, 2, 0, 1, 4);

    connect(m_pElecDrawBoardTreeWidget, &CustomTreeWidget::itemClicked, this, &ElecTypeWidget::onDrawBorderNameClicked);
    connect(m_pElecTypeComboBoxTemp, SIGNAL(activated(QString)), this, SLOT(onElecTypeChanged(QString)));
    connect(m_pElecElementComboBoxTemp, SIGNAL(activated(QString)), this, SLOT(onElectricalElementChanged(QString)));
    connect(m_pGotoPushButton, &QPushButton::clicked, this, &ElecTypeWidget::onGotoPushButtonClicked);
}

void ElecTypeWidget::initTreeWidget(QSharedPointer<Kcc::BlockDefinition::DrawingBoardClass> drawingBoard,
                                    QTreeWidgetItem *item)
{
    if (drawingBoard == nullptr) {
        return;
    }

    QTreeWidgetItem *item_c = nullptr;
    // 电气画板上的所有模块中英文原型名称映射，key是模块的PrototypeName_CH，value是模块的PrototypeName
    QMap<QString, QString> allBlockNameMap;
    item_c = new QTreeWidgetItem(QStringList(drawingBoard->getName()));
    if (item == nullptr) {
        m_pElecDrawBoardTreeWidget->addTopLevelItem(item_c);
    } else {
        item->addChild(item_c);
    }

    QMapIterator<QString, PModel> iter(drawingBoard->getChildModels());
    while (iter.hasNext()) {
        iter.next();
        if (!iter.value()) {
            continue;
        }

        if (iter.value()->getModelType() == ElecCombineBoardModel::Type) {
            auto elecCombineBoard = iter.value().dynamicCast<ElecCombineBoardModel>();
            if (elecCombineBoard == nullptr) {
                break;
            }
            initTreeWidget(elecCombineBoard, item_c);
        }
        QString strBlockPrototypeName = iter.value()->getPrototypeName();
        if (!allBlockNameMap.contains(strBlockPrototypeName)
            && iter.value()->getModelType() != ElecCombineBoardModel::Type) {
            if (strBlockPrototypeName == NPS::PROTOTYPENAME_DOTBUSBAR
                || strBlockPrototypeName == NPS::PROTOTYPENAME_BUSBAR) {
                allBlockNameMap[tr("Busbar")] = NPS::PROTOTYPENAME_BUSBAR;
            } else {
                allBlockNameMap[iter.value()->getPrototypeName_CHS()] = iter.value()->getPrototypeName();
            }
        }
    }

    QString strPath = QString();
    if (item_c != nullptr) {
        strPath = m_pElecDrawBoardTreeWidget->getCurrentItemPathText(*item_c);
    }

    if (!strPath.isEmpty()) {
        if (0 < drawingBoard->getChildModels().size()) {
            m_allBlockMap[strPath] = allBlockNameMap;
        } else {
            m_pElecTypeComboBoxTemp->clear();
            m_pElecElementComboBoxTemp->clear();
        }
    }
}

QSharedPointer<Kcc::BlockDefinition::DrawingBoardClass> ElecTypeWidget::getBlockDrawBoard(const QString &boarduuid)
{
    foreach (PModel pdrawboard, m_drawingBoardList) {
        if (pdrawboard != nullptr && boarduuid == pdrawboard->getUUID()) {
            PDrawingBoardClass board = pdrawboard.dynamicCast<DrawingBoardClass>();
            return board;
        }
    }
    return PDrawingBoardClass(nullptr);
}

bool ElecTypeWidget::hasBlockName(const QString &namestr)
{
    QString str = m_lineEdit->text().trimmed();
    // 名称无更改时，直接返回
    if (m_pElecContainerBlock == nullptr) {
        return false;
    }

    if (m_pElecContainerBlock->getName() == namestr) {
        return false;
    }

    QList<PModel> drawingBoardList = PropertyServerMng::getInstance().m_projectManagerServer->GetAllBoardModel();
    foreach (PModel drawingBoard, drawingBoardList) {
        if (drawingBoard->getUUID() == m_pElecContainerBlock->getParentModelUUID()) {
            // 判断当前画板中是否有重名模块
            PDrawingBoardClass pDrawingBoard = drawingBoard.dynamicCast<DrawingBoardClass>();
            if (pDrawingBoard->isHadDuplicateBlockName(namestr)) {
                return true;
            } else {
                return false;
            }
        }
    }
    return false;
}

void ElecTypeWidget::creatTypeMenu()
{
    m_drawingBoardList = PropertyServerMng::getInstance().m_projectManagerServer->GetAllBoardModel();
    foreach (PModel drawingBoard, m_drawingBoardList) {
        if (drawingBoard == nullptr) {
            continue;
        }
        QString strDrawingBoardName = drawingBoard->getName();
        if (drawingBoard->getModelType() == ElecBoardModel::Type) {
            m_elecDrawingBoardNameList.append(strDrawingBoardName);
            auto drawingBoardClass = drawingBoard.dynamicCast<DrawingBoardClass>();
            if (drawingBoardClass) {
                initTreeWidget(drawingBoardClass);
            }
        }
    }

    qSort(m_elecDrawingBoardNameList.begin(), m_elecDrawingBoardNameList.end());

    if (0 != m_elecDrawingBoardNameList.size()) {
        auto elecBlockList = m_pElecContainerBlock->getConnectedModel(m_strAssociateType);
        if (elecBlockList.isEmpty()) {
            m_pElecTypeComboBoxTemp->setCurrentIndex(-1);
        } else {
            for (auto &elec : elecBlockList) {
                PModel elecBlock = elec.toStrongRef();
                if (elecBlock.isNull()) {
                    continue;
                }

                QString strModelPath = getCurrentModelParentsPathText(elecBlock);
                auto treeItemsList = getMatchingItems(strModelPath);
                for (auto &child : treeItemsList) {
                    if (child == nullptr) {
                        continue;
                    }
                    QString strTreePath = m_pElecDrawBoardTreeWidget->getCurrentItemPathText(*child);
                    if (strModelPath == strTreePath) {
                        auto parent = child->parent();
                        if (parent) {
                            m_pElecDrawBoardTreeWidget->expandItem(parent);
                        }
                        m_pElecDrawBoardTreeWidget->scrollToItem(child);
                        m_pElecDrawBoardTreeWidget->setCurrentItem(child);
                        onDrawBorderNameClicked(child);
                        break;
                    }
                }
            }
        }
    }
    m_isInitProcess = false;
}

void ElecTypeWidget::clearModelInfo(QSharedPointer<Kcc::BlockDefinition::Model> eleInterfaceModel)
{
    if (eleInterfaceModel == nullptr) {
        return;
    }

    auto allGroups = eleInterfaceModel->getVariableGroupList();
    for (auto group : allGroups) {
        if (group == nullptr || group->getGroupType() == RoleDataDefinition::MainGroup) {
            continue;
        }
        group->clearVariableMap();

        // 画板中显示电气接口原型字符的图
        auto modelTemp = CMA::getModelByPrototype(eleInterfaceModel->getPrototypeName());
        if (modelTemp == nullptr) {
            return;
        }
        eleInterfaceModel->setResource(modelTemp->getResource());
    }
}

QSharedPointer<Kcc::BlockDefinition::Model>
ElecTypeWidget::checkBlockIsConnected(QList<QWeakPointer<Kcc::BlockDefinition::Model>> modelList)
{
    PModel targetConnection;
    if (0 < modelList.size()) {
        auto modelConnection = modelList.at(0);
        if (!modelConnection) {
            // 所选电气元件的连接关系信息有误！无法检查是否已被其它模块连接。
            LOGOUT(tr("The connection relationship information of the selected electrical component is incorrect! "
                      "Unable to check if it has been connected by another module."),
                   LOG_ERROR);
            return QSharedPointer<Kcc::BlockDefinition::Model>();
        }
        targetConnection = modelConnection.toStrongRef();
        if (!targetConnection) {
            return QSharedPointer<Kcc::BlockDefinition::Model>();
        }
        if (targetConnection->getUUID() == m_pElecContainerBlock->getUUID()) {
            return QSharedPointer<Kcc::BlockDefinition::Model>();
        }
        return targetConnection;
    }
    return QSharedPointer<Kcc::BlockDefinition::Model>();
}

void ElecTypeWidget::allComponentsOnDrawingBoard(
        QSharedPointer<Kcc::BlockDefinition::DrawingBoardClass> elecDrawingBoard, QStringList namePathsList,
        QString strElementType)
{
    namePathsList.removeFirst();

    foreach (PModel block, elecDrawingBoard->getChildModels()) {
        if (namePathsList.isEmpty()) {
            if (block->getPrototypeName() == strElementType) {
                m_electricalModelMap[block->getName()] = block;
            } else {
                QString strPrototypeName = block->getPrototypeName();
                if (strPrototypeName == NPS::PROTOTYPENAME_DOTBUSBAR && strElementType == NPS::PROTOTYPENAME_BUSBAR) {
                    m_electricalModelMap[block->getName()] = block;
                }
            }
        } else {
            if (block->getName() == namePathsList.first()) {
                auto elecDrawingBoard = block.dynamicCast<DrawingBoardClass>();
                if (elecDrawingBoard) {
                    allComponentsOnDrawingBoard(elecDrawingBoard, namePathsList, strElementType);
                }
            }
        }
    }
}

QString ElecTypeWidget::getCurrentModelParentsPathText(QSharedPointer<Kcc::BlockDefinition::Model> curModel)
{
    QString text;
    QList<QString> parentsTextList;
    parentsTextList = getCurrentModelParents(curModel); // 遍历所选的item的所有父节点item

    // 构建字符串
    int nInitIndex = parentsTextList.size() - 1;
    for (int index = nInitIndex; index >= 0; index--) {
        auto itemText = parentsTextList.at(index);
        text.append(itemText);
        text.append(".");
    }
    text.chop(1); // 删除最后的逗号
    return text;
}

QList<QTreeWidgetItem *> ElecTypeWidget::getMatchingItems(const QString &itemPath)
{
    QList<QTreeWidgetItem *> items;
    for (int i = 0; i < m_pElecDrawBoardTreeWidget->topLevelItemCount(); i++) {
        QTreeWidgetItem *item = m_pElecDrawBoardTreeWidget->topLevelItem(i); // 获取根项
        if (item->text(0) == itemPath.split(".").first()) {
            if (1 == itemPath.split(".").size() && itemPath.split(".").first() == itemPath.split(".").last()) {
                items.append(item);
                return items;
            }
            findChildItems(item, itemPath.split(".").last(), items);
            break;
        }
    }
    return items;
}

void ElecTypeWidget::findChildItems(QTreeWidgetItem *parent, const QString &name, QList<QTreeWidgetItem *> &items)
{
    for (int i = 0; i < parent->childCount(); i++) {
        QTreeWidgetItem *child = parent->child(i);
        if (child->text(0) == name) {
            items.append(child);
        }
        findChildItems(child, name, items); // 递归查找子项
    }
}

QSharedPointer<Kcc::BlockDefinition::Model>
ElecTypeWidget::getElecCombineBoardModel(QSharedPointer<Kcc::BlockDefinition::Model> elecBoardModel,
                                         const QStringList &pathList, const int &index)
{
    QString strName = pathList.at(index);
    auto uuid = elecBoardModel->getUuidByChildName(strName);
    auto subSystemModel = elecBoardModel->getChildModel(uuid);
    if (subSystemModel == nullptr) {
        return PModel();
    }

    if (0 != subSystemModel->getChildModels().size()) {
        int nIndex = index + 1;
        if (nIndex >= pathList.size()) {
            return subSystemModel;
        }
        return getElecCombineBoardModel(subSystemModel, pathList, nIndex);
    }
    return QSharedPointer<Kcc::BlockDefinition::Model>();
}

QStringList ElecTypeWidget::getCurrentModelParents(QSharedPointer<Kcc::BlockDefinition::Model> curModel)
{
    QList<QString> parentsTextList;
    auto parentModel = curModel->getParentModel();
    if (!parentModel) {
        return parentsTextList;
    }
    parentsTextList.push_front(parentModel->getName());
    parentsTextList << getCurrentModelParents(parentModel);
    return parentsTextList;
}

//////////////////////////////////////////////////////////
ElecContainerPortTableWidget::ElecContainerPortTableWidget(
        QString name, QSharedPointer<Kcc::BlockDefinition::Model> eleInterfaceModel,
        QSharedPointer<Kcc::BlockDefinition::Model> elecModel, QWidget *parent /*= nullptr*/)
    : CWidget(parent),
      strWidgetName(name),
      m_pCurrentContainerBlock(eleInterfaceModel.dynamicCast<ElectricalContainerBlock>()),
      m_pElectricalBlock(elecModel.dynamicCast<ElectricalBlock>())
{
    initUI();
    PVariableGroup inputVarGroup = m_pCurrentContainerBlock->getVariableGroup(RoleDataDefinition::InputSignal);
    PVariableGroup outputVarGroup = m_pCurrentContainerBlock->getVariableGroup(RoleDataDefinition::OutputSignal);

    if (inputVarGroup) {
        m_mapInputVariables = inputVarGroup->getVariableMap();
    }

    if (outputVarGroup) {
        m_mapOutputVariables = outputVarGroup->getVariableMap();
    }
}

void ElecContainerPortTableWidget::displayBlockPortsInfo(
        QSharedPointer<Kcc::BlockDefinition::ElectricalBlock> electricalBlock)
{
    if (PropertyServerMng::getInstance().m_pIElecSysServer == nullptr) {
        return;
    }

    int nSelectCount = 0;
    PVariableGroup inputVarGroup;
    PVariableGroup outputVarGroup;

    m_pModel->clear();
    m_pModel->setHorizontalHeaderLabels(QStringList() << tr("Variable Name")          // 变量名称
                                                      << tr("Variable Default Value") // 变量默认值
                                                      << tr("Variable Description")); // 变量描述

    if (m_pCurrentContainerBlock) {
        inputVarGroup = m_pCurrentContainerBlock->getVariableGroup(RoleDataDefinition::InputSignal);
        outputVarGroup = m_pCurrentContainerBlock->getVariableGroup(RoleDataDefinition::OutputSignal);
    }

    if (electricalBlock) {
        auto inputparamMap = PropertyServerMng::getInstance().m_pIElecSysServer->GetInputParamMap(
                electricalBlock->getPrototypeName());
        auto outputparamMap = PropertyServerMng::getInstance().m_pIElecSysServer->GetOutputParamMap(
                electricalBlock->getPrototypeName(), electricalBlock->getPhaseNumber());

        if (strWidgetName == CMA::BLOCK_TAB_NAME_SELECT_INPUT) {
            QStringList tempInPortList = inputparamMap.keys();
            for (int index = 0; index < tempInPortList.size(); index++) {
                m_pModel->appendRow(QList<QStandardItem *>()
                                    << new QStandardItem(tempInPortList[index]) << new QStandardItem("0.0")
                                    << new QStandardItem(inputparamMap[tempInPortList[index]].description));

                if (inputVarGroup && inputVarGroup->getVariableMapNames().contains(tempInPortList[index])
                    && inputVarGroup->getVariableByName(tempInPortList[index])->getIsSelect()) {
                    m_pModel->item(index, 0)->setCheckState(Qt::Checked);
                    nSelectCount++;
                }
            }

            if (nSelectCount == tempInPortList.size() && nSelectCount) { // 检测是否全选
                m_pHeaderView->setHeaderCheckStatus(TableHeaderView::AllChecked, true);
            } else if (0 < nSelectCount && nSelectCount < tempInPortList.size()) {
                m_pHeaderView->setHeaderCheckStatus(TableHeaderView::PartChecked, false);
            } else {
                m_pHeaderView->setHeaderCheckStatus(TableHeaderView::NoChecked, false);
            }
            update();
        } else if (strWidgetName == CMA::BLOCK_TAB_NAME_SELECT_OUTPUT) {
            QStringList tempOutPortList = outputparamMap.keys();
            for (int index = 0; index < tempOutPortList.size(); index++) {
                m_pModel->appendRow(QList<QStandardItem *>()
                                    << new QStandardItem(tempOutPortList[index]) << new QStandardItem("0.0")
                                    << new QStandardItem(outputparamMap[tempOutPortList[index]].description));

                if (outputVarGroup && outputVarGroup->getVariableMapNames().contains(tempOutPortList[index])
                    && outputVarGroup->getVariableByName(tempOutPortList[index])->getIsSelect()) {
                    m_pModel->item(index, 0)->setCheckState(Qt::Checked);
                    nSelectCount++;
                }
            }

            if (nSelectCount == tempOutPortList.size() && nSelectCount) { // 检测是否全选
                m_pHeaderView->setHeaderCheckStatus(TableHeaderView::AllChecked, true);
            } else if (0 < nSelectCount && nSelectCount < tempOutPortList.size()) {
                m_pHeaderView->setHeaderCheckStatus(TableHeaderView::PartChecked, false);
            } else {
                m_pHeaderView->setHeaderCheckStatus(TableHeaderView::NoChecked, false);
            }
        }
    }
    // else {
    //     if (inputVarGroup && strWidgetName == CMA::BLOCK_TAB_NAME_SELECT_OUTPUT) {
    //         QList<QString> tempInPortList;
    //         foreach (QString keyname, inputVarGroup->getVariableMapNames()) {
    //             tempInPortList << keyname;
    //         }

    //         for (int index = 0; index < inputVarGroup->getVariableMap().size(); index++) {
    //             m_pModel->appendRow(
    //                     QList<QStandardItem *>()
    //                     << new QStandardItem(tempInPortList[index])
    //                     << new QStandardItem(inputVarGroup->getVariableByName(tempInPortList[index])
    //                                                  ->getDefaultValue()
    //                                                  .toString())
    //                     << new
    //                     QStandardItem(inputVarGroup->getVariableByName(tempInPortList[index])->getDataType()));

    //             if (inputVarGroup->getVariableByName(tempInPortList[index])->getIsSelect()) {
    //                 m_pModel->item(index, 0)->setCheckState(Qt::Checked);
    //                 nSelectCount++;
    //             }
    //         }
    //     } else if (outputVarGroup && strWidgetName == CMA::BLOCK_TAB_NAME_SELECT_OUTPUT) {
    //         QList<QString> tempOutPortList;
    //         foreach (QString keyname, outputVarGroup->getVariableMapNames()) {
    //             tempOutPortList << keyname;
    //         }

    //         for (int index = 0; index < outputVarGroup->getVariableMap().size(); index++) {
    //             m_pModel->appendRow(
    //                     QList<QStandardItem *>()
    //                     << new QStandardItem(tempOutPortList[index])
    //                     << new QStandardItem(outputVarGroup->getVariableByName(tempOutPortList[index])
    //                                                  ->getDefaultValue()
    //                                                  .toString())
    //                     << new
    //                     QStandardItem(outputVarGroup->getVariableByName(tempOutPortList[index])->getDataType()));

    //             if (outputVarGroup->getVariableByName(tempOutPortList[index])->getIsSelect()) {
    //                 m_pModel->item(index, 0)->setCheckState(Qt::Checked);
    //                 nSelectCount++;
    //             }
    //         }
    //     }
    // }
    QFontMetrics metric(tr("Variable Name"));
    if (metric.width(tr("Variable Name")) > FOUR_CHINESE_LENGTHS) {
        m_pHeaderView->resizeSection(0, TABLEWIDGET_ITEM_WITH_1);
        m_pHeaderView->resizeSection(1, TABLEWIDGET_ITEM_WITH_2);
    }
    m_pHeaderView->setSectionResizeMode(0, QHeaderView::Fixed);
    setColumnCheckable(0, true);
}

void ElecContainerPortTableWidget::initUI()
{
    QVBoxLayout *pVerticalLayout = new QVBoxLayout(this);
    pVerticalLayout->setMargin(0);
    pVerticalLayout->setSpacing(0);

    QLabel *pPortListTittle = new QLabel(this);
    pPortListTittle->setText(tr("Parameter List")); // 参数列表
    pPortListTittle->setObjectName("PortListTittle");
    pVerticalLayout->addWidget(pPortListTittle, 1);

    m_pInPortTableView = new QTableView(this);
    m_pInPortTableView->setFrameShape(QFrame::NoFrame);
    m_pInPortTableView->verticalHeader()->setVisible(false);
    m_pInPortTableView->verticalHeader()->setDefaultSectionSize(HORIZONTAL_HEADER_HEIGHT);
    m_pInPortTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_pInPortTableView->setSelectionMode(QAbstractItemView::NoSelection);
    m_pInPortTableView->setSelectionBehavior(QAbstractItemView::SelectItems);
    m_pInPortTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    pVerticalLayout->addWidget(m_pInPortTableView, 10);
    connect(m_pInPortTableView, SIGNAL(clicked(QModelIndex)), this, SLOT(onItemClicked(QModelIndex)));

    m_pModel = new QStandardItemModel(m_pInPortTableView);
    m_pModel->setHorizontalHeaderLabels(QStringList() << "Variable Name"          // 变量名称
                                                      << "Variable Default Value" // 变量默认值
                                                      << "Variable Description"); // 变量描述
    m_pInPortTableView->setModel(m_pModel);

    m_pHeaderView = new TableHeaderView(Qt::Horizontal, m_pInPortTableView);
    m_pHeaderView->setMaximumHeight(27);
    connect(m_pHeaderView, SIGNAL(columuSectionClicked(int, bool)), this, SLOT(onColumnAllSelected(int, bool)));
    m_pInPortTableView->setHorizontalHeader(m_pHeaderView);

    displayBlockPortsInfo(m_pElectricalBlock);
}

void ElecContainerPortTableWidget::setColumnCheckable(int col, bool checkable)
{
    m_pHeaderView->setColumnCheckable(col, checkable);
    for (int row = 0; row != m_pModel->rowCount(); ++row) {
        m_pModel->item(row, col)->setCheckable(checkable);
    }
}

void ElecContainerPortTableWidget::onItemClicked(QModelIndex item)
{
    Q_UNUSED(item);
    int nCheckedCount = 0;
    int nUnCheckedCount = 0;
    QList<QStandardItem *> itemList = m_pModel->findItems("*", Qt::MatchWildcard | Qt::MatchRecursive);

    for (int row = 0; row < itemList.length(); ++row) {
        QString strName = m_pModel->index(row, 0).data().toString();

        if (itemList.at(row)->checkState() == Qt::Checked) {
            // 被勾选
            nCheckedCount++;
            m_pHeaderView->setHeaderCheckStatus(TableHeaderView::PartChecked, false);
        } else {
            // 取消勾选
            nUnCheckedCount++;
            m_pHeaderView->setHeaderCheckStatus(TableHeaderView::PartChecked, false);
        }
    }

    // 检测是否全选或全取消
    if (nCheckedCount == itemList.length()) {
        m_pHeaderView->setHeaderCheckStatus(TableHeaderView::AllChecked, true);
    } else if (nUnCheckedCount == itemList.length()) {
        m_pHeaderView->setHeaderCheckStatus(TableHeaderView::NoChecked, false);
    }
}

void ElecContainerPortTableWidget::onColumnAllSelected(int col, bool selected)
{
    for (int row = 0; row != m_pModel->rowCount(); ++row) {
        QString strName = m_pModel->index(row, col).data().toString();
        if (selected) {
            m_pModel->item(row, col)->setCheckState(Qt::Checked);
            m_pHeaderView->setHeaderCheckStatus(TableHeaderView::AllChecked, true);
        } else {
            m_pModel->item(row, col)->setCheckState(Qt::Unchecked);
            m_pHeaderView->setHeaderCheckStatus(TableHeaderView::NoChecked, false);
        }
    }
}

QMap<QString, QSharedPointer<Kcc::BlockDefinition::Variable>>
ElecContainerPortTableWidget::getCheckedVaruablesMap(QString tabWidgetType)
{
    m_mapInputVariables.clear();
    m_mapOutputVariables.clear();
    QList<QStandardItem *> itemList = m_pModel->findItems("*", Qt::MatchWildcard | Qt::MatchRecursive);

    for (int row = 0; row < itemList.length(); ++row) {
        QString strName = m_pModel->index(row, 0).data().toString();
        // 被勾选，则添加到端口MAP
        if (itemList.at(row)->checkState() == Qt::Checked) {
            if (tabWidgetType == CMA::BLOCK_TAB_NAME_SELECT_INPUT) {
                PVariable variable(new Variable(nullptr));
                m_mapInputVariables[strName] = variable;
                m_mapInputVariables[strName]->setIsSelect(true);
                m_mapInputVariables[strName]->setDefaultValue(0.0);
                m_mapInputVariables[strName]->setDataType("double");
            } else {
                PVariable variable(new Variable(nullptr));
                m_mapOutputVariables[strName] = variable;
                m_mapOutputVariables[strName]->setIsSelect(true);
                m_mapOutputVariables[strName]->setDefaultValue(0.0);
                m_mapOutputVariables[strName]->setDataType("double");
            }
        }
        // 取消勾选，则从端口MAP中删除
        else {
            if (tabWidgetType == CMA::BLOCK_TAB_NAME_SELECT_INPUT && m_mapInputVariables.contains(strName)) {
                m_mapInputVariables.remove(strName);
            } else {
                m_mapOutputVariables.remove(strName);
            }
        }
    }

    if (tabWidgetType == CMA::BLOCK_TAB_NAME_SELECT_INPUT) {
        return m_mapInputVariables;
    } else {
        return m_mapOutputVariables;
    }
}

void ElecContainerPortTableWidget::setWidgetEnable(bool enable)
{
    m_pHeaderView->setEnabled(enable);
    m_pInPortTableView->setEnabled(enable);
}

void ElecContainerPortTableWidget::setColumnAllSelected(int col)
{
    for (int row = 0; row != m_pModel->rowCount(); ++row) {
        m_pModel->item(row, col)->setCheckState(Qt::Checked);
    }
    if (0 != m_pModel->rowCount()) {
        m_pHeaderView->setHeaderCheckStatus(TableHeaderView::AllChecked, true);
    }
}

void ElecContainerPortTableWidget::setCWidgetReadOnly(bool bReadOnly)
{
    if (m_pHeaderView != nullptr) {
        m_pHeaderView->setCheckBoxEnableStatus(!bReadOnly);
    }
    if (m_pInPortTableView != nullptr) {
        // fixme，可以再优化一下
        m_pInPortTableView->setDisabled(bReadOnly);
    }
}
