#ifndef COLORMATCHWIDGET_H
#define COLORMATCHWIDGET_H

#include "CommonModelAssistant.h"
#include "ColorMatchModel.h"
#include "TypeItemView.h"

class ColorMatchWidget : public CWidget
{
    Q_OBJECT

public:
    ColorMatchWidget(const QList<ColorMatchModelItem> &listdata, QWidget *parent = nullptr);
    ColorMatchWidget(QWidget *parent = nullptr);
    ~ColorMatchWidget();
    QVariantList getNewListData();
    void setListData(const QList<ColorMatchModelItem> &listdata,
                     const QStringList headerlist = QStringList()
                             << CMA::HEADER_PARAMNAME << CMA::HEADER_PARAMVALUE); // 参数名 参数值
    
    bool checkValueChanged();
    virtual void setCWidgetReadOnly(bool bReadOnly) override;
    virtual bool checkLegitimacy(QString &errorinfo) override;
    void setVerticalHeaderVisible(bool bvisible);
signals:
    void tableDataItemChanged(const ColorMatchModelItem &olditem, const ColorMatchModelItem &newitem);
private slots:
    void onTableItemDoubleClicked(const QModelIndex &index);

private:
    void initUI();
    void setModelData(const QList<ColorMatchModelItem> &listdata,
                      const QStringList headerlist = QStringList()
                              << CMA::HEADER_PARAMNAME << CMA::HEADER_PARAMVALUE); // 参数名 参数值
private:
    TableView *m_tableView;
    ColorMatchModel *m_customTableModel;
};

#endif // COLORMATCHWIDGET_H
