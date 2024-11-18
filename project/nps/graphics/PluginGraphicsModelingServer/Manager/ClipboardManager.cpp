#include "ClipboardManager.h"

ClipboardManager &ClipboardManager::getInstance()
{
    static ClipboardManager instance;
    return instance;
}

void ClipboardManager::addBlock(PModel model)
{
    if (!model)
        return;
    QString boardUUID = model->getParentModelUUID();
    if (m_cutBlockMap.contains(boardUUID)) {
        m_cutBlockMap[boardUUID][model->getUUID()] = model;
    } else {
        QMap<QString, PModel> blockMap;
        blockMap[model->getUUID()] = model;
        m_cutBlockMap[boardUUID] = blockMap;
    }
}

PModel ClipboardManager::getChildModel(QString boardUUID, QString blockUUID)
{
    QMap<QString, PModel> blockMap;

    if (m_cutBlockMap.contains(boardUUID)) {
        blockMap = m_cutBlockMap[boardUUID];
        if (blockMap.contains(blockUUID)) {
            return blockMap.value(blockUUID);
        }
    }

    return PModel();
}
