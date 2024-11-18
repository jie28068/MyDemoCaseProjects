#pragma once
#include "GraphicsModelingKernel/graphicsmodelingkernel.h"
#include "KLModelDefinitionCore/KLModelDefinitionCore.h"
#include "Manager/ModelingServerMng.h"

using namespace Kcc::BlockDefinition;

class BlockDelegateMng;
class BlockDelegateBase : public QObject
{
    Q_OBJECT
public:
    BlockDelegateBase();

    void initBlock(PModel model, BlockDelegateMng *mng);
    void uninitBlock(PModel model);

    /// @brief  编辑模块属性
    /// @return 模块是否变更 true为变更
    virtual bool onEditProperty(SourceProxy *source);

    /// @brief 双击模块
    /// @param source
    /// @return
    virtual bool onDoubleClicked(SourceProxy *source);

    /// <summary>
    /// 模块名称变更后，更新保存的变量路径列表
    /// </summary>
    /// <param name="blockOldNam">模块的旧名称</param>
    /// <param name="blockNewNam">模块的新名称</param>
    void updateOutputVars(QString blockOldName, QString blockNewName);

protected:
    virtual void init(PModel model);
    virtual void uninit();

protected:
    PModel m_pModel;
    BlockDelegateMng *m_pBlockDelegateMng;

    PModel m_boardModel;
};

typedef QSharedPointer<BlockDelegateBase> PBlockDelegateBase;
