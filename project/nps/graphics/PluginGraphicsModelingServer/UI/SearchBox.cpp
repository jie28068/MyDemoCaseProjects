#include "SearchBox.h"
#include "Manager/ModelingServerMng.h"
#include <QApplication>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QPainter>
#include <QScrollBar>
#include <QStyle>
#include <QStyleOption>

static const int LINEEDIT_WIDTH = 240;         // 搜索框宽度
static const int POPUPWIDGET_HEIGHT = 240;     // 搜索框的弹出框高度
static const int POPUPWIDGET_ITEM_HEIGHT = 26; // 弹出框中项的高度
static const int POPUPWIDGET_MAX_HEIGHT = 180; // 搜索框的弹出框最大高度
static const int SEARCH_MAX_TEXT_LENGTH = 50;  // 搜索框输入文本最大长度
static const int CONTENTWIDGET_MAX_HEIGHT = 210;

SearchBox::SearchBox(QWidget *parent /*= nullptr*/) : QWidget(parent), m_searchLineEdit(nullptr)
{
    hide();
    setWindowFlags(Qt::Widget | Qt::WindowStaysOnTopHint);
    m_searchLineEdit = new SearchLineEdit(this);
    connect(m_searchLineEdit, SIGNAL(displayPopupWidget(int)), this, SLOT(onSetContentWidgetSize(int)));
    connect(m_searchLineEdit, SIGNAL(selectedBlock(searchListData &)), this, SIGNAL(selectedBlock(searchListData &)));
}

SearchBox::~SearchBox() { }

void SearchBox::setVisableStatus(bool visible)
{
    if (visible) {
        show();
        m_searchLineEdit->show();
        m_searchLineEdit->setFocus();
    } else {
        hide();
        m_searchLineEdit->hide();
        m_searchLineEdit->clear();
    }
}

void SearchBox::setModel(PModel model)
{
    if (m_searchLineEdit) {
        m_searchLineEdit->m_model = model;
    }
}

void SearchBox::onSetContentWidgetSize(int itemCount)
{
    if (m_searchLineEdit == nullptr) {
        return;
    }
    int nWidgetHeight = itemCount + m_searchLineEdit->height() + 3;
    if (CONTENTWIDGET_MAX_HEIGHT < nWidgetHeight) {
        nWidgetHeight = CONTENTWIDGET_MAX_HEIGHT;
    }
    setFixedSize(m_searchLineEdit->width(), nWidgetHeight);
}

// 搜索输入框line
SearchLineEdit::SearchLineEdit(QWidget *parent /* = 0 */)
    : KLineEdit(parent), nCurrenPopupItemCount(0), m_model(nullptr)
{
    // hide();
    installEventFilter(this);
    setFixedWidth(LINEEDIT_WIDTH);
    setFixedHeight(21);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);

    QPushButton *iconButton = new QPushButton(this);
    iconButton->setObjectName("iconButton");
    iconButton->setFlat(true);

    mainLayout->addWidget(iconButton);
    mainLayout->addStretch();

    m_pClearButton = new QPushButton(this);
    m_pClearButton->setObjectName("clearButton");
    m_pClearButton->setVisible(false);
    m_pClearButton->installEventFilter(this);

    mainLayout->addWidget(m_pClearButton);

    mainLayout->setContentsMargins(2, 0, 2, 0);
    setTextMargins(15, 0, 20, 0);
    setLayout(mainLayout);

    m_pFilterDisplayPopup = new QListWidget(parent);
    m_pFilterDisplayPopup->setFixedSize(this->width(), POPUPWIDGET_HEIGHT);
    m_pFilterDisplayPopup->move(0, parent->height() - 3);
    m_pFilterDisplayPopup->setVisible(false); // 是否可见

    QScrollBar *verticalScrollBar = m_pFilterDisplayPopup->verticalScrollBar();
    verticalScrollBar->setStyleSheet(QString("QScrollBar:vertical { width: %1px; }").arg(3));

    QObject::connect(m_pClearButton, SIGNAL(pressed()), this, SLOT(onClearSearchText()));
    QObject::connect(this, SIGNAL(textChanged(const QString &)), this, SLOT(onTextChanged(const QString &)));
    // 单击选中QListWidget中的某一项是触发此信号
    connect(m_pFilterDisplayPopup, SIGNAL(itemSelectionChanged()), this, SLOT(onBlockClicked()));
    // 双击或回车选中QListWidget中的某一项是触发此信号
    connect(m_pFilterDisplayPopup, SIGNAL(itemActivated(QListWidgetItem *)), this,
            SLOT(onBlockActivatedClicked(QListWidgetItem *)));

    // refreshBlockDataList();
}

void SearchLineEdit::refreshBlockDataList()
{
    if (!parent())
        return;
    if (m_model) {
        m_BlockDatalist.clear();
        QStringList uuidList;
        uuidList.push_back(m_model->getUUID());
        insertDataListByBoard(m_model, m_BlockDatalist, m_model->getName(), uuidList);
    }
    qSort(m_BlockDatalist.begin(), m_BlockDatalist.end());
}

void SearchLineEdit::insertDataListByBoard(PModel model, QList<searchListData> &dataList, QString boardPath,
                                           QStringList pathUuidList)
{
    if (!model)
        return;
    for each (auto childModel in model->getChildModels()) {
        if (!childModel)
            continue;
        searchListData data;
        data.name = childModel->getName();
        data.uuid = childModel->getUUID();
        data.boardName = boardPath;
        data.boardUuidPath = pathUuidList;
        dataList.append(data);
        QString path = boardPath;
        path += "/";
        path += childModel->getName();
        QStringList pathUuid = pathUuidList;
        pathUuid.push_back(childModel->getUUID());
        insertDataListByBoard(childModel, dataList, path, pathUuid);
    }
}

void SearchLineEdit::focusInEvent(QFocusEvent *e)
{
    refreshBlockDataList();
    onTextChanged(text());
    QLineEdit::focusInEvent(e);
}

bool SearchLineEdit::eventFilter(QObject *obj, QEvent *evt)
{
    if (obj == m_pClearButton) {
        if (evt->type() == QEvent::Enter) {
            setCursor(Qt::ArrowCursor);
        }

        if (evt->type() == QEvent::Leave) {
            setCursor(QCursor(QPixmap(":/GraphicsModelingKernel/cursorInput")));
        }
    }

    if (obj == this) {
        QKeyEvent *keyEvt = dynamic_cast<QKeyEvent *>(evt);
        if (keyEvt != nullptr) {
            if (keyEvt->key() == Qt::Key_Enter || keyEvt->key() == Qt::Key_Return) {
                if (m_pFilterDisplayPopup->item(0) != nullptr) {
                    searchListData data;
                    data.name = m_pFilterDisplayPopup->item(0)->data(Qt::DisplayRole).toString();
                    data.uuid = m_pFilterDisplayPopup->item(0)->data(Qt::UserRole + 1).toString();
                    data.boardName = m_pFilterDisplayPopup->item(0)->data(Qt::UserRole + 2).toString();
                    data.boardUuidPath = m_pFilterDisplayPopup->item(0)->data(Qt::UserRole + 3).toStringList();
                    emit selectedBlock(data);
                    this->setText(data.name);
                }
            }

            if (keyEvt->key() == Qt::Key_Down) {
                m_pFilterDisplayPopup->setFocus();
                m_pFilterDisplayPopup->setCurrentRow(-1);
            }
            keyEvt->accept();
        }

        if (evt->type() == QEvent::FocusOut) {
            if (!m_pFilterDisplayPopup->hasFocus()) {
                onEditFinish();
            }
        }
    }

    return QWidget::eventFilter(obj, evt);
}

void SearchLineEdit::onClearSearchText()
{
    clear();
}

void SearchLineEdit::onTextChanged(const QString &value)
{
    nCurrenPopupItemCount = 0;
    m_pFilterDisplayPopup->setCurrentItem(nullptr);
    m_pFilterDisplayPopup->clear();
    int nTempPos = SEARCH_MAX_TEXT_LENGTH;
    if (value.isEmpty()) {
        m_pClearButton->setVisible(false);
        m_pFilterDisplayPopup->setVisible(false);
        emit selectedBlock(searchListData());
        emit displayPopupWidget(0);
    } else {
        // 不区分大小写搜索
        Qt::CaseSensitivity cs = Qt::CaseInsensitive;
        QList<searchListData> blockNamelist;
        foreach (searchListData data, m_BlockDatalist) {
            if (data.name.contains(value, cs)) {
                // 弹出框中匹配项的前后位置
                // 按输入文本在被匹配出来的字符串中的位置进行排序显示
                QListWidgetItem *pItem = new QListWidgetItem();
                pItem->setData(Qt::DisplayRole, QString("%1(%2)").arg(data.name).arg(data.boardName));
                pItem->setData(Qt::UserRole + 1, data.uuid);
                pItem->setData(Qt::UserRole + 2, data.boardName);
                pItem->setData(Qt::UserRole + 3, data.boardUuidPath);
                int nPos = data.name.indexOf(value, 0, Qt::CaseInsensitive);
                if (nPos <= nTempPos) {
                    nTempPos = nPos;
                    m_pFilterDisplayPopup->insertItem(0, pItem);
                } else {
                    m_pFilterDisplayPopup->addItem(pItem);
                }
                ++nCurrenPopupItemCount;
            }
        }

        m_pClearButton->setVisible(true);
        m_pFilterDisplayPopup->setVisible(true);

        // 根据匹配项数量，设置弹出框高度
        int nPopupHeight = POPUPWIDGET_ITEM_HEIGHT * nCurrenPopupItemCount;
        if (POPUPWIDGET_MAX_HEIGHT < nPopupHeight) {
            nPopupHeight = POPUPWIDGET_MAX_HEIGHT;
        }
        m_pFilterDisplayPopup->setFixedHeight(nPopupHeight);
        emit displayPopupWidget(nPopupHeight);
    }
}

void SearchLineEdit::onEditFinish()
{
    m_pClearButton->setVisible(false);
    m_pFilterDisplayPopup->setVisible(false);
    emit displayPopupWidget(0);
}

void SearchLineEdit::onBlockActivatedClicked(QListWidgetItem *item)
{
    searchListData data;
    data.name = item->data(Qt::DisplayRole).toString();
    data.uuid = item->data(Qt::UserRole + 1).toString();
    data.boardName = item->data(Qt::UserRole + 2).toString();
    data.boardUuidPath = item->data(Qt::UserRole + 3).toStringList();
    emit selectedBlock(data);
    this->setText(data.name);
    onEditFinish();
    emit selectedBlock(data);
}

void SearchLineEdit::onBlockClicked()
{
    if (!m_pFilterDisplayPopup->currentItem())
        return;

    m_pClearButton->setVisible(true);
    m_pFilterDisplayPopup->setVisible(true);
    int nPopupHeight = POPUPWIDGET_ITEM_HEIGHT * nCurrenPopupItemCount;
    if (POPUPWIDGET_MAX_HEIGHT < nPopupHeight) {
        nPopupHeight = POPUPWIDGET_MAX_HEIGHT;
    }
    m_pFilterDisplayPopup->setFixedHeight(nPopupHeight);
    emit displayPopupWidget(nPopupHeight);

    searchListData data;
    data.name = m_pFilterDisplayPopup->currentItem()->data(Qt::DisplayRole).toString();
    data.uuid = m_pFilterDisplayPopup->currentItem()->data(Qt::UserRole + 1).toString();
    data.boardName = m_pFilterDisplayPopup->currentItem()->data(Qt::UserRole + 2).toString();
    data.boardUuidPath = m_pFilterDisplayPopup->currentItem()->data(Qt::UserRole + 3).toStringList();
    emit selectedBlock(data);
}