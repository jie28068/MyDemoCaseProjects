#include "customtablewidget.h"

#include "GlobalAssistant.h"
#include "KLWidgets/KColorDialog.h"
#include "PropertyServerMng.h"
#include <QFileDialog>
#include <QGridLayout>

USE_LOGOUT_("CustomTableWidget")

CustomTableWidget::CustomTableWidget(const QList<CustomModelItem> &listdata, QWidget *parent)
    : CWidget(parent), m_tableView(nullptr), m_customTableModel(nullptr)
{
    initUI();
    setModelData(listdata);
}

CustomTableWidget::CustomTableWidget(QWidget *parent)
    : CWidget(parent), m_tableView(nullptr), m_customTableModel(nullptr)
{
    initUI();
}

CustomTableWidget::~CustomTableWidget() { }

QList<CustomModelItem> CustomTableWidget::getNewListData()
{
    if (m_customTableModel != nullptr) {
        return m_customTableModel->getNewListData();
    }
    return QList<CustomModelItem>();
}

void CustomTableWidget::setListData(const QList<CustomModelItem> &listdata, const QStringList headerlist)
{
    setModelData(listdata, headerlist);
}

void CustomTableWidget::setItemData(const QString &keyword, const CustomModelItem &item, bool bsendSignal)
{
    if (m_customTableModel != nullptr) {
        m_customTableModel->updateValue(keyword, item, bsendSignal);
    }
}

CustomModelItem CustomTableWidget::getItemData(const QString &keyword, bool bnew)
{
    if (m_customTableModel != nullptr) {
        return m_customTableModel->getModelItem(keyword, bnew);
    }
    return CustomModelItem();
}

bool CustomTableWidget::checkValueChanged()
{
    if (m_customTableModel != nullptr) {
        return m_customTableModel->checkValueChange();
    }
    return false;
}

void CustomTableWidget::setCWidgetReadOnly(bool bReadOnly)
{
    if (m_customTableModel != nullptr) {
        m_customTableModel->setModelDataReadOnly(bReadOnly);
    }
}

bool CustomTableWidget::checkLegitimacy(QString &errorinfo)
{
    errorinfo = "";
    if (m_customTableModel != nullptr) {
        return m_customTableModel->checkValueLegitimacy(errorinfo);
    }
    return false;
}

void CustomTableWidget::setVerticalHeaderVisible(bool bvisible)
{
    if (m_tableView != nullptr && m_tableView->horizontalHeader() != nullptr) {
        m_tableView->horizontalHeader()->setVisible(false);
    }
}

void CustomTableWidget::onTableItemDoubleClicked(const QModelIndex &index)
{
    if (!index.isValid() || m_customTableModel == nullptr || !index.data(NPS::ModelDataEditRole).toBool()) {
        return;
    }
    if (RoleDataDefinition::ControlTypeColor == index.data(NPS::ModelDataTypeRole).toString()) {
        KColorDialog dlg(tr("Select Color"), this);
        QColor curColor;
        QString keyword = index.data(NPS::ModelDataKeywordsRole).toString();
        QString strColor = index.data(PropertyTableModel::CustomRole_ColorRole).toString();
        curColor.setNamedColor(strColor);
        dlg.setCurrentColor(curColor);
        if (dlg.exec() == KColorDialog::Ok) {
            QColor color = dlg.currentColor();
            m_customTableModel->updateValue(keyword, color.name());
        }
    } else if (RoleDataDefinition::ControlTypeFileSelect == index.data(NPS::ModelDataTypeRole).toString()) {
        QString pixpath = QFileDialog::getOpenFileName(
                this, tr("Select File"), "",
                QString("%1(*.txt *.csv);;%2(*)").arg(tr("Common Files")).arg(tr("All Files")));
        if (pixpath.isEmpty()) {
            return;
        }
        m_customTableModel->updateValue(index.data(NPS::ModelDataKeywordsRole).toString(), pixpath);
    } else if (RoleDataDefinition::ControlTypeFont == index.data(NPS::ModelDataTypeRole).toString()) {
        QFont oldFont = index.data(Qt::FontRole).value<QFont>();
        QFont newFont = PropertyServerMng::getInstance().propertyServer->ShowFontSetDialog(oldFont);
        if (newFont == oldFont) {
            return;
        }
        m_customTableModel->updateValue(index.data(NPS::ModelDataKeywordsRole).toString(), newFont);
    }
}

void CustomTableWidget::initUI()
{
    QGridLayout *pGridLayout = new QGridLayout(this);
    pGridLayout->setMargin(0);
    m_tableView = new TableView(this);
    m_tableView->verticalHeader()->setVisible(false);
    m_tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_tableView->setAccessibleName("CustomTableWidget_TableView");
    m_tableView->setItemDelegate(new CustomTableItemDelegate(this));

    m_customTableModel = new PropertyTableModel(m_tableView);

    pGridLayout->addWidget(m_tableView, 0, 0, 1, 1);

    connect(m_tableView, SIGNAL(doubleClicked(const QModelIndex &)), this,
            SLOT(onTableItemDoubleClicked(const QModelIndex &)));
    connect(m_customTableModel, SIGNAL(modelDataItemChanged(const CustomModelItem &, const CustomModelItem &)), this,
            SIGNAL(tableDataItemChanged(const CustomModelItem &, const CustomModelItem &)));
}

void CustomTableWidget::setModelData(const QList<CustomModelItem> &listdata, const QStringList headerlist)
{
    if (m_customTableModel != nullptr && m_tableView != nullptr) {
        m_customTableModel->setModelData(listdata, headerlist);
        m_tableView->setModel(m_customTableModel);
    }
}
