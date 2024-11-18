#pragma once

#include "ControlParameterWidget.h"
#include "FMUBlockPropertyWidget.h"
#include "blockproperty.h"
#include "blocktypeselect.h"
#include "customtablewidget.h"
#include "signalselectwidget.h"
#include "variablewidget.h"
#include "varselectwidget.h"

// 控制
class ControlBlockPropertyEditor : public BlockProperty
{
    Q_OBJECT
public:
    ControlBlockPropertyEditor(QSharedPointer<Kcc::BlockDefinition::Model> model, CMA::Operation ope,
                               bool isReadOnly = false, bool canChangeType = false);
    virtual ~ControlBlockPropertyEditor();

    // 设置属性窗口是否可编

    virtual void init() override;
    // CommonWrapper
    virtual QString getTitle() override;
    virtual QPixmap getBlockPixmap() override;
    virtual CommonWrapper::LeftWidgetType getLeftWidgetType() override;
    virtual void onDialogExecuteResult(QDialog::DialogCode code) override;
    virtual QString getHelpUrl() override;
    QSharedPointer<Kcc::BlockDefinition::Model> getCurrentModel();
private slots:
    void onBlockTypeChanged(QSharedPointer<Kcc::BlockDefinition::Model> model);
    void onFMUBlockChanged(QSharedPointer<Kcc::BlockDefinition::Model> model);

private:
    void refreshAllWidget(QSharedPointer<Kcc::BlockDefinition::Model> model, const bool &isInitial = true);
    void refreshCustomTableWidget(CustomTableWidget *&pwidget, const QString &tabname,
                                  QSharedPointer<Kcc::BlockDefinition::Model> model);
    QList<CustomModelItem> getWidgetModelList(const QString &tabname,
                                              QSharedPointer<Kcc::BlockDefinition::Model> model);
    void addParameterWidget(QSharedPointer<Kcc::BlockDefinition::Model> model);
    void addCodeTextWidget();
    /// @brief 判断模块是否可编辑结果保存
    /// @param model
    /// @return
    bool blockCanCheckVar(QSharedPointer<Kcc::BlockDefinition::Model> model);
    /// @brief 是否可添加变量widget
    /// @param model
    /// @return
    bool canAddVarWidget(QSharedPointer<Kcc::BlockDefinition::Model> model);
    /// @brief 还原模块初始值
    /// @param model
    /// @return
    void restoreBlockGroupData(QSharedPointer<Kcc::BlockDefinition::Model> model);
    /// @brief 更新传递函数模块的馈通状态
    /// @param model
    void updateDirectFeedThrough(QSharedPointer<Kcc::BlockDefinition::Model> model);
    /// @brief 获取阶数。格式为a,b,c；取最大阶数,如果a不为0，则为2阶。
    /// @param str
    /// @return
    int getMaxPower(const QString &str);

private:
    QSharedPointer<Kcc::BlockDefinition::Model> m_pCacheInitModel; // FMU类型时，缓存初始的block
    // tab
    BlockTypeSelect *m_pTypeSelectWidget; // 类型
    QWidget *m_pCodeWidget;               // 代码编辑Widget
    VariableWidget *m_pVariableWidget;    // 变量
    CustomTableWidget *m_pInputVarWidget;
    CustomTableWidget *m_pOutputVarWidget;
    CustomTableWidget *m_pDisStateVarWidget;
    CustomTableWidget *m_pConStateVarWidget;
    ControlParameterWidget *m_pParamWidget;
    VarSelectWidget *m_resuleSaveWidget;          // 结果保存页面
    FMUBlockPropertyWidget *m_pFMUPropertyWidget; // 加载FMU模块页面
    SignalSelectWidget *m_pSignalSelectWidget;    // 参数信号选择页面（目前仅bus-selector模块使用）

    QSet<QString> m_otherBlkVarNames;
    CMA::Operation m_Operation;
    bool m_canChangeType;
};
