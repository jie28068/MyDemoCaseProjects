#ifndef WIZARDPAGEINFOCHANGE_H
#define WIZARDPAGEINFOCHANGE_H

#include "KLModelDefinitionCore/Variable.h"
#include "KLModelDefinitionCore/VariableGroup.h"
#include "WizardPageInfoNew.h"

// 模块信息页（修改）
class ComponentInfoPageOnModify : public ComponentInfoPage
{
public:
    ComponentInfoPageOnModify(PControlBlock pBlock, PControlBlock npBlock, QWidget *parent = nullptr)
        : ComponentInfoPage(pBlock, npBlock, parent)
    {
    }

    ComponentInfoPageOnModify(PElectricalBlock pElecBlock, PElectricalBlock npElecBlock, QWidget *parent = nullptr)
        : ComponentInfoPage(pElecBlock, npElecBlock, parent)
    {
    }

    ComponentInfoPageOnModify(PDeviceModel pdeviceBlock, PDeviceModel npdeviceBlock, QWidget *parent = nullptr)
        : ComponentInfoPage(pdeviceBlock, npdeviceBlock, parent)
    {
    }

    virtual void initializePage() override;

    /// @brief 电气模块临时模块赋值
    void temporaryElecModuleAssignment();

    /// @brief 控制模块临时模块赋值
    void temporaryControlModuleAssignment();

    /// @brief 设备类型临时模块赋值
    void temporaryDeviceModuleAssignment();

protected:
    virtual bool isValidName(const QString &strName) const override;

private:
    struct indexsort {
        QString key;
        int index;
        indexsort(QString _key, int _index)
        {
            key = _key;
            index = _index;
        }
    };

    enum type {
        _inputVariables,         // 输入变量
        _outputVariables,        // 输出变量
        _parameters,             // 模块参数
        _InternalVariables,      // 内部变量
        _discreteStateVariables, // 状态变量
        _continueStateVariables  // 状态变量
    };

    QList<indexsort> indexsortList;
    QStringList keyReorder(QStringList keylist, PControlBlock block, type _type);

private:
    QStringList portNameList; // 电气端口名称链表
};

#endif
