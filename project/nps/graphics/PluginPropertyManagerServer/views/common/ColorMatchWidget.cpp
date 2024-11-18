#include "ColorMatchWidget.h"

#include "GlobalAssistant.h"
#include "KLModelDefinitionCore/PublicDefine.h"
#include "KLWidgets/KColorDialog.h"
#include <QGridLayout>
using namespace Kcc::BlockDefinition;

ColorMatchWidget::ColorMatchWidget(const QList<ColorMatchModelItem> &listdata, QWidget *parent)
    : CWidget(parent), m_tableView(nullptr), m_customTableModel(nullptr)
{
    initUI();
    setModelData(listdata);
}

ColorMatchWidget::ColorMatchWidget(QWidget *parent)
    : CWidget(parent), m_tableView(nullptr), m_customTableModel(nullptr)
{
    initUI();
}

ColorMatchWidget::~ColorMatchWidget() { }

QVariantList ColorMatchWidget::getNewListData()
{
    if (m_customTableModel != nullptr) {
        return m_customTableModel->getNewListData();
    }
    return QVariantList();
}

void ColorMatchWidget::setListData(const QList<ColorMatchModelItem> &listdata, const QStringList headerlist)
{
    setModelData(listdata, headerlist);
}

bool ColorMatchWidget::checkValueChanged()
{
    if (m_customTableModel != nullptr) {
        // return m_customTableModel->checkValueChange();
    }
    return false;
}

void ColorMatchWidget::setCWidgetReadOnly(bool bReadOnly)
{
    if (m_customTableModel != nullptr) {
        m_customTableModel->setModelDataReadOnly(bReadOnly);
    }
}

bool ColorMatchWidget::checkLegitimacy(QString &errorinfo)
{
    errorinfo = "";
    if (m_customTableModel != nullptr) {
        // return m_customTableModel->checkValueLegitimacy(errorinfo);
    }
    return false;
}

void ColorMatchWidget::setVerticalHeaderVisible(bool bvisible)
{
    if (m_tableView != nullptr && m_tableView->horizontalHeader() != nullptr) {
        m_tableView->horizontalHeader()->setVisible(false);
    }
}

void ColorMatchWidget::onTableItemDoubleClicked(const QModelIndex &index)
{
    if (index.isValid() && m_customTableModel != nullptr
        && RoleDataDefinition::ControlTypeColor == index.data(NPS::ModelDataTypeRole).toString()) {
        KColorDialog dlg(tr("Select Color"), this);
        dlg.setCurrentColor(index.data(NPS::ModelDataRealValueRole).value<QColor>());
        if (dlg.exec() == KColorDialog::Ok) {
            QColor color = dlg.currentColor();
            m_customTableModel->updateValue(index.row(), index.column(),color);
        }
    }
}

void ColorMatchWidget::initUI()
{
    QGridLayout *pGridLayout = new QGridLayout(this);
    pGridLayout->setMargin(0);
    m_tableView = new TableView(this);
    m_tableView->verticalHeader()->setVisible(false);
    m_tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_tableView->setAccessibleName("ColorMatchWidget_TableView");
    m_tableView->setItemDelegate(new CustomTableItemDelegate(this));

    m_customTableModel = new ColorMatchModel(m_tableView);

    pGridLayout->addWidget(m_tableView, 0, 0, 1, 1);
    connect(m_tableView, SIGNAL(doubleClicked(const QModelIndex &)), this,
            SLOT(onTableItemDoubleClicked(const QModelIndex &)));
    connect(m_customTableModel, SIGNAL(modelDataItemChanged(const ColorMatchModelItem &, const ColorMatchModelItem &)), this,
            SIGNAL(tableDataItemChanged(const ColorMatchModelItem &, const ColorMatchModelItem &)));
}

void ColorMatchWidget::setModelData(const QList<ColorMatchModelItem> &listdata, const QStringList headerlist)
{
    if (m_customTableModel != nullptr && m_tableView != nullptr) {
        m_customTableModel->setModelData(listdata, headerlist);
        m_tableView->setModel(m_customTableModel);
    }
}
