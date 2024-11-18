#include "MyToolBox.h"
#include "ComponentListDelegate.h"
#include "ContextMenu.h"
#include "MyListView.h"
#include "search_line_edit.h"
#include <QDebug>
#include <QFileInfo>
#include <QScrollBar>
#include <QStandardItem>

using namespace Global;

MyToolBox::MyToolBox(QWidget *parent)
    : QWidget(parent), m_pContentVBoxLayout(nullptr), m_pLastSelected(nullptr), m_pModel(nullptr)
{
    ui.setupUi(this);
    ui.gridLayout->setContentsMargins(0, 0, 0, 0);
    m_pContentVBoxLayout = new QVBoxLayout();
    m_pContentVBoxLayout->setContentsMargins(0, 0, 2, 0);
    m_pContentVBoxLayout->setSpacing(8);

    /*SearchLineEdit* serarchLineEdit = new SearchLineEdit(this);
    m_pContentVBoxLayout->addWidget(serarchLineEdit);*/

    QVBoxLayout *pLayout = new QVBoxLayout();
    pLayout->setContentsMargins(0, 0, 0, 0);
    pLayout->addLayout(m_pContentVBoxLayout);
    pLayout->addStretch(1);

    // ui.searchLineEdit->setFixedHeight(25);

    ui.scrollAreaWidgetContents->setLayout(pLayout);
    ui.scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    ui.scrollArea->verticalScrollBar()->setFixedWidth(8);

    QFile file(COMPOENTVIEW_QSS);
    QString styleSheet;
    if (file.open(QFile::ReadOnly)) {
        styleSheet = QLatin1String(file.readAll());
        ui.scrollArea->setStyleSheet(styleSheet);
        ui.searchLineEdit->setStyleSheet(styleSheet);
    }
    file.close();

    connect(ui.searchLineEdit, SIGNAL(textChanged(QString)), this, SLOT(onFilter(QString)));
    // connect(serarchLineEdit,SIGNAL(textChanged(QString)), this, SLOT(onFilter(QString)));
    connect(ui.scrollAreaWidgetContents, SIGNAL(scrollWidgetSizeChanged(int)), this,
            SLOT(onScrollWidgetSizeChanged(int)));
}

ToolPage *MyToolBox::addGroup(Category category, const QString &strGroupName, const bool &isExpand)
{
    if (m_mapGroupPages.contains(strGroupName)) {
        return m_mapGroupPages[strGroupName];
    }

    ToolPage *page = new ToolPage(category, strGroupName, this, isExpand);
    m_pContentVBoxLayout->addWidget(page);
    m_mapGroupPages.insert(strGroupName, page);
    connect(page->m_pListView, SIGNAL(pressed(QModelIndex)), this, SLOT(onListViewClicked(QModelIndex)));
    connect(page->m_pListView, SIGNAL(leftMouseButtonDoubleClicked(QString)), this,
            SIGNAL(leftMouseButtonDoubleClicked(QString)));
    return page;
}

void MyToolBox::removeAllGroup()
{
    QList<QString> grouplist = m_mapGroupPages.keys();
    foreach (QString groupname, grouplist) {
        removeGroup(groupname);
    }
    update();
}

void MyToolBox::removeGroup(const QString &strGroupName)
{
    if (!m_mapGroupPages.contains(strGroupName)) {
        return;
    }
    disconnect(m_mapGroupPages[strGroupName]->m_pListView, SIGNAL(pressed(QModelIndex)), this,
               SLOT(onListViewClicked(QModelIndex)));
    disconnect(m_mapGroupPages[strGroupName]->m_pListView, SIGNAL(leftMouseButtonDoubleClicked(QString)), this,
               SIGNAL(leftMouseButtonDoubleClicked(QString)));
    QWidget *toolpage = m_mapGroupPages[strGroupName];
    m_pContentVBoxLayout->removeWidget(toolpage);
    delete toolpage;
    toolpage = nullptr;
    m_mapGroupPages.remove(strGroupName);
}

void MyToolBox::setDragEnable(bool bEnable)
{
    QMapIterator<QString, ToolPage *> iter(m_mapGroupPages);
    while (iter.hasNext()) {
        iter.next();
        iter.value()->m_pListView->setDragEnabled(bEnable);
        iter.value()->m_pProxyModel->invalidate();
    }
}

void MyToolBox::invalidatePage(const QString &strGroupName /*= QString()*/)
{
    if (strGroupName.isEmpty()) {
        QMapIterator<QString, ToolPage *> iter(m_mapGroupPages);
        while (iter.hasNext()) {
            iter.next();
            iter.value()->m_pProxyModel->invalidate();
        }
    } else if (m_mapGroupPages.contains(strGroupName)) {
        m_mapGroupPages[strGroupName]->m_pProxyModel->invalidate();
    }
}

void MyToolBox::expandAll()
{
    QMapIterator<QString, ToolPage *> iter(m_mapGroupPages);
    while (iter.hasNext()) {
        iter.next();
        iter.value()->expand();
    }
}

void MyToolBox::collapseAll()
{
    QMapIterator<QString, ToolPage *> iter(m_mapGroupPages);
    while (iter.hasNext()) {
        iter.next();
        iter.value()->collapse();
    }
}

void MyToolBox::onListViewClicked(const QModelIndex &index)
{
    Q_ASSERT(index.isValid());
    if (m_pLastSelected != sender()) {
        if (m_pLastSelected != nullptr) {
            m_pLastSelected->setCurrentIndex(QModelIndex());
        }
        m_pLastSelected = dynamic_cast<MyListView *>(sender());
    }
}

void MyToolBox::onFilter(const QString &strFilter)
{
    QMapIterator<QString, ToolPage *> iter(m_mapGroupPages);
    while (iter.hasNext()) {
        iter.next();
        iter.value()->m_pProxyModel->setFilterString(strFilter);
        iter.value()->expand();
    }
    if (strFilter.isEmpty()) {
        expandAll();
    }
}

void MyToolBox::onScrollWidgetSizeChanged(int height)
{
    if (ui.scrollArea->verticalScrollBar() == nullptr) {
        return;
    }
    if (height > this->height()) {
        ui.scrollArea->verticalScrollBar()->setMaximumHeight(this->height());
    } else {
        ui.scrollArea->verticalScrollBar()->setMaximumHeight(0);
    }
}

void MyToolBox::paintEvent(QPaintEvent *e)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    QWidget::paintEvent(e);
}

void MyToolBox::resizeEvent(QResizeEvent *e)
{
    QWidget::resizeEvent(e);
    onScrollWidgetSizeChanged(ui.scrollAreaWidgetContents->height());
}

void MyToolBox::setSelectedPtr()
{
    m_pLastSelected = nullptr;
}

//////////////////////////////////////////////////////////////////////////

ToolPage::ToolPage(Global::Category category, const QString &strGroupName, MyToolBox *parent /*= nullptr*/,
                   const bool &isExpand)
    : QWidget(parent), m_category(category), m_strGroupName(strGroupName), m_parent(parent)
{
    Q_ASSERT(!m_strGroupName.isEmpty());

    m_pButton = new PageButton(m_strGroupName, this, isExpand);
    // m_pButton->setExpand(isExpand);
    connect(m_pButton, SIGNAL(expandChanged()), SLOT(expandChanged()));
    connect(m_pButton, SIGNAL(clicked()), this, SLOT(onBottonFoldClicked()));

    m_pProxyModel = new ComponentProxyModel(this);
    m_pProxyModel->setFilterGroupName(m_strGroupName); // 筛选
    // m_pProxyModel->sort(0); //加入顺序，不用自动排序
    m_pListView = new MyListView(m_strGroupName, this);

    QFile file(COMPOENTVIEW_QSS);
    QString styleSheet;
    if (file.open(QFile::ReadOnly)) {
        styleSheet = QLatin1String(file.readAll());
        m_pListView->setStyleSheet(styleSheet);
    }
    file.close();

    m_pListView->setModel(m_pProxyModel);
    if (Controller == m_category) {
        m_pListView->setSpacing(4);
        m_pListView->setItemDelegate(new ComponentListDelegate(m_pListView));
    } else {
        m_pListView->setSpacing(4);
        m_pListView->setItemDelegate(new ComponentListIconDelegate(m_pListView));
    }
    m_mapGroupRoleTempalte[Global::GroupNameRole] = m_strGroupName;

    m_pLayout = new QVBoxLayout(this);
    m_pLayout->setContentsMargins(0, 0, 0, 0);
    m_pLayout->addWidget(m_pButton);
    m_pLayout->addWidget(m_pListView);
    m_pLayout->setSpacing(0);

    setLayout(m_pLayout);
    setAcceptDrops(true);
    setAttribute(Qt::WA_StyledBackground);

    connect(m_pProxyModel, SIGNAL(rowsInserted(QModelIndex, int, int)), this,
            SLOT(onModelRowInserted(QModelIndex, int, int)));

    if (!isExpand) {
        m_pListView->hide();
    } else {
        m_pListView->show();
    }
}

void ToolPage::setModel(QAbstractItemModel *pModel)
{
    if (pModel == m_pProxyModel->sourceModel()) {
        m_pProxyModel->invalidate();
        m_pProxyModel->sort(0, Qt::AscendingOrder);
    } else {
        m_pProxyModel->setSourceModel(pModel);
        m_pProxyModel->sort(0, Qt::AscendingOrder);
    }
}

void ToolPage::setData(int nRole, const QVariant &value)
{
    m_mapGroupRoleTempalte[nRole] = value;
}

void ToolPage::expand()
{
    m_pListView->show();
    m_pButton->setExpand(true);
}

void ToolPage::collapse()
{
    m_pListView->hide();
    m_pButton->setExpand(false);
}

void ToolPage::expandChanged()
{
    emit isExpandChanged(m_strGroupName, m_pButton->isExpand());
}

void ToolPage::contextMenuEvent(QContextMenuEvent *e)
{

    ContextMenuManager *pContextMenuManager = ContextMenuManager::instance(nameFromCategory(m_category));
    if (pContextMenuManager == nullptr) {
        QWidget::contextMenuEvent(e);
        return;
    }

    auto pMenu = pContextMenuManager->createMenu();

    if (m_category == Controller) {
        if (QObject::tr(m_strGroupName.toUtf8()) == ControllerGroupCustomCode) {
            pMenu->setAddCodeEnable();
#ifdef _DEBUG
            pMenu->setImportCodeEnable();
#endif
            // pMenu->setSwitchViewEnable();
        } else if (QObject::tr(m_strGroupName.toUtf8()) == ControllerGroupCustomBoard) {
        }
        //  else if (m_mapGroupRoleTempalte.contains(BuildInRole) && m_mapGroupRoleTempalte[BuildInRole].toBool()) {
        // #ifdef COMPILER_SUPPORT_BUILTIN_MODIFY
        //             pMenu->setAddBuildInEnable(m_mapGroupRoleTempalte[ControlClassificationRole].toInt());
        // #endif
        //         }
        //     } else if (m_category == Device) {
        // #ifdef COMPILER_SUPPORT_BUILTIN_MODIFY
        //         pMenu->setAddElcBuildInEnable(m_mapGroupRoleTempalte[DeviceClassificationRole].toInt());
        // #endif
    }

    pMenu->addSeparator();
    pMenu->setRefreshViewEnable();
    // pMenu->setSwitchViewEnable();
    pMenu->addSeparator();
    pMenu->setExpandAllEnable();
    pMenu->setCollapseAllEnable();
    // #ifdef COMPILER_SUPPORT_BUILTIN_MODIFY
    //     pMenu->setAdjustListViewEnable();
    // #endif
    pMenu->setObjectName("uniformStyleMenu_cV");
    pMenu->exec(mapToGlobal(e->pos()));
    e->accept();
}

void ToolPage::dragEnterEvent(QDragEnterEvent *e)
{
    if (!m_mapGroupRoleTempalte.contains(ControlClassificationRole)) {
        return;
    }
    if (m_mapGroupRoleTempalte[ControlClassificationRole].toInt() > Other) {
        return;
    }

    if (e->possibleActions().testFlag(Qt::MoveAction) && e->mimeData()->hasFormat(DRAG_MIME_DATA_CUSTOM_NAME)) {
        QString strTypeName, strGroupName;
        QByteArray byCustomData = e->mimeData()->data(DRAG_MIME_DATA_CUSTOM_NAME);
        QDataStream dataStream(&byCustomData, QIODevice::ReadOnly);
        dataStream >> strTypeName >> strGroupName;

        // 同组内不接受拖拽
        if (strGroupName != m_strGroupName) {
            e->acceptProposedAction();
        }
    } else {
        QWidget::dragEnterEvent(e);
    }
}

void ToolPage::dragMoveEvent(QDragMoveEvent *e)
{
    QWidget::dragMoveEvent(e);
}

void ToolPage::dropEvent(QDropEvent *e)
{
    BaseListModel *pModel = dynamic_cast<BaseListModel *>(m_pProxyModel->sourceModel());
    if (pModel == nullptr) {
        return;
    }

    QString strOldGroupName = pModel->dropMimeDataCustom(e->mimeData(), m_mapGroupRoleTempalte);
    if (strOldGroupName.isEmpty()) {
        return;
    }

    m_pProxyModel->invalidate();
    m_parent->invalidatePage(strOldGroupName); // 刷新item原来的组，否则视图不会更新
    if (!m_pListView->isVisible() && m_pProxyModel->rowCount() == 1) {
        // 往空的ListView中添加模块后展开视图
        expand();
    }
    m_pListView->update();

    e->setDropAction(Qt::MoveAction);
    e->accept();
}

void ToolPage::onBottonFoldClicked()
{
    m_pListView->setVisible(m_pButton->isExpand());
}

void ToolPage::onModelRowInserted(const QModelIndex &parent, int start, int end)
{
    if (!m_pListView->isVisible() && m_pProxyModel->rowCount() == 1) {
        // 往空的ListView中添加模块后展开视图
        expand();
    }
}

//////////////////////////////////////////////////////////////////////////
// static const QRgb COLOR = qRgb(0, 61, 116);
static const QRgb COLOR = qRgb(235, 243, 254);

PageButton::PageButton(const QString &strText, QWidget *parent /*= nullptr*/, const bool &isExpand)
    : QWidget(parent), m_strText(strText), m_bExpand(isExpand), m_bPressed(false)
{
    setAutoFillBackground(true);
    setMinimumHeight(30);
    QPalette palette;
    palette.setColor(QPalette::Background, COLOR);
    setPalette(palette);
}

void PageButton::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        m_bPressed = true;
        update();
    }
}

void PageButton::mouseReleaseEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        m_bPressed = false;
        m_bExpand = !m_bExpand;
        emit expandChanged();
        update();
        emit clicked();
    }
}

void PageButton::enterEvent(QEvent *)
{
    QPalette palette;
    palette.setColor(QPalette::Background, QColor(COLOR).lighter());
    setPalette(palette);
}

void PageButton::leaveEvent(QEvent *)
{
    QPalette palette;
    palette.setColor(QPalette::Background, COLOR);
    setPalette(palette);
}

void PageButton::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);

    painter.fillRect(e->rect(), QColor(235, 243, 254));
    QPen pen;
    // 文本
    QColor presscolor("#036EB7");  // 压下色
    QColor normalcolor("#005A92"); // 正常色
    pen.setWidth(1);
    pen.setColor(m_bPressed ? presscolor : normalcolor);
    painter.setPen(pen);
    QFont font("Microsoft YaHei");
    font.setBold(true);
    font.setPixelSize(14);
    painter.setFont(font);
    m_strText = QObject::tr(m_strText.toUtf8());
    int textW = QFontMetrics(font).width(m_strText);
    int textH = QFontMetrics(font).height();
    QRect drawRect(10, (height() - textH) / 2, textW + 1, textH + 1);
    painter.drawText(drawRect, m_strText);

    // 指示器
    QPainterPath path;
    int width = e->rect().width();
    if (m_bExpand) {
        /*QPoint pos1(width-6,10);
        QPoint pos2(width-18,10);
        QPoint pos3(width-12,20);*/
        QPoint pos1(width - 10, 12);
        QPoint pos2(width - 20, 12);
        QPoint pos3(width - 15, 17);
        path.moveTo(pos1);
        path.lineTo(pos2);
        path.lineTo(pos3);
        path.lineTo(pos1);
        painter.fillPath(path, m_bPressed ? presscolor : normalcolor);
    } else {
        QPoint pos1(width - 15, 12);
        QPoint pos2(width - 20, 17);
        QPoint pos3(width - 10, 17);
        path.moveTo(pos1);
        path.lineTo(pos2);
        path.lineTo(pos3);
        path.lineTo(pos1);
        painter.fillPath(path, m_bPressed ? presscolor : normalcolor);
    }

    pen.setColor(QColor(206, 206, 206));
    pen.setWidth(2);
    painter.setPen(pen);
    painter.drawRect(e->rect());
}

QSize PageButton::sizeHint() const
{
    return QSize(200, 30);
}

void PageButton::setExpand(bool bExpand)
{
    m_bExpand = bExpand;
    emit expandChanged();
    update();
}
bool PageButton::isExpand() const
{
    return m_bExpand;
}