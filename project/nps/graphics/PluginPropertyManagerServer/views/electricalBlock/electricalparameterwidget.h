#ifndef ELECTRICALPARAMETERWIDGET_H
#define ELECTRICALPARAMETERWIDGET_H

#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWidget>

#include "CoreLib/ServerBase.h"
#include "ElideLineText.h"
#include "GraphicsModelingKernel/SourceProxy.h"
#include "KLineEdit.h"
#include "customtablewidget.h"

namespace Kcc {
namespace BlockDefinition {
class ElectricalBlock;
}
}

namespace Kcc {
namespace ElecSys {
struct ElecParamDef;
}
}

class BlockObj;
class BusbarItem;

// 电气属性页“电气参数”tag的内容页
class ElectricalParameterWidget : public CWidget
{
    Q_OBJECT

public:
    ElectricalParameterWidget(QSharedPointer<Kcc::BlockDefinition::ElectricalBlock> block, QWidget *parent = nullptr);
    ~ElectricalParameterWidget();
    QList<CustomModelItem> getTableList();
    QString getName();
    QSharedPointer<Kcc::BlockDefinition::ElectricalBlock> getElecBlock();
    // 判断模块名称是否为空
    void resetName();
    virtual bool saveData();
    /// @brief 检查除名称外的有效性
    /// @param errorinfo
    /// @return
    bool checkExcludeName(QString &errorinfo);
    virtual bool checkLegitimacy(QString &errorinfo) override;
    virtual void setCWidgetReadOnly(bool bReadOnly) override;
    CustomModelItem getItemByKeywords(const QString &keyword,
                                      const QList<CustomModelItem> listdata = QList<CustomModelItem>());
signals:
    void electricalParamChanged(const QList<CustomModelItem> &paramList);
    void phaseNoChanged(const QMap<QString, Kcc::ElecSys::ElecParamDef> &allvars,
                        QSharedPointer<Kcc::BlockDefinition::ElectricalBlock> peblock);
    void jumpToModel();
public slots:
    void onBusBarItemEditFinished();
    void onJumpToConnectModel();
    void onModelDataChanged(const CustomModelItem &olditem, const CustomModelItem &newitem);
    // void onLineEditTextChanged(const QString &);
private:
    void initUI();
    void addBusbarItem();
    bool addBusbarToLayout(QSharedPointer<Kcc::BlockDefinition::ElectricalBlock> block, const QString &portInfoName,
                           QStringList &portNameList);
    QList<CustomModelItem> getBusBarPropertyList();
    QString getPath(QSharedPointer<Kcc::BlockDefinition::ElectricalBlock> busbarblock);
    void setModelItemEditable(QList<CustomModelItem> &listdata, const QMap<QString, bool> editMap);
    void updateElecModelItemList(QList<CustomModelItem> &listdata);
    const int getCurPhaseNo();
    QString getModelBoardPath(QSharedPointer<Kcc::BlockDefinition::Model> model); // 获取控制系统引用的画板路径
    void updateIconLabelPix();

private:
    QSharedPointer<Kcc::BlockDefinition::ElectricalBlock> m_pElectricalBlock;

    QGridLayout *m_gridLayout;
    QGridLayout *m_busbarlayout;
    QLabel *m_iconLabel;
    QLabel *m_labelName;
    KLineEdit *m_lineName;
    QLabel *m_ctrSysReferenceLabel;
    QLabel *m_ctrSysReferencePath;
    QToolButton *m_jumpBtn;
    CustomTableWidget *m_tableWidget;
    QList<BusbarItem *> m_bustbarWidgetList;

    // 当模块名设置为空,则设置成初始化的name
    QString initName;
};

// 母线item
class BusbarItem : public CWidget
{
    Q_OBJECT
public:
    BusbarItem(QSharedPointer<Kcc::BlockDefinition::ElectricalBlock> block, const QString &name, const QString &path,
               QWidget *parent = nullptr);
    ~BusbarItem();
    void setBusbarName(const QString &name);
    QSharedPointer<Kcc::BlockDefinition::ElectricalBlock> getBusbarBlock();

protected:
    virtual void resizeEvent(QResizeEvent *event) override;

private slots:
    void onEditBusbarClicked(bool checked);

signals:
    void busBarItemEditFinished();

private:
    QLabel *m_pBusbarName;
    QPushButton *m_pEditBusbar;
    ElideLineText *m_pBusbarPath;
    QSharedPointer<Kcc::BlockDefinition::ElectricalBlock> m_block;
};

#endif // ELECTRICALPARAMETERWIDGET_H
