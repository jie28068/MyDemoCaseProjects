#ifndef WIZARDPAGECODENEW_H
#define WIZARDPAGECODENEW_H

#include "WizardPageBase.h"
#include <QDir>
class ComponentCodePage : public WizardPageBase
// 模块代码页 (新增)
{
    Q_OBJECT
public:
    ComponentCodePage(PControlBlock pBlock, PControlBlock npBlock, const QString &path, QWidget *parent = nullptr);
    ~ComponentCodePage();

    virtual void initializePage() override;
    virtual void cleanupPage() override;
    virtual bool validatePage() override;

    void compile();

protected:
    bool isVariableChanged(PControlBlock left, PControlBlock right);
    bool isVariableChanged(PVariableGroup mapVariablesLeft, PVariableGroup mapVariablesRight);
    /// @brief 保存模型数据
    bool saveContorlModel();
    static bool buildCallback(void *pParam, const QString &strInfo, CompileCode code);
    /// @brief 输出输入端口数据生成
    /// @param pBlock
    /// @param variableGroup
    /// @param bIsInput
    /// @param order
    void initPortInfo(PControlBlock pBlock, PVariableGroup variableGroup, bool bIsInput, int order);
    /// @brief 保存输出输入生成的端口
    void savePortInfo();
    /// @brief 将临时的model variable赋值到原始的数据上
    /// @param group 要复制的临时组
    /// @param str 要复制组的名称
    void copyVariable(PVariableGroup group, const QString &str);
    /// @brief 将输入输出的数据赋值到临时的上
    /// @param oldVariable 临时存储的链表
    /// @param newVariable 输入/输出变量
    /// @param isCtrlIn 是否为输入
    void copyPortVariable(QList<PVariable> &oldVariable, const QList<PVariable> &newVariable, bool isCtrlIn);
    /// @brief 赋值端口的详细数据
    /// @param oldVariable
    /// @param newVariable
    /// @param isCtrlIn  是否为输入
    /// @param index
    /// @param total
    void copyPortVariableDate(PVariable oldVariable, PVariable newVariable, bool isCtrlIn, int index, int total);

private slots:
    void onCodeEditorTextChanged(unsigned int code, const NotifyStruct &param);

protected:
    ICodeEditWidget *m_pCodeEdit;
    PControlBlock m_blockCache;
    // 控制动态库生成路径
    QString m_path;
    // 旧文本
    QString oldText;
    // 是否保存了旧文本
    bool isSaveOldText;

private:
    QWidget *m_pCodeWidget;
    QTextEdit *m_pTextEdit;
};

#endif
