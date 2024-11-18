#include "GraphicsInputModelEdit.h"
#include "BuriedData.h"
#include "SimulationListItemDelegate.h"
#include "SimulationModelListView.h"

#include <QApplication>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QPainter>
#include <QStyle>
#include <QStyleOption>

static const int POPUPWIDGET_ITEM_HEIGHT = 23; // 弹出框中项的高度
static const int POPUPWIDGET_MAX_HEIGHT = 180; // 搜索框的弹出框最大高度
static const int SEARCH_MAX_TEXT_LENGTH = 50;  // 搜索框输入文本最大长度
static const int CONTENTWIDGET_MAX_HEIGHT = 210;
static const int SEARCH_EDT_MAX_LENGTH = 500; // 搜索框最大长度

CreateModelEditBox::CreateModelEditBox(QWidget *parent /*= nullptr*/)
    : QWidget(parent), m_searchLineEdit(nullptr), isActived(false)
{
    hide();
    setWindowFlags(Qt::Widget | Qt::WindowStaysOnTopHint);
    m_searchLineEdit = new CreateModelEdit(this);

    // installEventFilter(this);
    m_searchLineEdit->installEventFilter(this);
    m_searchLineEdit->m_pClearButton->installEventFilter(this);

    initialSize = QSize(m_searchLineEdit->size());

    connect(m_searchLineEdit, SIGNAL(displayPopupWidget(int)), this, SLOT(onSetContentWidgetSize(int)));
    connect(m_searchLineEdit, SIGNAL(createBlock(QString, QString)), this, SIGNAL(createBlock(QString, QString)));
}

CreateModelEditBox::~CreateModelEditBox() { }

void CreateModelEditBox::setBlockList(const QString &canvasTypeString, QList<BlockStruct> &blocList)
{
    BuriedData &buriedData = BuriedData::getInstance();
    // 设置模型的使用量，会根据该值进行排序，使用的越多的模型，在下拉列表的上面，方便选择
    for (auto &block : blocList) {
        block.usageCount =
                buriedData.getSourceUsageCount(canvasTypeString.toStdString(), block.prototypeName.toStdString());
    }
    m_searchLineEdit->blockList = blocList;
}

void CreateModelEditBox::setStatus(bool visible, bool actived)
{
    isActived = actived;
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

bool CreateModelEditBox::isActive()
{
    return isActived;
}

void CreateModelEditBox::onSetContentWidgetSize(int itemCount)
{
    if (m_searchLineEdit == nullptr) {
        return;
    }
    int nWidgetHeight = itemCount + m_searchLineEdit->height();
    if (CONTENTWIDGET_MAX_HEIGHT < nWidgetHeight) {
        nWidgetHeight = CONTENTWIDGET_MAX_HEIGHT;
    }
    setFixedSize(m_searchLineEdit->getPopWidth(), nWidgetHeight + 1);
}

bool CreateModelEditBox::eventFilter(QObject *obj, QEvent *evt)
{
    if (obj == m_searchLineEdit->m_pClearButton) {
        if (evt->type() == QEvent::Enter) {
            m_searchLineEdit->setCursor(Qt::ArrowCursor);
        }

        if (evt->type() == QEvent::Leave) {
            m_searchLineEdit->setCursor(QCursor(QPixmap(":/GraphicsModelingKernel/cursorInput")));
        }
    }

    if (obj == m_searchLineEdit) {
        QKeyEvent *keyEvt = dynamic_cast<QKeyEvent *>(evt);
        if (keyEvt != nullptr) {
            if (keyEvt->key() == Qt::Key_Enter || keyEvt->key() == Qt::Key_Return) {
                if (m_searchLineEdit->m_pFilterDisplayPopup->item(0) != nullptr) {
                    QListWidgetItem *item = m_searchLineEdit->m_pFilterDisplayPopup->currentItem();
                    QString protypeName = item->data(Qt::UserRole + 1).toString();
                    QString text = item->data(Qt::UserRole + 2).toString();
                    m_searchLineEdit->m_pFilterDisplayPopup->setCurrentItem(nullptr);
                    m_searchLineEdit->m_pFilterDisplayPopup->clear();
                    m_searchLineEdit->m_pFilterDisplayPopup->setVisible(false);
                    onSetContentWidgetSize(0);
                    emit createBlock(protypeName, text);
                }
            }

            if (keyEvt->key() == Qt::Key_Down) {
                m_searchLineEdit->m_pFilterDisplayPopup->setFocus();
                m_searchLineEdit->m_pFilterDisplayPopup->setCurrentRow(0);
            }
            keyEvt->accept();
        }

        if (evt->type() == QEvent::FocusOut) {
            if (!m_searchLineEdit->m_pFilterDisplayPopup->hasFocus()) {
                m_searchLineEdit->onEditFinish();
            }
        }
    }

    return QWidget::eventFilter(obj, evt);
}

QSizeF CreateModelEditBox::getInitialSize()
{
    return initialSize;
}

CreateModelEdit *CreateModelEditBox::searchLineEdit()
{
    return m_searchLineEdit;
}

// 搜索输入框line
CreateModelEdit::CreateModelEdit(QWidget *parent) : QLineEdit(parent), nCurrenPopupItemCount(0)
{
    hide();

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

    mainLayout->addWidget(m_pClearButton);

    mainLayout->setContentsMargins(2, 0, 2, 0);
    setTextMargins(15, 0, 20, 0);
    setLayout(mainLayout);

    setFixedWidth(sizeHint().width());

    m_pFilterDisplayPopup = new SimulationModelListView(parent);
    m_pFilterDisplayPopup->setFixedSize(this->width(), 175);
    m_pFilterDisplayPopup->move(0, parent->height() + 1);
    m_pFilterDisplayPopup->setVisible(false); // 是否可见
    m_pFilterDisplayPopup->setItemDelegate(new SimulationListItemDelegate(m_pFilterDisplayPopup));
    // 隐藏水平滚动条
    m_pFilterDisplayPopup->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    connect(m_pClearButton, SIGNAL(pressed()), this, SLOT(onClearSearchText()));
    connect(this, SIGNAL(textChanged(const QString &)), this, SLOT(onTextChanged(const QString &)));

    connect(m_pFilterDisplayPopup, SIGNAL(itemClicked(QListWidgetItem *)), this,
            SLOT(onBlockSelected(QListWidgetItem *)));
    // 双击或回车选中QListWidget中的某一项是触发此信号
    connect(m_pFilterDisplayPopup, SIGNAL(itemActivated(QListWidgetItem *)), this,
            SLOT(onBlockSelected(QListWidgetItem *)));

    setCursor(QCursor(QPixmap(":/GraphicsModelingKernel/cursorInput")));
}

QSize CreateModelEdit::sizeHint() const
{
    return QSize(qMin(fontMetrics().width(text()) + 56, SEARCH_EDT_MAX_LENGTH), this->height());
}

void CreateModelEdit::onClearSearchText()
{
    clear();
}

void CreateModelEdit::onTextChanged(const QString &value)
{
    m_pFilterDisplayPopup->setCurrentItem(nullptr);
    m_pFilterDisplayPopup->clear();
    int nTempPos = SEARCH_MAX_TEXT_LENGTH;
    if (value.isEmpty()) {
        m_pClearButton->setVisible(false);
        m_pFilterDisplayPopup->setVisible(false);
        setFixedSize(sizeHint());
        emit selectedBlockTypeName(QString());
        emit displayPopupWidget(0);
    } else {
        // 不区分大小写搜索
        Qt::CaseSensitivity cs = Qt::CaseInsensitive;
        // QStringList blockNamelist;
        int fixWidth = 0;
        // 搜索匹配的时候去掉前后空格
        QString noSpaceValue = value;
        noSpaceValue = noSpaceValue.trimmed();
        QList<BlockStruct> usagedBlocks;
        QList<BlockStruct> unusagedBlocks;

        if (!noSpaceValue.isEmpty()) {
            foreach (BlockStruct block, blockList) {
                if (block.prototypeName.contains(noSpaceValue, cs)
                    || block.prototypeChsName.contains(noSpaceValue, cs)) {
                    if (block.usageCount == 0) {
                        unusagedBlocks.append(block);
                    } else {
                        usagedBlocks.append(block);
                    }
                }
            }
        }

        qSort(usagedBlocks.begin(), usagedBlocks.end(), [](const BlockStruct &block1, const BlockStruct &block2) {
            return block1.usageCount > block2.usageCount;
        });
        usagedBlocks = usagedBlocks + unusagedBlocks;

        foreach (BlockStruct block, usagedBlocks) {
            QString text = block.prototypeReadableName.isEmpty() ? block.prototypeName : block.prototypeReadableName;
            if (!block.prototypeChsName.isEmpty()) {
                text += QString("  (%2)").arg(block.prototypeChsName);
            }
            fixWidth = qMax(fixWidth, fontMetrics().width(text) + 20);
            // 弹出框中匹配项的前后位置
            // 按输入文本在被匹配出来的字符串中的位置进行排序显示
            QListWidgetItem *pItem = new QListWidgetItem();
            pItem->setData(Qt::DisplayRole, text);
            pItem->setData(Qt::UserRole + 1, block.prototypeName);
            pItem->setData(Qt::UserRole + 2, noSpaceValue);

            // int nPos = -1;
            // if (block.prototypeName.contains(noSpaceValue, cs)) {
            //     nPos = QString(block.prototypeName).indexOf(noSpaceValue, 0, Qt::CaseInsensitive);
            // } else {
            //     nPos = QString(block.prototypeChsName).indexOf(noSpaceValue, 0, Qt::CaseInsensitive);
            // }
            // if (nPos <= nTempPos) {
            //     nTempPos = nPos;
            //     m_pFilterDisplayPopup->insertItem(0, pItem);
            // } else {
            //     m_pFilterDisplayPopup->addItem(pItem);
            // }

            m_pFilterDisplayPopup->addItem(pItem);
        }

        // 添加文本
        QListWidgetItem *pItem = new QListWidgetItem();
        QString textBlockString = tr("Create text annotations: %1").arg(value); // 创建文本注解： %1
        pItem->setData(Qt::DisplayRole, textBlockString);
        pItem->setData(Qt::UserRole + 1, "text");
        pItem->setData(Qt::UserRole + 2, value);
        m_pFilterDisplayPopup->addItem(pItem);
        fixWidth = qMax(fixWidth, fontMetrics().width(textBlockString) + 20);
        // 设定最大长度
        fixWidth = qMin(fixWidth, SEARCH_EDT_MAX_LENGTH);
        setFixedSize(QSize(fixWidth, this->height()));

        nCurrenPopupItemCount = m_pFilterDisplayPopup->count();

        m_pClearButton->setVisible(true);
        m_pFilterDisplayPopup->setVisible(true);

        m_pFilterDisplayPopup->setCurrentRow(0);

        // 设置宽度
        m_pFilterDisplayPopup->setFixedWidth(fixWidth);
        // 根据匹配项数量，设置弹出框高度
        int nPopupHeight = POPUPWIDGET_ITEM_HEIGHT * nCurrenPopupItemCount;
        if (POPUPWIDGET_MAX_HEIGHT < nPopupHeight) {
            nPopupHeight = POPUPWIDGET_MAX_HEIGHT;
        }
        m_pFilterDisplayPopup->setFixedHeight(nPopupHeight);
        emit displayPopupWidget(nPopupHeight);
    }
}

void CreateModelEdit::onEditFinish()
{
    m_pClearButton->setVisible(false);
    m_pFilterDisplayPopup->setVisible(false);
    emit displayPopupWidget(0);
}

void CreateModelEdit::onBlockSelected(QListWidgetItem *item)
{
    QString protypeName = item->data(Qt::UserRole + 1).toString();
    QString text = item->data(Qt::UserRole + 2).toString();
    m_pFilterDisplayPopup->setCurrentItem(nullptr);
    m_pFilterDisplayPopup->clear();
    m_pFilterDisplayPopup->setVisible(false);
    emit displayPopupWidget(0);
    emit createBlock(protypeName, text);
}

int CreateModelEdit::getPopWidth()
{
    return m_pFilterDisplayPopup->width();
}