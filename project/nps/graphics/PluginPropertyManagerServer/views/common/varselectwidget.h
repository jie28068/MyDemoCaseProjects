#ifndef VARSELECTWIDGET_H
#define VARSELECTWIDGET_H

#include <QComboBox>
#include <QLineEdit>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QWidget>

#include "PropertyTableModel.h"
#include "TypeItemView.h"
#include "tableheaderview.h"
namespace Kcc {
namespace BlockDefinition {
class Model;
class ElectricalBlock;
class ControlBlock;
}
}
namespace Kcc {
namespace ElecSys {
struct ElecParamDef;
}
}

struct OldBlockVarInfo {
    OldBlockVarInfo() : prototypeName(""), boardName(""), blockName(""), oldOutPutList(QStringList()) { }

    OldBlockVarInfo(const QString &_prototypeName, const QString &_boardName, const QString &_blockName,
                    const QStringList &_oldOutPutList)
        : prototypeName(_prototypeName), boardName(_boardName), blockName(_blockName), oldOutPutList(_oldOutPutList)
    {
    }
    inline bool isValid() const
    {
        return !prototypeName.trimmed().isEmpty() && !boardName.isEmpty() && !blockName.isEmpty();
    }
    inline bool operator==(const OldBlockVarInfo &other) const
    {
        return (prototypeName == other.prototypeName) && (boardName == other.boardName)
                && (blockName == other.blockName) && (oldOutPutList == other.oldOutPutList);
    }
    inline bool operator!=(const OldBlockVarInfo &other) const { return !(*this == other); }

    QString prototypeName;
    QString boardName;
    QString blockName;
    QStringList oldOutPutList; // 初始值已保存的var列表
    QStringList tmpInitList;   // 控制模块切换了类型/母线切换单项三项的初始var列表
};

class VarSelectWidget : public CWidget
{
    Q_OBJECT

public:
    VarSelectWidget(bool bcanOperation = true, QWidget *parent = nullptr);
    ~VarSelectWidget();
    virtual bool saveData() override; // 目前仅给电气用
    bool saveData(QSharedPointer<Kcc::BlockDefinition::Model> model, const QString &groupname = "");
    void updateCheckedVariableInfo(QSharedPointer<Kcc::BlockDefinition::Model> model, const QString &groupname = "",
                                   const bool &isInitial = true); // 控制使用
    void updateCheckedVariableInfo(const QList<CustomModelItem> &listdata, const QString &prototype = "",
                                   const QString &boardname = "", const QString &blockname = "",
                                   const bool &isInitial = true);
    QList<CustomModelItem> getModelItemList();
    virtual void setCWidgetReadOnly(bool bReadOnly) override;
    QStringList getNewSavedList();
    void setCanOperation(bool canope);

signals:
    void syncLineComponentInfo();

public slots:
    void updateElecCheckedVariableInfo(const QMap<QString, Kcc::ElecSys::ElecParamDef> &allvars,
                                       QSharedPointer<Kcc::BlockDefinition::ElectricalBlock> peblock); // 电气使用
private slots:
    void onColumnAllSelected(int col, bool selected);
    void onItemClicked(QTableWidgetItem *item);
    QStringList getSavedVariableList(const QList<CustomModelItem> listdata);

private:
    QTableWidgetItem *createTableWidgetItem(const QString &namestr, bool bReadOnly = false);

private:
    QTableWidget *m_tableWidget;
    OldBlockVarInfo m_oldBlockInfo;
    TableHeaderView *m_pHeaderView;
    QMap<QString, QString> m_keyMaptoUUID;
    bool m_rememberOldBlockInfo;
    bool m_bcanOperation;
    QSharedPointer<Kcc::BlockDefinition::Model> m_elecModel; // 目前仅给电气用
    QString m_prototype;                                     // 仅判断scope模块原型名称
};

#endif // VARSELECTWIDGET_H
