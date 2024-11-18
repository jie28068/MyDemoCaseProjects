#include "signalselectwidget.h"
#include "CoreLib/ServerManager.h"
#include "GlobalAssistant.h"
#include "KLModelDefinitionCore/KLModelDefinitionCore.h"
#include "customlineedit.h"

#include <QDrag>
#include <QHBoxLayout>
#include <QLabel>
#include <QMimeData>
#include <QPushButton>
#include <QVBoxLayout>

using namespace Kcc::BlockDefinition;
USE_LOGOUT_("signalselectwidget")

SignalSelectWidget::SignalSelectWidget(QSharedPointer<Kcc::BlockDefinition::Model> model, bool canEdit, QWidget *parent)
    : CWidget(parent), m_pModel(model), m_bIsItemExpandChanged(false), m_canEdit(canEdit)
{
    initUI();
    initData();
}

SignalSelectWidget::~SignalSelectWidget() { }

bool SignalSelectWidget::saveData(QSharedPointer<Kcc::BlockDefinition::Model> model)
{
    auto busSelectorBlock = model.dynamicCast<ControlBlock>();
    if (busSelectorBlock == nullptr) {
        return false;
    }

    QList<QString> itemsTextList;
    for (int i = 0; i < m_pSelectedSignalsListWidget->count(); i++) { // 遍历所选的item
        QListWidgetItem *item = m_pSelectedSignalsListWidget->item(i);
        QString str = item->text();
        itemsTextList.push_back(str);
    }

    busSelectorBlock->updataVariableInfo(m_oldItemsTextList.size(), itemsTextList.size(), itemsTextList);
    busSelectorBlock->parseDynamicBlockPorts();

    if (m_oldItemsTextList == itemsTextList) {
        return false;
    } else {
        return true;
    }
}

void SignalSelectWidget::setCWidgetReadOnly(bool bReadOnly)
{
    if (m_canEdit) {
        return;
    }
    setDisabled(bReadOnly);
}

void SignalSelectWidget::initUI()
{
    QGridLayout *pGridLayout = new QGridLayout(this);
    pGridLayout->setMargin(0);
    pGridLayout->setVerticalSpacing(8);
    pGridLayout->setHorizontalSpacing(10);

    CustomLineEdit *pSearchLineEdit = new CustomLineEdit(this, true);
    pSearchLineEdit->setObjectName("SearchLineEdit");
    pSearchLineEdit->setPlaceholderText(tr("Find by Name")); // 搜索
    QWidget *pHeader = new QWidget;
    pHeader->setObjectName("Header");
    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addStretch();
    hLayout->addWidget(new QLabel(tr("Elements in the bus"))); // 总线中的元素
    hLayout->addStretch();
    hLayout->setMargin(0);
    pHeader->setLayout(hLayout);

    m_pAllSignalsListWidget = new CustomTreeWidget(this);
    m_pAllSignalsListWidget->setSelectionMode(QAbstractItemView::ExtendedSelection); // 设置选择模式为单选
    m_pAllSignalsListWidget->setDragEnabled(true);                                   // 启用拖拽
    m_pAllSignalsListWidget->setDropIndicatorShown(true); // 设置显示将要被放置的位置
    m_pAllSignalsListWidget->setObjectName("AllSignalsListWidget");

    QVBoxLayout *pLeftVBoxLayoutHeader = new QVBoxLayout(this);
    pLeftVBoxLayoutHeader->addWidget(pSearchLineEdit);
    pLeftVBoxLayoutHeader->addWidget(pHeader);
    QVBoxLayout *pLeftVBoxLayoutTreeWidget = new QVBoxLayout(this);
    pLeftVBoxLayoutTreeWidget->addWidget(m_pAllSignalsListWidget);
    QVBoxLayout *pLeftVBoxLayout = new QVBoxLayout(this);
    pLeftVBoxLayout->addLayout(pLeftVBoxLayoutHeader);
    pLeftVBoxLayout->addLayout(pLeftVBoxLayoutTreeWidget);
    pGridLayout->addLayout(pLeftVBoxLayout, 0, 0, 0, 6);

    QLabel *textLabel = new QLabel(this);
    textLabel->setText(tr("Selected Element")); // 所选元素
    textLabel->setObjectName("TextLabel");

    QPushButton *upButton = new QPushButton(this);
    upButton->setObjectName("UpButton");
    connect(upButton, &QPushButton::clicked, this, &SignalSelectWidget::onBtnUpClicked);
    QPushButton *downButton = new QPushButton(this);
    downButton->setObjectName("DownButton");
    connect(downButton, &QPushButton::clicked, this, &SignalSelectWidget::onBtnDownClicked);
    QPushButton *removeButton = new QPushButton(this);
    removeButton->setObjectName("removeButton");
    connect(removeButton, &QPushButton::clicked, this, &SignalSelectWidget::onBtnRemoveClicked);

    QHBoxLayout *rightTopHBoxLayout = new QHBoxLayout(this);
    rightTopHBoxLayout->addWidget(textLabel, 3);
    rightTopHBoxLayout->addWidget(upButton, 1);
    rightTopHBoxLayout->addWidget(downButton, 1);
    rightTopHBoxLayout->addWidget(removeButton, 1);
    m_pSelectedSignalsListWidget = new CustomListWidget(this);
    m_pSelectedSignalsListWidget->setAcceptDrops(true);
    m_pSelectedSignalsListWidget->setSelectionMode(QAbstractItemView::SingleSelection); // 设置选择模式为单选
    m_pSelectedSignalsListWidget->setDragEnabled(true);                                 // 启用拖拽
    m_pSelectedSignalsListWidget->viewport()->setAcceptDrops(true);                     // 设置接受拖放
    m_pSelectedSignalsListWidget->setDropIndicatorShown(true); // 设置显示将要被放置的位置
    m_pSelectedSignalsListWidget->setDragDropMode(
            QAbstractItemView::InternalMove); // 设置拖放模式为移动项目，如果不设置，默认为复制项目
    m_pSelectedSignalsListWidget->setObjectName("SelectedSignalsListWidget");

    QVBoxLayout *pRightVBoxLayout = new QVBoxLayout(this);
    pRightVBoxLayout->addLayout(rightTopHBoxLayout);
    pRightVBoxLayout->addWidget(m_pSelectedSignalsListWidget);
    pGridLayout->addLayout(pRightVBoxLayout, 0, 6, 0, 6);

    setLayout(pGridLayout);

    connect(pSearchLineEdit, &QLineEdit::textChanged, [&](const QString &text) {
        // 搜索子项并展开
        QString searchText = text.toLower();
        if (!searchText.isEmpty()) {
            for (int i = 0; i < m_pAllSignalsListWidget->topLevelItemCount(); i++) {
                QTreeWidgetItem *item = m_pAllSignalsListWidget->topLevelItem(i);
                treeWidgetSearchAndExpandItem(item, searchText);
            }
        } else {
            m_pAllSignalsListWidget->clear();
            setElementInBus(m_pModel);
        }
    });

    connect(m_pAllSignalsListWidget, &CustomTreeWidget::doubleClickedItem, [&](QTreeWidgetItem *item) {
        if (!m_bIsItemExpandChanged) {
            QString strItemName = m_pAllSignalsListWidget->getCurrentItemPathText(*item);
            QList<QString> itemsTextList = m_pSelectedSignalsListWidget->getAllItemsText(); // 获取列表中已有的文本数据
            if (!strItemName.isEmpty() && !itemsTextList.contains(strItemName)) {
                QListWidgetItem *itemB = new QListWidgetItem(strItemName);
                m_pSelectedSignalsListWidget->addItem(itemB);
                // m_pAllSignalsListWidget->takeItem(m_pAllSignalsListWidget->row(item));// 预留功能
            }
        }
        m_bIsItemExpandChanged = false;
    });

    QObject::connect(m_pAllSignalsListWidget, &QTreeWidget::itemExpanded,
                     [&](QTreeWidgetItem *item) { m_bIsItemExpandChanged = true; });

    QObject::connect(m_pAllSignalsListWidget, &QTreeWidget::itemCollapsed,
                     [&](QTreeWidgetItem *item) { m_bIsItemExpandChanged = true; });
    if (!m_canEdit) {
        this->setDisabled(true);
    }

    // connect(m_pSelectedSignalsListWidget, &CustomListWidget::removeSelectedItems, m_pAllSignalsListWidget,
    //         &CustomListWidget::onRemoveSelectedItems);// 预留功能
}

void SignalSelectWidget::initData()
{
    QList<QString> initNameList;
    initNameList = setElementInBus(m_pModel);

    auto portsGroup = m_pModel->getPortManager();
    if (portsGroup == nullptr) {
        return;
    }

    QList<PVariable> inputPortsList = portsGroup->getPortListSortByOrder(Variable::ControlOut);
    for (auto portVar : inputPortsList) {
        if (portVar == nullptr) {
            continue;
        }
        if (initNameList.contains(portVar->getName()) || portVar->getName().contains("??? ")) {
            m_oldItemsTextList.push_back(portVar->getName());
        } else {
            m_oldItemsTextList.push_back("??? " + portVar->getName());
        }
    }
    m_pSelectedSignalsListWidget->addItems(m_oldItemsTextList);
}

QList<QString> SignalSelectWidget::setElementInBus(QSharedPointer<Kcc::BlockDefinition::Model> model, const QString &id,
                                                   QTreeWidgetItem *item)
{
    QSharedPointer<Kcc::BlockDefinition::Model> parentModel = model->getParentModel();
    if (parentModel == nullptr) {
        return QList<QString>();
    }

    QSharedPointer<Kcc::BlockDefinition::DrawingBoardClass> drawingBoard = parentModel.dynamicCast<DrawingBoardClass>();
    if (drawingBoard == nullptr) {
        return QList<QString>();
    }

    QList<QString> initLinkNameList;
    QTreeWidgetItem *topItem = nullptr;
    QTreeWidgetItem *childItem = nullptr;
    QList<PModel> modelList = drawingBoard->getAdjacentChild(model->getUUID(), Block::InputTable);
    for (auto m : modelList) {
        if (m == nullptr) {
            continue;
        }

        if (model->getPrototypeName() != NPS::PROTOTYPENAME_CTRLOUT && !id.isEmpty() && m->getUUID() != id) {
            continue;
        }

        if (m_pModel == model) {
            item = nullptr;
        }

        if (m->getPrototypeName() == NPS::PROTOTYPENAME_BUSCREATOR) {
            auto portsGroup = m->getPortManager();
            if (portsGroup == nullptr) {
                return QList<QString>();
            }

            QList<PBlockConnector> connectorList = drawingBoard->getAdjacentConnector(m->getUUID());
            QList<PVariable> inputPortsList = portsGroup->getPortListSortByOrder(Variable::ControlIn);
            for (auto portVar : inputPortsList) {
                if (portVar == nullptr) {
                    continue;
                }

                if (item == nullptr) {
                    topItem = new QTreeWidgetItem(QStringList(portVar->getName()));
                    m_pAllSignalsListWidget->addTopLevelItem(topItem);
                    QString strText = m_pAllSignalsListWidget->getCurrentItemPathText(*topItem);
                    initLinkNameList.append(strText);

                    for (auto connectorVar : connectorList) {
                        if (connectorVar == nullptr) {
                            continue;
                        }

                        // 获取当前连接线连接的输入端口
                        auto inputPort =
                                portsGroup->findVariable(RoleDataDefinition::UuidRole, connectorVar->getDstPortUuid());
                        if (1 != inputPort.size() || inputPort.at(0) == nullptr) {
                            continue;
                        }

                        auto strInputPortName = inputPort.at(0)->getName();
                        if (connectorVar->getDstModelUuid() == m->getUUID() && inputPort.at(0) == portVar) {
                            initLinkNameList << setElementInBus(m, connectorVar->getSrcModelUuid(), topItem);
                        }
                    }
                } else {
                    childItem = new QTreeWidgetItem(QStringList(portVar->getName()));
                    item->addChild(childItem);
                    QString strText = m_pAllSignalsListWidget->getCurrentItemPathText(*childItem);
                    initLinkNameList.append(strText);

                    QList<PBlockConnector> connectorList = drawingBoard->getAdjacentConnector(m->getUUID());
                    for (auto connectorVar : connectorList) {
                        if (connectorVar == nullptr) {
                            continue;
                        }

                        // 获取当前连接线连接的输入端口
                        auto inputPort =
                                portsGroup->findVariable(RoleDataDefinition::UuidRole, connectorVar->getDstPortUuid());
                        if (1 != inputPort.size() || inputPort.at(0) == nullptr) {
                            continue;
                        }
                        auto strInputPortName = inputPort.at(0)->getName();
                        if (connectorVar->getDstModelUuid() == m->getUUID() && inputPort.at(0) == portVar) {
                            initLinkNameList << setElementInBus(m, connectorVar->getSrcModelUuid(), childItem);
                        }
                    }
                }
            }
        } else if (m->getModelType() == CombineBoardModel::Type) {
            QList<PBlockConnector> modelList = drawingBoard->getAdjacentConnector(model->getUUID());
            for (auto connector : modelList) {
                if (connector == nullptr) {
                    continue;
                }

                if (m->getUUID() == connector->getSrcModelUuid()) {
                    QString outputBlockID = connector->getSrcPortUuid();
                    auto portManager = m->getPortManager();
                    if (portManager == nullptr) {
                        continue;
                    }
                    QString portName;
                    auto portVar = portManager->getVariable(outputBlockID);
                    if (portVar != nullptr) {
                        portName = portVar->getName();
                    }

                    auto outputBlockMap = m->getChildModels();
                    for (auto outputBlock : outputBlockMap) {
                        if (outputBlock == nullptr || outputBlock->getName() != portName) {
                            continue;
                        }
                        initLinkNameList << setElementInBus(outputBlock, connector->getSrcModelUuid(), item);
                    }
                }
            }
        }
    }
    return initLinkNameList;
}

void SignalSelectWidget::treeWidgetSearchAndExpandItem(QTreeWidgetItem *item, const QString &searchText)
{
    QString search = item->text(0);
    if (item->text(0).toLower().contains(searchText)) {
        item->setHidden(false);
        item->setExpanded(true);
        setParentItemsExpand(item);
        setChildItemsExpand(item);
    } else {
        item->setHidden(true);
        item->setExpanded(false);
        for (int i = 0; i < item->childCount(); i++) {
            treeWidgetSearchAndExpandItem(item->child(i), searchText);
        }
    }
}

void SignalSelectWidget::setParentItemsExpand(QTreeWidgetItem *item)
{
    auto parentItem = item->parent();
    if (!parentItem) {
        return;
    } else {
        parentItem->setHidden(false);
        parentItem->setExpanded(true);
        setParentItemsExpand(parentItem);
    }
}

void SignalSelectWidget::setChildItemsExpand(QTreeWidgetItem *item)
{
    for (int i = 0; i < item->childCount(); i++) {
        auto childItem = item->child(i);
        if (!childItem) {
            return;
        } else {
            childItem->setHidden(false);
            childItem->setExpanded(true);
            setChildItemsExpand(childItem);
        }
    }
}

void SignalSelectWidget::onBtnUpClicked()
{
    QListWidgetItem *pSelItem = m_pSelectedSignalsListWidget->currentItem(); // 获取用户当前选中项
    if (pSelItem != nullptr)                                                 // 判断非空，否则点击会出错
    {
        int nCurrRow = m_pSelectedSignalsListWidget->currentRow(); // 获取当前行号
        QString strSel = pSelItem->text();                         // 获取当前行内容
        if (nCurrRow > 0)                                          // 限制（上移至第0行终止）
        {
            delete m_pSelectedSignalsListWidget->takeItem(nCurrRow); // 删除当前行
            m_pSelectedSignalsListWidget->insertItem(nCurrRow - 1, strSel); // 在上一行插入新行，内容为之前选中内容
        }
        m_pSelectedSignalsListWidget->setCurrentRow(nCurrRow - 1); // 将上移后的新行选中，实现连续上移
    }
}

void SignalSelectWidget::onBtnDownClicked()
{
    QListWidgetItem *pSelItem = m_pSelectedSignalsListWidget->currentItem();
    if (pSelItem != nullptr) {
        int nCurrRow = m_pSelectedSignalsListWidget->currentRow();
        QString strSel = pSelItem->text();
        int nRowAll = m_pSelectedSignalsListWidget->count(); // 获取当前list总行数
        if ((nRowAll - nCurrRow) > 0)                        // 限制（下移至最后一行终止）
        {
            delete m_pSelectedSignalsListWidget->takeItem(nCurrRow);
            m_pSelectedSignalsListWidget->insertItem(nCurrRow + 1, strSel);
        }
        m_pSelectedSignalsListWidget->setCurrentRow(nCurrRow + 1);
    }
}

void SignalSelectWidget::onBtnRemoveClicked()
{
    QList<QListWidgetItem *> pSelItemsList = m_pSelectedSignalsListWidget->selectedItems();
    for (auto pSelitem : pSelItemsList) {
        if (pSelitem != nullptr) {
            // m_pAllSignalsListWidget->addItem(pSelitem->text());// 预留功能
            m_pSelectedSignalsListWidget->takeItem(m_pSelectedSignalsListWidget->row(pSelitem)); // 删除选中item
        }
    }
}

/// @brief
/// @param parent
CustomListWidget::CustomListWidget(QWidget *parent) { }

void CustomListWidget::dropEvent(QDropEvent *event)
{
    if (event->source() == this) {
        return QListView::dropEvent(event);
    }

    bool isAddItems = false;
    QList<QString> itemsTextList;
    // 获取拖放的数据
    const QMimeData *mimeData = event->mimeData();
    QByteArray textData = mimeData->data("application/x-qabstractitemmodeldatalist");
    QString text = QString::fromLocal8Bit(textData);      // 将QByteArray转换为QString
    QStringList textItems = text.split(",");              // 使用split()方法根据分隔符分割字符串
    for (QString itemText : textItems) {                  // 遍历QStringList并append到QList中
        QList<QString> itemsTextList = getAllItemsText(); // 获取列表中已有的文本数据
        if (itemText.isEmpty() || itemsTextList.contains(itemText)) {
            event->ignore();
        } else {
            event->accept();
            this->addItem(itemText);
            isAddItems = true;
        }
    }

    if (isAddItems) {
        emit removeSelectedItems();
    }
}

void CustomListWidget::dragEnterEvent(QDragEnterEvent *event)
{
    event->accept();
    return QListView::dragEnterEvent(event);
}

void CustomListWidget::startDrag(Qt::DropActions supportedActions)
{
    Q_UNUSED(supportedActions);

    m_selItems = this->selectedItems();

    QList<QString> itemsTextList;
    for (int i = 0; i < m_selItems.size(); i++) { // 遍历所选的item
        QListWidgetItem *sel = m_selItems[i];
        QString str = sel->text();
        itemsTextList.push_back(str);
    }

    // 构建字符串
    QString text;
    QByteArray textData;
    for (QString itemText : itemsTextList) {
        text.append(itemText);
        text.append(",");
    }
    text.chop(1);                  // 删除最后的逗号
    textData = text.toLocal8Bit(); // 转换编码并传给QByteArray

    // 设置拖放的数据
    QMimeData *mimeData = new QMimeData;
    mimeData->setData("application/x-qabstractitemmodeldatalist", textData);

    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->exec(Qt::CopyAction);
}

QList<QString> CustomListWidget::getAllItemsText()
{
    int count = this->count();
    QList<QString> itemsTextList;
    for (int i = 0; i < count; ++i) {
        QListWidgetItem *item = this->item(i);
        if (item == nullptr) {
            continue;
        }
        itemsTextList.push_back(item->text());
    }
    return itemsTextList;
}

void CustomListWidget::onRemoveSelectedItems()
{
    for (int i = 0; i < m_selItems.size(); i++) { // 遍历所选的item
        QListWidgetItem *sel = m_selItems[i];
        takeItem(row(sel)); // 删除选中item
    }
}
