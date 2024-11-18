#include "NavWidget.h"
#include "CanvasWidget.h"

const QString itemSufix = "  >";

NavWidget::NavWidget(QWidget *parent) : QListWidget(parent)
{
    m_canvasWidget = qobject_cast<CanvasWidget *>(parent);

    this->setFlow(QListWidget::LeftToRight);
    this->setSelectionMode(QListWidget::SingleSelection);
    this->setFixedHeight(24);
    this->setObjectName("navWidget");
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    connect(this, &NavWidget::itemClicked, this, &NavWidget::onItemClicked);
}

NavWidget::~NavWidget() { }

void NavWidget::setTopModel(PModel pTopModel)
{
    if (pTopModel == nullptr) {
        return;
    }
    m_topModel = pTopModel;
    m_curModel = pTopModel;

    addModelItem(pTopModel);
}

void NavWidget::setCurModel(PModel pModel)
{
    if (!m_curModel || !pModel) {
        return;
    }

    // 清除下级item
    int curRow = -1;
    for (int i = 0; i < this->count(); i++) {
        auto modelUUID = this->item(i)->data(kModelUUIDRole).toString();
        if (pModel->getUUID() == modelUUID) {
            curRow = i;
        }
    }
    if (curRow >= 0) {
        while (this->count() > curRow + 1) {
            QListWidgetItem *item = this->takeItem(curRow + 1);
            if (item) {
                delete item;
                item = nullptr;
            }
        }
    }

    if (m_curModel->getChildModel(pModel->getUUID())) {
        addModelItem(pModel);
    }

    int itemCounts = count();
    for (int i = 0; i < itemCounts; i++) {
        auto currentListItem = item(i);
        if (currentListItem) {
            QString name = currentListItem->text();
            if (i == itemCounts - 1) {
                name.replace(itemSufix, "");
            } else {
                bool flag = name.endsWith(itemSufix);
                if (!flag) {
                    name += itemSufix;
                }
            }
            currentListItem->setText(name);
        }
    }

    // QListWidgetItem *currentListItem = currentItem();
    // if (currentListItem) { }

    m_curModel = pModel;
}

void NavWidget::changeFirstItemName(const QString &modelName)
{
    if (this->count() > 0) {
        this->item(0)->setText(modelName);
    }
}

void NavWidget::addItemList(const QList<PModel> &modelList)
{
    if (!m_topModel || modelList.empty() || modelList[0]->getUUID() != m_topModel->getUUID())
        return;
    this->clear();
    for each (auto model in modelList) {
        QListWidgetItem *item = new QListWidgetItem(model->getName() + itemSufix, this);
        item->setIcon(QIcon(QString(":/drawingboard/modelWidget-normal")));
        item->setData(kModelUUIDRole, model->getUUID());
        if (model == modelList.last()) {
            item->setText(model->getName());
        }
        this->addItem(item);
        m_curModel = model;
    }
}

void NavWidget::addModelItem(PModel pModel)
{
    if (pModel == nullptr) {
        return;
    }
    // 查重
    for (int i = 0; i < this->count(); i++) {
        if (this->item(i)->data(kModelUUIDRole).toString() == pModel->getUUID()) {
            this->item(i)->setSelected(true);
            return;
        }
    }
    int itemCounts = count();
    QString name = pModel->getName();
    if (itemCounts > 0) {
        name += itemSufix;
    }

    QListWidgetItem *item = new QListWidgetItem(name, this);
    item->setIcon(QIcon(QString(":/drawingboard/modelWidget-normal")));
    item->setData(kModelUUIDRole, pModel->getUUID());
    this->addItem(item);

    item->setSelected(true);
}

void NavWidget::onItemClicked(QListWidgetItem *item)
{
    if (!m_canvasWidget || !m_topModel) {
        return;
    }

    QString modelUUID = item->data(kModelUUIDRole).toString();
    if (modelUUID == m_topModel->getUUID()) {
        m_canvasWidget->openCurrentModel(m_topModel);
    } else {
        auto childModel = m_topModel->findChildModelRecursive(modelUUID);
        if (childModel) {
            m_canvasWidget->openCurrentModel(childModel);
        }
    }
}
