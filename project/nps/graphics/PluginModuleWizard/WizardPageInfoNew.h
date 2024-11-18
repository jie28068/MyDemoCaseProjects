#ifndef WIZARDPAGEINFONEW_H
#define WIZARDPAGEINFONEW_H

#include "KLModelDefinitionCore/Variable.h"
#include "KLModelDefinitionCore/VariableGroup.h"
#include "KMessageBox.h"
#include "ModelSystem.h"
#include "WizardIamgeSourceWidget.h"
#include "WizardPageBase.h"
#include "WizardPageParamNew.h"
#include "ui_WizardComponentInfo.h"
#include <QFileDialog>

class IamgeSourceWidget;
// 模块信息页（新增）
class ComponentInfoPage : public WizardPageBase
{
    Q_OBJECT
public:
    /// @brief 控制新增
    /// @param pBlock
    /// @param pModelServer
    /// @param parent
    ComponentInfoPage(PControlBlock pBlock, PControlBlock npBlock, QWidget *parent = nullptr);
    /// @brief 电气新增
    /// @param pBlock
    /// @param pModelServer
    /// @param parent
    explicit ComponentInfoPage(PElectricalBlock pBlock, PElectricalBlock npBlock, QWidget *parent = nullptr);
    /// @brief 设备类型新增
    /// @param pBlock
    /// @param pModelServer
    /// @param parent
    explicit ComponentInfoPage(PDeviceModel pBlock, PDeviceModel npBlock, QWidget *parent = nullptr);

    virtual bool isComplete() const override;
    virtual bool validatePage() override;
    void setBlockEditable(bool edit);
    /// @brief 设置电气参数页
    /// @param pParamPage
    void setElecParameterPage(ComponentParamPage *pParamPage);

private slots:
    void onAddButtonClicked();
    /// @brief 电气原型下拉框
    /// @param strText
    void onComboBoxIndexChanged(const QString &strText);

protected:
    /// @brief 判断工具箱是否存在该模块
    /// @param strName
    /// @return
    virtual bool isValidName(const QString &strName) const;
    void setTips(const QString &strTips) const;
    bool invalidateVariableName(const QString &strText, QStringList &listTotalVariable,
                                const QString &variableType) const;
    void addVariableFromTemplate(QLineEdit *pLineEdit, const QStringList &listTemplateVariable, bool bKeepOld) const;
    /// @brief 是否为设备类型
    /// @return
    bool getIsDevice();
    /// @brief 是否为电气
    /// @return
    bool getIsElec();
    /// @brief 获取名称控件
    /// @return
    QLineEdit *getNmaeLineEdit();
    /// @brief 设置相数
    /// @param value
    void setPhaseComboBox(int value);
    /// @brief 所有控件是否可操作
    /// @param value
    void setControlIde(bool value);
    /// @brief 设置模板控件是否可用
    /// @param value
    void setModelComboBox(bool value);
    /// @brief 获取中文名称
    QLineEdit *getNameCHSLineEdit();
    /// @brief 获取当前电气所有的模块原型名
    /// @return
    QStringList getAllElecModelProeryName();
    /// @brief 使用已有的模块初始化参数页
    /// @param block 电气模块
    /// @param portlist 端口列表
    void setElecModuleType(PElectricalBlock block);
    /// @brief 获取电气与设备类型模块的各参数组的参数
    /// @param map 参数组
    /// @param type 参数组类型
    /// @return 参数列表
    QStringList initParamList(const QList<PVariable> &list, const QString &type,
                              QStringList &portNameList = QStringList());
    /// @brief 设置关联电气原型所有控件
    /// @return
    void setRelevanceElecComboBox(const QString &str);
    /// @brief 设置关联电气原型下拉框控件
    /// @param falg
    void setRelevanceElecComboBox(bool falg);

private:
    Ui::WizardComponentInfo ui;
    QStringList classificationLists; // 电气模块原型的分类（每种类型按从上到下显示）
    QStringList phaseNumberLists;    // 相数
    bool isElec;                     // 是否为电气模块
    bool isDevice;                   // 是否为电气设备类型
    QLabel *modelTypeLabel;          // 参数模板
    //**电气控件**//
    QComboBox *modelType;             // 模板类型
    KLineEdit *nameLineEdit;          // 原型名称
    KLineEdit *nameCHSLineEdit;       // 中文名称
    KLineEdit *nameReadLineEdit;      // 简称
    KLineEdit *showExplainLineEdit;   // 说明
    KLineEdit *portLineEdit;          // 端口
    KLineEdit *paramLineEdit;         // 电气参数
    KLineEdit *tideLineEdit;          // 潮流参数
    KLineEdit *tideResultLineEdit;    // 潮流结果
    KLineEdit *simuLineEdit;          // 仿真设置
    KLineEdit *initLineEdit;          // 仿真初始值
    QLabel *elecLabelTips;            // 提示消息
    QComboBox *phaseComboBox;         // 相数
    QLineEdit *phaseLineEdit;         // 自定义相数
    QComboBox *relevanceElecComboBox; // 关联电气原型
    // end
    IamgeSourceInfoPage *iamgeSourceWidget; // 导入图片页面
    ComponentParamPage *m_pParamPage;       // 参数页
    QStringList devideModelNames;           // 设备类型模板组
};
#endif
