#ifndef TRANSLATIONTOOL_H
#define TRANSLATIONTOOL_H

#include "ModelManagerConst.h"

class TranslationTool
{
public:
    // 获取工具箱翻译名称
    static QString getTrToolkitName(const QString &toolkitName)
    {
        if (KL_TOOLKIT::CONTROL_TOOLKIT == toolkitName) {
            return QObject::tr("Control Tool Kit"); // 控制
        } else if (KL_TOOLKIT::ELECTRICAL_TOOLKIT == toolkitName) {
            return QObject::tr("Electrial Tool Kit"); // 电气
        } else if (KL_TOOLKIT::GREENHYDROGEN_TOOLKIT == toolkitName) {
            return QObject::tr("GreenHydrogen Tool Kit"); // 绿氢
        } else if (KL_TOOLKIT::COMPREHENSIVEENERGY_TOOLKIT == toolkitName) {
            return QObject::tr("IntergratedEnergy Tool Kit"); // 综合能源
        } else if (KL_TOOLKIT::IEECONTROLSYSTEM_TOOLKIT == toolkitName) {
            return QObject::tr("IEEControlSystemModel Tool Kit"); // IEE控制系统
        } else {
            return toolkitName;
        }
    }

    // 绿氢模型 树节点名称
    void GreenHydrogenTreeItemNames()
    {
        QObject::tr("Code Model");    // 代码型
        QObject::tr("Combine Model"); // 构造型
    }
};

#endif