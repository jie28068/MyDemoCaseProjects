#include "customtableview.h"

#include "customtableitemdelegate.h"

#include "GlobalAssistant.h"
#include "TableModelDataModel.h"
#include <QApplication>
#include <QClipboard>
#include <QHeaderView>

CustomTableView::CustomTableView(QWidget *parent /*= nullptr*/)
    : KTableView(parent), m_bMousePressed(false), m_selectStartRow(-1)
{

    // 设置编辑模式
    this->setEditTriggers(QAbstractItemView::DoubleClicked);
    // 开启tableView的右键菜单
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    this->horizontalHeader()->setStretchLastSection(true);
    this->setSortingEnabled(true);
    // this->horizontalScrollBar()->setVisible(false);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->horizontalHeader()->setHighlightSections(false);
    // this->horizontalHeader()->setDefaultSectionSize(150);
    // this->verticalHeader()->setHighlightSections(false);
    this->verticalHeader()->setMinimumWidth(30);
    this->verticalHeader()->setDefaultSectionSize(26);
    this->verticalHeader()->setDefaultAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    this->verticalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);

    QObject::connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this,
                     SLOT(onClickedMouseRightButton(const QPoint &)));
    QObject::connect(this->verticalHeader(), SIGNAL(sectionDoubleClicked(int)), this,
                     SLOT(onVHeaderDoubleClicked(int)));
    QObject::connect(this->verticalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(onVHeaderSectionClicked(int)));
    QObject::connect(this->verticalHeader(), SIGNAL(customContextMenuRequested(const QPoint &)), this,
                     SLOT(onHeaderMenuRequested(const QPoint &)));
    QAbstractButton *cornerButton = this->findChild<QAbstractButton *>(); // 获取表格左上角按钮
    if (cornerButton != nullptr) {
        QObject::connect(cornerButton, SIGNAL(clicked()), this, SLOT(onCornerBtnClicked()));
    }
    this->setItemDelegate(new CustomTableItemDelegate(this));
}

CustomTableView::~CustomTableView() { }

const QList<ProjActionManager::ActionType> CustomTableView::getTableMenuEnableList(const QModelIndex &tableindex)
{
    QModelIndex index = this->currentIndex();
    if (tableindex.isValid()) {
        index = tableindex;
    }
    if (!index.isValid()) {
        return QList<ProjActionManager::ActionType>();
    }
    QList<ProjActionManager::ActionType> enableList;
    TableBaseModel::ModelDataType ModelDataType =
            TableBaseModel::ModelDataType(index.data(TableBaseModel::CustomRole_ModelDataType).toInt());
    bool isRuningDrawboard = index.data(TableBaseModel::CustomRole_DrawboardRunningSts).toBool();
    if (TableBaseModel::ModelDataType_Invalid == ModelDataType || TableBaseModel::ModelDataType_Tree == ModelDataType
        || isRuningDrawboard) {
        return enableList;
    }
    QString keywordStr = index.data(NPS::ModelDataKeywordsRole).toString();
    bool bEditable = index.data(NPS::ModelDataEditRole).toBool();
    QModelIndexList selectedlist = this->selectedIndexes();
    bool projectActive = index.data(TableBaseModel::CustomRole_CurProjectActiveSts).toBool();

    AssistantDefine::WidgetObject widgetObj =
            AssistantDefine::WidgetObject(index.data(AssistantDefine::ModelDataObjectRole).toInt());
    CustomMineData *cMineData = nullptr;
    if (QApplication::clipboard()->mimeData() != nullptr) {
        cMineData = dynamic_cast<CustomMineData *>((QMimeData *)QApplication::clipboard()->mimeData());
    }
    if (TableBaseModel::ModelDataType_CodeBlocks == ModelDataType) {
        // open,edit
        bool canOpen = index.data(TableBaseModel::CustomRole_CanOpen).toBool();
        if ((AssistantDefine::KEYWORD_TITLE_MODELNAME == keywordStr) && projectActive) {
            if (canOpen) {
                enableList << ProjActionManager::Act_open;
            }
            enableList << ProjActionManager::Act_edit;
        }
    } else if (TableBaseModel::ModelDataType_ControlBoardBlocks == ModelDataType
               || TableBaseModel::ModelDataType_CombineBoardBlocks == ModelDataType
               || TableBaseModel::ModelDataType_ComplexBoardBlocks == ModelDataType) {
        // open,edit
        if ((AssistantDefine::KEYWORD_TITLE_MODELNAME == keywordStr
             || AssistantDefine::KEYWORD_TITLE_ELEMENTNAME == keywordStr
             || AssistantDefine::KEYWORD_TITLE_SLOT == keywordStr)
            && projectActive) {
            enableList << ProjActionManager::Act_open << ProjActionManager::Act_edit;
        }
    } else if (TableBaseModel::ModelDataType_ComplexBoardInstances == ModelDataType) {
        // edit
        if ((AssistantDefine::KEYWORD_TITLE_ELEMENTNAME == keywordStr) && projectActive
            && !index.data().toString().isEmpty()) {
            enableList << ProjActionManager::Act_edit;
        }
    } else if (TableBaseModel::ModelDataType_GlobalDeviceTypes == ModelDataType) {
        if (AssistantDefine::KEYWORD_TITLE_DEVICETYPE_NAME == keywordStr) {
            enableList << ProjActionManager::Act_copy;
        }
        // 内建模式放开编辑全局设备类型
#ifdef COMPILER_SUPPORT_BUILTIN_MODIFY
        enableList << ProjActionManager::Act_edit;
#endif
    } else if (TableBaseModel::ModelDataType_ProjectDeviceTypes == ModelDataType) {
        // copy,paste,new,delete,edit
        if (AssistantDefine::KEYWORD_TITLE_DEVICETYPE_NAME == keywordStr) {
            QString devicePrototype = index.data(TableBaseModel::CustomRole_DevicePrototype).toString();
            enableList << ProjActionManager::Act_copy;
            if (AssistantDefine::WidgetObject_DataManagerTable == widgetObj && projectActive) {
                enableList << ProjActionManager::Act_new << ProjActionManager::Act_delete
                           << ProjActionManager::Act_edit;
            } else if (AssistantDefine::WidgetObject_NetworkTable == widgetObj && projectActive) {
                enableList << ProjActionManager::Act_edit;
            }
            if (projectActive && cMineData != nullptr
                && cMineData->getModels(DeviceModel::Type, devicePrototype).size() == 1) {
                enableList << ProjActionManager::Act_paste;
            }
        }
        if (bEditable && selectedlist.size() > 1) {
            enableList << ProjActionManager::Act_BatchModify;
        }
    } else if (TableBaseModel::ModelDataType_ElectricalBlocks == ModelDataType && projectActive) {
        // open edit/edit/edit
        if (AssistantDefine::KEYWORD_TITLE_ELEMENTNAME == keywordStr) {
            enableList << ProjActionManager::Act_open << ProjActionManager::Act_edit;
        } else if (AssistantDefine::KEYWORD_TITLE_DEVICETYPE_NAME == keywordStr
                   || AssistantDefine::KEYWORD_TITLE_BOARDNAME == keywordStr) {
            enableList << ProjActionManager::Act_edit;
        }
        if (bEditable && selectedlist.size() > 1) {
            enableList << ProjActionManager::Act_BatchModify;
        }
    } else if ((TableBaseModel::ModelDataType_CodeInstance == ModelDataType
                || TableBaseModel::ModelDataType_CombineBoardInstance == ModelDataType)
               && projectActive) {
        // batch modify
        if (bEditable && selectedlist.size() > 1) {
            enableList << ProjActionManager::Act_BatchModify;
        }
    } else if (TableBaseModel::ModelDataType_ControlInstances == ModelDataType) {
        if (AssistantDefine::KEYWORD_TITLE_MODELNAME == keywordStr) {
            enableList << ProjActionManager::Act_edit;
        }
        // batch modify
        if (bEditable && selectedlist.size() > 1 && AssistantDefine::KEYWORD_TITLE_MODELNAME != keywordStr) {
            enableList << ProjActionManager::Act_BatchModify;
        }
    }
    return enableList;
}

void CustomTableView::onClickedMouseRightButton(const QPoint &point)
{
    QModelIndex index = this->indexAt(point);
    createCustomMenu(index);
}

void CustomTableView::onVHeaderDoubleClicked(int logicalIndex)
{
    QModelIndex index = model()->index(logicalIndex, 0);
    emit leftMouseDoubleClickedComponent(index);
    emit selectedRowsChanged(getSelectRowNos());
}

void CustomTableView::onVHeaderSectionClicked(int logicalIndex)
{
    emit selectedRowsChanged(getSelectRowNos());
}

void CustomTableView::onHeaderMenuRequested(const QPoint &pos)
{
    if (this->verticalHeader() == nullptr) {
        return;
    }
    int logicalIndex = this->verticalHeader()->logicalIndexAt(pos);
    QModelIndex headerindex = this->verticalHeader()->currentIndex();
    this->verticalHeader()->setCurrentIndex(headerindex.sibling(logicalIndex, headerindex.column()));
    this->selectRow(logicalIndex);
    QModelIndex index = model()->index(logicalIndex, 0);
    createCustomMenu(index);
}

void CustomTableView::onCornerBtnClicked()
{
    emit selectedRowsChanged(getSelectRowNos());
}

void CustomTableView::mouseDoubleClickEvent(QMouseEvent *e)
{
    QTableView::mouseDoubleClickEvent(e);
    QModelIndex index = indexAt(e->pos());
    emit leftMouseDoubleClickedComponent(index);
}

void CustomTableView::contextMenuEvent(QContextMenuEvent *e)
{
    QTableView::contextMenuEvent(e);
    // QModelIndex index = indexAt(e->pos());
    // if (!index.isValid() || m_menu == nullptr) {
    //	QTableView::contextMenuEvent(e);
    //	return;
    // }

    // if (index.column() != 0) { //只有右键第0列（“模块名称”列）才弹出菜单
    //	return;
    // }
    //
    // m_menu->exec(this->mapToGlobal(e->pos()));
    // e->accept();
}

void CustomTableView::mousePressEvent(QMouseEvent *e)
{
    // 只处理左键
    QTableView::mousePressEvent(e);
    if (e->button() == Qt::LeftButton) {
        QModelIndex index = this->indexAt(e->pos());
        if (index.isValid()) {
            m_bMousePressed = true;
            m_selectStartRow = index.row();
        }
    }
}

void CustomTableView::mouseReleaseEvent(QMouseEvent *e)
{
    QTableView::mouseReleaseEvent(e);
    m_bMousePressed = false;

    QModelIndex index = this->indexAt(e->pos());
    if (index.isValid()) {
        emit selectedRowsChanged(getSelectRowNos());
    } else {
        emit selectedRowsChanged(0);
    }
}

void CustomTableView::mouseMoveEvent(QMouseEvent *e)
{
    // 只处理左键
    QTableView::mouseMoveEvent(e);
    if (m_bMousePressed) {
        QModelIndex index = this->indexAt(e->pos());
        if (index.isValid()) {
            emit selectedRowsChanged(getSelectRowNos());
        }
    }
    setCursor(QCursor(Qt::ArrowCursor));
}

void CustomTableView::focusOutEvent(QFocusEvent *e)
{
    QWidget::focusOutEvent(e);
    emit selectedRowsChanged(0);
}

void CustomTableView::focusInEvent(QFocusEvent *e)
{
    QWidget::focusInEvent(e);
    emit selectedRowsChanged(getSelectRowNos());
}

void CustomTableView::createCustomMenu(const QModelIndex &index)
{
    if (!index.isValid()) { // 右键空白处，不显示菜单
        return;
    }
    AssistantDefine::WidgetObject widgetObj =
            AssistantDefine::WidgetObject(index.data(AssistantDefine::ModelDataObjectRole).toInt());
    QList<ProjActionManager::ActionType> enableList = getTableMenuEnableList(index);
    QString newText = AssistantDefine::STR_ACT_NEW;
    ProjActionManager::getInstance().setTableMenuStatus(enableList, newText);
    if (enableList.size() <= 0) {
        return;
    }
    ProjActionManager::getInstance().setActTriggerObj(widgetObj);
    QMenu *tableMenu = ProjActionManager::getInstance().getMenu(ProjActionManager::MenuType_Table);
    if (tableMenu != nullptr) {
        tableMenu->setObjectName("uniformStyleMenu_cV");
        tableMenu->exec(cursor().pos());
    }
    emit tableMenuEnd();
}

int CustomTableView::getSelectRowNos()
{
    QModelIndexList allselects = this->selectedIndexes();
    QList<int> rowlist;
    for (QModelIndex index : allselects) {
        if (!index.isValid()) {
            continue;
        }
        bool haverow = false;
        for (int i = 0; i < rowlist.size(); ++i) {
            if (rowlist[i] == index.row()) {
                haverow = true;
                break;
            }
        }
        if (!haverow) {
            rowlist.append(index.row());
        }
    }
    return rowlist.size();
}

// 自定义滚动条
CustomHorizontalScrollBar::CustomHorizontalScrollBar(QWidget *parent /*= nullptr*/) : QScrollBar(parent)
{
    this->setOrientation(Qt::Horizontal);
    this->setSingleStep(10);
    connect(this, SIGNAL(valueChanged(int)), this, SLOT(onValueChanged(int)));
}

CustomHorizontalScrollBar::~CustomHorizontalScrollBar() { }

void CustomHorizontalScrollBar::initCustomScrollbar(const int &freezecols, int min, int max)
{
    m_freezeCols = freezecols;
    m_oldposition = 0;
    this->setValue(0);
    this->setRange(min, max);
}

// QSize CustomHorizontalScrollBar::sizeHint() const
//{
//	return QSize(15,15);
// }

void CustomHorizontalScrollBar::onValueChanged(int value)
{
    // 左滑
    if (m_oldposition > value) {
        emit scrollbarValueChanged(false, m_freezeCols + m_oldposition - 1, m_oldposition - value);
    } else if (m_oldposition < value) {
        // 右滑
        emit scrollbarValueChanged(true, m_freezeCols + m_oldposition, value - m_oldposition);
    }
    m_oldposition = value;
}

void CustomHorizontalScrollBar::onTableRangeChanged(int min, int max) { }
