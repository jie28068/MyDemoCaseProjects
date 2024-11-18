#ifndef WIZARDPAGEPARAMNEW_H
#define WIZARDPAGEPARAMNEW_H

#include "CustomTableView.h"
#include "KLModelDefinitionCore/Variable.h"
#include "TableDelegate/ComboxDelegate.h"
#include "TableDelegate/DeviceComboxDelegate.h"
#include "TableDelegate/ElecComboxDelegate.h"
#include "TableModel/DeviceWizardTableModel.h"
#include "TableModel/ElecWizardTableModel.h"
#include "TableModel/WizardTableModel.h"
#include "VariableGroup.h"
#include "WizardPageBase.h"
#include <QMouseEvent>
class MyTableView;
// 模块参数页 (新增)
class ComponentParamPage : public WizardPageBase
{
    Q_OBJECT
public:
    ComponentParamPage(PControlBlock pBlock, PControlBlock npBlock, QWidget *parent = nullptr);
    /// @brief 新增电气模块
    /// @param pBlock
    /// @param pDataServer
    /// @param parent
    explicit ComponentParamPage(PElectricalBlock pElecBloc, PElectricalBlock npElecBloc, QWidget *parent = nullptr);
    /// @brief 新增设备类型
    /// @param pElecBloc
    /// @param pModelServer
    /// @param parent
    explicit ComponentParamPage(PDeviceModel pElecBloc, PDeviceModel npElecBloc, QWidget *parent = nullptr);
    virtual void initializePage() override;
    virtual void cleanupPage() override;
    virtual bool validatePage() override;
    virtual bool isComplete() const override;
    /// @brief 设置已有的电气组
    /// @param lists
    void setElecGroup(QList<PVariableGroup> lists);
    /// @brief 设置已有的设置组
    /// @param list
    void setDeviceGroup(PVariableGroup list);

    void Init(CustomProxyModel *proxyModel, const QStringList &names);

protected:
    //*****************控制*******************//
    /// 初始化
    void pageInit(PControlBlock templateBlock = nullptr);
    /// @brief 完成时将表格上的数据保存到Variable中
    void getPageInfo();
    /// @brief 遍历各个组获取variable，以读取到表格上
    /// @param category
    /// @param listVariableNames
    void initModelData(WizardTableModel::Category category, const QStringList &listVariableNames);
    /// @brief 提示消息
    /// @param strTips
    void setTipsInfo(const QString &strTips) const { messageLabel->setText(strTips); }
    /// @brief 检查表格输入是否合法
    /// @param strName
    /// @param strText
    /// @param paramType
    /// @return
    bool checkParamValue(const QString &strName, const QString &strText, const QString &paramType) const;
    /// @brief 组的创建/复制
    /// @param controlBlock
    void setControlGroup(PControlBlock controlBlock);
    /// @brief 将临时的model variable赋值到原始的数据上
    /// @param group 要复制的临时组
    /// @param str 要复制组的名称
    /// @param model 要复制的model
    void copyVariable(PVariableGroup group, const QString &str, PModel model);
    /// @brief 将variable中的值赋值到表格上
    /// @param variable
    /// @param category
    /// @param strName
    void setVariableValue(PVariable variable, WizardTableModel::Category category, const QString &strName);
    /// @brief 按照order顺序重新将variable的name赋值
    /// @param group 当前组
    /// @param nameList 新的名称
    /// @param groupName 当前组名
    void setVariableNameByOrder(PVariableGroup group, QStringList nameList, QString groupName);

    //*****************电气*******************//
    /// @brief 初始化页面数据 1 创建组
    /// @param
    void pageElecInit(PElectricalBlock elecBlock = nullptr);
    /// @brief 初始化页面数据 2 分组添加row
    /// @param category 类别
    /// @param listVariableNames 变量名称组
    /// @param isResultTide 是否为潮流结果组
    void initElecModelData(ElecWizardTableModel::Category category, const QStringList &listVariableNames,
                           bool isResultTide = false);
    /// @brief 初始化页面数据 3 组设置数据
    /// @param key
    /// @param type
    void initElecVariable(const QString &key, PVariable var, ElecWizardTableModel::Category category, int order = 1);
    /// @brief 初始化页面数据 4 表设置数据
    /// @param category 表行
    /// @param var 一般的数据
    /// @param isPowerResult 是否为潮流结果
    /// @param vable 端口数据
    void initElecAddRow(ElecWizardTableModel::Category category, PVariable var, bool isPowerResult = false,
                        PVariable vable = nullptr);
    /// @brief 保存电气参数
    void saveElecParam();
    /// @brief 是否为电气向导
    /// @return
    bool getIsElec();
    /// @brief 删除多余的参数组
    /// @param group 指定要删除的组
    /// @param str 要比较的字符串
    void deleteMoreThanVariable(PVariableGroup group, const QList<QString> &str);
    /// @brief 将临时的模块保存
    /// @param elecBlock
    void saveTempElecModel(PElectricalBlock elecBlock);

    //*****************设备类型*****************//
    void pageDevice(PDeviceModel deviceBlock = nullptr);
    /// @brief 是否为设备类型
    /// @return
    bool getIsDevice();
    /// @brief 设备类型参数组赋值
    /// @param list
    void setValueDevice(const QStringList &list);
    /// @brief 设备类型表格赋值
    /// @param list
    void setTableValueDevice(const QStringList &list);

private slots:
    void onActivated(const QModelIndex &index);

protected:
    WizardTableModel m_model;
    ElecWizardTableModel m_elecModel;
    DeviceWizardTableModel m_deviceModel;
    // 控制
    PControlBlock m_blockTemp;
    PControlBlock m_oldBlock;
    MyTableView *tableView;

private:
    bool isElec;          // 是否为电气模块
    bool isTemplate;      // 是否为模板
    QLabel *messageLabel; // 消息提示
    bool isDevice;        // 是否为设备类型

    //************参数组***********//
    PVariableGroup mElectricalGroup;     // 电气组
    PVariableGroup mLoadFlowGroup;       // 潮流参数组
    PVariableGroup mLoadFlowResultGroup; // 潮流结果组
    PVariableGroup mSimulationGroup;     // 仿真组
    PVariableGroup mInitializationGroup; // 初始值组
    PVariableGroup mDeviceGroup;         // 设备类型
};
#endif