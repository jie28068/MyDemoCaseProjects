#ifndef IPLUGINGRAPHICMODELINGSERVER_H
#define IPLUGINGRAPHICMODELINGSERVER_H

#include "KLModelDefinitionCore/KLModelDefinitionCore.h"
#include <QSharedPointer>

class SourceProxy;
class CanvasContext;

namespace Kcc {
namespace PluginGraphicModeling {

using namespace Kcc::BlockDefinition;

class IPluginGraphicModelingServer
{
public:
    /// @brief 根据画板UUID获取指定画板数据
    /// @param boardUUID 画板UUID
    /// @return 指定画板数据
    virtual PDrawingBoardClass getDrawingBoardInfoById(const QString &boardUUID) = 0;

    /// @brief 工作区是否存在此名称的画板
    /// @param boardName 画板名
    /// @return 是否存在
    virtual bool hasDrawingBoard(const QString &boardName) = 0;

    /// @brief 工作区是否存在此名称的画板
    /// @param boardName 画板UUID
    /// @return 是否存在
    virtual bool hasDrawingBoardById(const QString &boardUUID) = 0;

    /// @brief 通过UUID获取画板名
    /// @param boardUUID 画板UUID
    /// @return 画板名
    virtual QString getBoardNameByUUID(const QString &boardUUID) = 0;

    /// @brief 获取当前激活的画板UUID
    /// @return 画板UUID
    virtual QString getActiveDrawingBoardUUID() = 0;

    /// @brief 打开画板，并滚动至指定模块
    /// @param boardClass 打开的画板
    /// @param blockID 指定模块ID
    /// @return 是否成功
    virtual bool openDrawingBoard(PModel pBoardModel, const QString &blockID = "") = 0;

    /// @brief 根据设置信息创建画板
    /// @param boardClass 设置的画板信息
    /// @return 是否成功
    virtual bool createDrawingBoard(PModel pBoardModel) = 0;

    /// @brief 删除画板
    /// @param boardName 画板名称
    /// @return 是否成功
    virtual bool deleteDrawingBoard(const QString &boardName) = 0; // 删除画板

    /// @brief 修改用户自定义画板
    /// @param blockName 自定义画板名称
    virtual void modifyUserDefineBlock(const QString &blockName) = 0;

    /// @brief 模块原型下一次创建时使用的索引
    /// @param prototypeName 模块原型名称
    /// @param pDrawingBoardClass 当前画板
    /// @param blockUUID 当前模块UUID
    /// @param useIndexZero 是否要使用0的索引，复制粘贴出来的模块命名不使用0的索引
    /// @return 可使用的索引
    virtual int getBlockPrototypeNextIndex(const QString &prototypeName, PModel pBoardModel = PModel(),
                                           const QString &blockUUID = "", bool useIndexZero = true) = 0;

    /// @brief 手动设置画板的修改标志(画板必须已在工作区打开)
    /// @param boardUUID 画板UUID
    virtual void setDrawBoardModified(const QString &boardUUID) = 0;

    virtual bool getRunningStatus() = 0; // 获取画板是否运行状态

    virtual QRectF getMinRect(QString &boardUUID) = 0; // 取能容纳当前画板所有模块的最小size

    virtual QStringList getAllOpenBoardUUID() = 0; // 获取当前所有打开画板uuid

    /// @brief 打开顶层画板或者其子系统，高亮并滚动至指定模块
    /// @param pBoardModel 当前顶层画板对象
    /// @param blockModel 指定高亮的模块对象
    /// @param SubSystemModel 顶层画板所属的子系统对象（可以是嵌套的子系统对象）
    /// @return 是否成功
    virtual bool setDisplayAndHighlightModel(PModel boardModel, PModel blockModel,
                                             PModel subSystemModel = PModel()) = 0;
    /// @brief 校验画板状态
    /// @param boardModels 画板列表
    virtual void VerifyStatus(QList<PModel> boardModels) = 0;
};

typedef QSharedPointer<IPluginGraphicModelingServer> PIPluginGraphicModelingServer;

// 服务通知类型
enum {
    Notify_DrawingBoardActived = 1, // 图层激活
    Notify_DrawBoardChanged,        // 工作区画板切换， todo 这个跟第一个有什么区别？
    Notify_DrawingBoardClosed,      // 工作区画板关闭
    Notify_PowerFlowResultClear,    // 潮流计算结果被清空
};

}
}

#endif // IPLUGINGRAPHICMODELINGSERVER_H