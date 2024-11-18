#ifndef CLIPBOARDMANAGER_H
#define CLIPBOARDMANAGER_H

#include "KLModelDefinitionCore/KLModelDefinitionCore.h"

using namespace Kcc::BlockDefinition;

// 剪切板管理-用于处理跨画板剪切粘贴
class ClipboardManager
{
public:
    static ClipboardManager &getInstance();

    void addBlock(PModel model);                                // 添加剪切的Block
    PModel getChildModel(QString boardUUID, QString blockUUID); // 查找map，并返回对应模块

private:
    ClipboardManager() {};
    ~ClipboardManager() {};

private:
    QMap<QString, QMap<QString, PModel>> m_cutBlockMap; // key1-画板UUID,key2-模块UUID，PBlock-模块对象
};

#endif // CLIPBOARDMANAGER_H
