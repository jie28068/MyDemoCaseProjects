#ifndef SIGNALSELECTWIDGET_H
#define SIGNALSELECTWIDGET_H

#include "TypeItemView.h"
#include "customtreewidget.h"
#include <QGridLayout>
#include <QListWidget>
#include <QTreeWidget>

namespace Kcc {
namespace BlockDefinition {
class Model;
class ControlBlock;
class DrawingBoardClass;
}
}
class CustomListWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit CustomListWidget(QWidget *parent = nullptr);
    ~CustomListWidget() { }
    QList<QString> getAllItemsText();

protected:
    void dropEvent(QDropEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void startDrag(Qt::DropActions supportedActions) override;

signals:
    void removeSelectedItems();

public slots:
    void onRemoveSelectedItems();

private:
    QList<QListWidgetItem *> m_selItems;
};

class SignalSelectWidget : public CWidget
{
    Q_OBJECT
public:
    explicit SignalSelectWidget(QSharedPointer<Kcc::BlockDefinition::Model> model, bool canEdit,
                                QWidget *parent = nullptr);
    ~SignalSelectWidget();
    bool saveData(QSharedPointer<Kcc::BlockDefinition::Model> model);
    virtual void setCWidgetReadOnly(bool bReadOnly) override;

private:
    void initUI();
    void initData();
    QList<QString> setElementInBus(QSharedPointer<Kcc::BlockDefinition::Model> model, const QString &id = QString(),
                                   QTreeWidgetItem *item = nullptr);
    void treeWidgetSearchAndExpandItem(QTreeWidgetItem *item,
                                       const QString &searchText); // 搜索树状列表中文本匹配的item并展开显示
    void setParentItemsExpand(QTreeWidgetItem *item);              // 树状列表中item的所有父项展开显示
    void setChildItemsExpand(QTreeWidgetItem *item);               // 树状列表中item的所有子项展开显示

public slots:
    void onBtnUpClicked();     // 单步上移操作
    void onBtnDownClicked();   // 单步下移操作
    void onBtnRemoveClicked(); // 单项删除操作

private:
    QSharedPointer<Kcc::BlockDefinition::Model> m_pModel;
    QSharedPointer<Kcc::BlockDefinition::Model> m_pTopItemModel;
    CustomTreeWidget *m_pAllSignalsListWidget;
    CustomListWidget *m_pSelectedSignalsListWidget;
    QList<QString> m_oldItemsTextList;
    bool m_bIsItemExpandChanged;
    bool m_canEdit;
};

#endif // SIGNALSELECTWIDGET_H