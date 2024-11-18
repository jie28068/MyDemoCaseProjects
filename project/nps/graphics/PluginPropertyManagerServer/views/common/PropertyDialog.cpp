#include "PropertyDialog.h"
#include "GlobalAssistant.h"
#include "ui_PropertyDialog.h"
#include <QDebug>
#include <QTreeWidgetItem>
#include <algorithm>

PropertyDialog::PropertyDialog(QWidget *parent /*= nullptr*/)
    : QWidget(parent), ui(new Ui::PropertyDialog), m_pWrapper(nullptr)
{
    ui->setupUi(this);
    // 这里设置一个600，0是为了底部信息栏以最小化显示
    // ui->splitter->setSizes(QList<int>() << 600 << 0);
    setWindowFlags(Qt::WindowCloseButtonHint);
    ui->treeWidget->setFont(QFont("Microsoft YaHei", 14));
    ui->treeWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->treeWidget->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->Iconlabel->setAlignment(Qt::AlignCenter);
    connect(ui->treeWidget, SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)), this,
            SLOT(onTabTreeCurrentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)));
}

PropertyDialog::~PropertyDialog(void)
{
    delete ui;
}
void PropertyDialog::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void PropertyDialog::setPropertyWrapper(CommonWrapper *pPropertyWrapper, const QString tabname)
{
    // 解析数据
    m_pWrapper = pPropertyWrapper;
    QObject::connect(m_pWrapper, SIGNAL(viewChanged()), this, SLOT(onViewChanged()));
    refreshPropertyUI(pPropertyWrapper, tabname);
}

CommonWrapper *PropertyDialog::getPropertyWrapper()
{
    return m_pWrapper;
}

void PropertyDialog::clear()
{
    clearWidget();
    m_pWrapper = nullptr;
}

void PropertyDialog::refreshPropertyUI(CommonWrapper *pPropertyWrapper, const QString tabname)
{
    if (pPropertyWrapper == nullptr || ui->stackedWidgetCustomPage == nullptr || ui->treeWidget == nullptr) {
        return;
    }

    setWindowTitle(pPropertyWrapper->getTitle());
    bool bInit = false;
    clearWidget();
    for (QString strTabName : pPropertyWrapper->getTreeTabNameList()) {
        QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidget);
        item->setText(0, strTabName);
        bool curTabHaveWidget = false;
        int tabSize = pPropertyWrapper->getTabWidgetNameList(strTabName).size();
        for (QString tabWidgetName : pPropertyWrapper->getTabWidgetNameList(strTabName)) {
            QWidget *pWidget = pPropertyWrapper->getCustomPropertyWidget(strTabName, tabWidgetName);
            if (pWidget == nullptr) {
                continue;
            }
            if (tabSize <= 1) {
                QTabWidget *tabWidget = new QTabWidget(ui->stackedWidgetCustomPage);
                tabWidget->addTab(pWidget, tabWidgetName);
                ui->stackedWidgetCustomPage->addWidget(tabWidget);
                m_mapCustomTabBrowser.insert(strTabName, tabWidget);
                curTabHaveWidget = true;
            } else if (!m_mapCustomTabBrowser.contains(strTabName)) {
                SwitchWidget *pswitchWidget = new SwitchWidget(ui->stackedWidgetCustomPage);
                pswitchWidget->addTabWidget(tabWidgetName, pWidget);
                pswitchWidget->setCurrentWidget(pWidget);
                ui->stackedWidgetCustomPage->addWidget(pswitchWidget);
                m_mapCustomTabBrowser.insert(strTabName, pswitchWidget);
                curTabHaveWidget = true;
            } else if (m_mapCustomTabBrowser[strTabName] != nullptr) {
                SwitchWidget *pswitchWidget = dynamic_cast<SwitchWidget *>(m_mapCustomTabBrowser[strTabName]);
                if (pswitchWidget != nullptr) {
                    pswitchWidget->addTabWidget(tabWidgetName, pWidget);
                }
                curTabHaveWidget = true;
            }
        }
        if (curTabHaveWidget) {
            if (!bInit) {
                ui->treeWidget->setCurrentItem(item);
                bInit = true;
            }
        } else {
            item->setFlags(Qt::NoItemFlags);
        }
        if (tabname == strTabName && curTabHaveWidget) {
            ui->treeWidget->setCurrentItem(item);
        }
    }
    int maxWidth = 85;
    int offset = 0;
    if (property("isDlgmemoryBlock").toString() != "true") {
        offset = 10;
    }
    ui->treeWidget->setFixedWidth(maxWidth);
    if (ui->treeWidget->model() != nullptr) {
        for (int logicalIndex = 0; logicalIndex < ui->treeWidget->model()->rowCount(); ++logicalIndex) {
            ui->treeWidget->resizeColumnToContents(logicalIndex);
            int rwidth = ui->treeWidget->columnWidth(logicalIndex) + offset;
            if (rwidth > maxWidth) {
                maxWidth = rwidth;
            }
        }
    }
    if (CommonWrapper::LeftWidgetType_Other == pPropertyWrapper->getLeftWidgetType()) {
        ui->iconwidget->hide();
    } else {
        ui->Iconlabel->setPixmap(pPropertyWrapper->getBlockPixmap().scaled(65, 45));
        ui->iconwidget->setFixedWidth(maxWidth);
    }
    if (pPropertyWrapper->getTreeTabNameList().size() <= 1
        && CommonWrapper::LeftWidgetType_Other == pPropertyWrapper->getLeftWidgetType()) {
        ui->leftwidget->hide();
    } else {
        ui->leftwidget->setFixedWidth(maxWidth);
        ui->treeWidget->setFixedWidth(maxWidth);
    }
}

void PropertyDialog::clearWidget()
{
    if (ui->treeWidget != nullptr) {
        ui->treeWidget->clear();
    }
    QMapIterator<QString, QWidget *> iter(m_mapCustomTabBrowser);
    while (iter.hasNext()) {
        iter.next();
        // 设置空指针，dialog销毁不销毁外部传进来的widget
        iter.value()->setParent(nullptr);
        ui->stackedWidgetCustomPage->removeWidget(iter.value());
    }
    m_mapCustomTabBrowser.clear();
}

void PropertyDialog::onViewChanged()
{
    refreshPropertyUI(m_pWrapper);
}

void PropertyDialog::onTabTreeCurrentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    if (current == nullptr) {
        return;
    }

    QString strTabName = current->text(0);

    if (m_mapCustomTabBrowser.contains(strTabName) && m_mapCustomTabBrowser[strTabName] != nullptr) {
        ui->stackedWidgetCustomPage->setCurrentWidget(m_mapCustomTabBrowser[strTabName]);
    }
}

SwitchWidget::SwitchWidget(QWidget *parent) : CWidget(parent), m_tabComboBox(nullptr), m_stackWidget(nullptr)
{
    m_widgetMap.clear();
    m_tabOrderList.clear();
    InitUI();
}

SwitchWidget::~SwitchWidget() { }

void SwitchWidget::addTabWidget(const QString &tabName, QWidget *pwidget)
{
    if (!isWidgetValid()) {
        return;
    }
    if (!m_tabOrderList.contains(tabName)) {
        m_tabOrderList.append(tabName);
        qSort(m_tabOrderList.begin(), m_tabOrderList.end(), [](const QString &plhs, const QString &prhs) -> bool {
            QStringList lList = plhs.split("/");
            QStringList rList = prhs.split("/");
            if (lList.size() == rList.size()) {
                return NPS::compareString(plhs, prhs);
            } else {
                return lList.size() < rList.size();
            }
        });
        QString curText = m_tabComboBox->currentText();
        m_tabComboBox->clear();
        m_tabComboBox->addItems(m_tabOrderList);
        m_tabComboBox->setCurrentText(curText);
    }
    m_widgetMap.insert(tabName, pwidget);
    m_stackWidget->addWidget(pwidget);
}

void SwitchWidget::setCurrentWidget(QWidget *pwidget)
{
    if (!isWidgetValid()) {
        return;
    }
    m_stackWidget->setCurrentWidget(pwidget);
    for (QString tabName : m_widgetMap.keys()) {
        if (m_widgetMap[tabName] != nullptr && m_widgetMap[tabName] == pwidget) {
            m_tabComboBox->setCurrentText(tabName);
            break;
        }
    }
}

void SwitchWidget::onCurrentTextChanged(const QString &textstr)
{
    if (!isWidgetValid()) {
        return;
    }
    if (m_widgetMap.contains(textstr)) {
        m_stackWidget->setCurrentWidget(m_widgetMap[textstr]);
    }
}

void SwitchWidget::InitUI()
{
    QGridLayout *mainLayout = new QGridLayout(this);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(10);
    QWidget *boxWidget = new QWidget(this);
    boxWidget->setObjectName("SwitchBoxWidget");
    QHBoxLayout *hlayout = new QHBoxLayout(boxWidget);
    hlayout->setMargin(0);
    m_tabComboBox = new QComboBox(this);
    m_tabComboBox->setObjectName("SwitchWidgetTabBox");
    m_stackWidget = new QStackedWidget(this);
    hlayout->addWidget(m_tabComboBox);
    QSpacerItem *hSpacer = new QSpacerItem(10, 33, QSizePolicy::Expanding, QSizePolicy::Minimum);
    hlayout->addSpacerItem(hSpacer);
    mainLayout->addWidget(boxWidget, 0, 0, 1, 1);
    mainLayout->addWidget(m_stackWidget, 1, 0, 1, 1);
    setLayout(mainLayout);
    connect(m_tabComboBox, &QComboBox::currentTextChanged, this, &SwitchWidget::onCurrentTextChanged,
            Qt::UniqueConnection);
}

bool SwitchWidget::isWidgetValid()
{
    if (m_stackWidget != nullptr && m_tabComboBox != nullptr) {
        return true;
    }
    return false;
}
