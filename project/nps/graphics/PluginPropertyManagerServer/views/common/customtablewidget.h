#ifndef CUSTOMTABLEWIDGET_H
#define CUSTOMTABLEWIDGET_H

#include "CommonModelAssistant.h"
#include "PropertyTableModel.h"
#include "TypeItemView.h"

class CustomTableWidget : public CWidget
{
    Q_OBJECT

public:
    CustomTableWidget(const QList<CustomModelItem> &listdata, QWidget *parent = nullptr);
    CustomTableWidget(QWidget *parent = nullptr);
    ~CustomTableWidget();
    QList<CustomModelItem> getNewListData();
    void setListData(const QList<CustomModelItem> &listdata,
                     const QStringList headerlist = QStringList()
                             << CMA::HEADER_PARAMNAME << CMA::HEADER_PARAMVALUE); // 参数名 参数值
    void setItemData(const QString &keyword, const CustomModelItem &item, bool bsendSignal = true); // 更新单条数据
    CustomModelItem getItemData(const QString &keyword, bool bnew = true);
    bool checkValueChanged();
    virtual void setCWidgetReadOnly(bool bReadOnly) override;
    virtual bool checkLegitimacy(QString &errorinfo) override;
    void setVerticalHeaderVisible(bool bvisible);
signals:
    void tableDataItemChanged(const CustomModelItem &olditem, const CustomModelItem &newitem);
private slots:
    void onTableItemDoubleClicked(const QModelIndex &index);

private:
    void initUI();
    void setModelData(const QList<CustomModelItem> &listdata,
                      const QStringList headerlist = QStringList()
                              << CMA::HEADER_PARAMNAME << CMA::HEADER_PARAMVALUE); // 参数名 参数值

private:
    TableView *m_tableView;
    PropertyTableModel *m_customTableModel;
};

#endif // CUSTOMTABLEWIDGET_H
