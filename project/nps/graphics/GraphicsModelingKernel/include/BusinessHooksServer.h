#pragma once

#include "ConnectorWireContext.h"
#include "Global.h"
#include "PortContext.h"
#include "SourceProxy.h"
#include <QSharedPointer>

class CanvasContext;
/// @brief 业务钩子,和画板业务相关的接口都由钩子服务提供默认实现
/// 不同业务根据需求不同，提供对应的钩子实例对象
class GRAPHICSMODELINGKERNEL_EXPORT BusinessHooksServer
{
public:
    BusinessHooksServer(QSharedPointer<CanvasContext> canvascontext);

    /// @brief 复制粘贴模块时，设置新模块的属性值
    /// @param orignalSouce 被复制的模块
    /// @param newSource 粘贴的新模块
    virtual void setNewSourceProxy(QSharedPointer<SourceProxy> orignalSouce, QSharedPointer<SourceProxy> newSource,
                                   QVector<QString> &allSourceName = QVector<QString>());

    /// @brief 资源是否可以创建
    /// @param source
    /// @return
    virtual bool enableCreate(int cpsourcetype, QMap<QString, QSharedPointer<SourceProxy>> cpsourcemap);

    virtual bool enableCreate(int cpsourcetype, QSharedPointer<SourceProxy> cpsource);

    QSharedPointer<CanvasContext> getCanvasContext();

    // 设置创建的端口
    virtual void setCreatePortContext(QSharedPointer<PortContext> portCtx);

    /// @brief 获取电气母线的相数
    virtual int getElectricalPhaseNumber(QSharedPointer<SourceProxy> source);

    /// @brief 获取电气模块的电压
    /// @param source
    /// @return
    virtual double getElectricalBlockVoltage(QSharedPointer<SourceProxy> source);

    /// @brief 获取电气模块相位
    /// @param source
    /// @return
    virtual QString getElectricalBlockPhase(QSharedPointer<SourceProxy> source);

    /// @brief 获取电气元件端口的类型  AC  DC   ACDC
    virtual QString getElectricalPortType(QSharedPointer<SourceProxy> source, QString portUUID);

    /// @brief 设置电气元件(母线)  "AC" 参数  0:dc 1:ac
    virtual void setElectricalACDCType(QSharedPointer<SourceProxy> source, int type);

    virtual bool isFullScreenWorkArea() { return false; };

    /// @brief 根据 模块Source判断模块是否为控制组合模块
    /// @param source
    /// @return
    virtual bool isBlockCtrlCombined(QSharedPointer<SourceProxy> source);
    /// @brief 控制组合模块打开模型的画板
    /// @param source
    /// @return
    virtual void openModelBoard(SourceProxy *source);

    /// @brief 校验画板状态,目前电气画板需要校验状态
    virtual void verifyCanvasStatus();

    /// @brief 校验名称是否合法（是否超长 是否重名等）
    virtual bool checkNameValid(const QString &nameString);

    /// @brief 打开警告窗口
    /// @param tips 警告内容
    virtual void showWaringDialg(const QString &tips);

    /// @brief 打开右下角图例作者编辑窗口
    /// @param tips b编辑作者信息
    virtual void showPropertyDialog();

    virtual void createImageAnnotationDialog(QSharedPointer<SourceProxy> sourceProxy);

    /// @brief 用于生成新的模块
    /// @param 生成模块的类型
    /// @param 生成模块的位置
    /// @param 是否支持撤消重做
    /// @param 新增模块的名字
    /// @return 新生成模块的uuid 如果生成失败 返回空
    virtual QString createNewBlock(QString prototypeName, QPointF centerPoint, bool needUndo = true,
                                   QString defaultName = QString(), QString blockUUID = "");
    /// @brief 判断本地库的模块是否在数据管理器
    virtual bool isExistBlock(QSharedPointer<SourceProxy> source);

    /// @brief 判断此模块是否为被修改或删除的状态
    virtual bool checkBlockState(QSharedPointer<SourceProxy> source);

    /// @brief 获取当前画板能创建的模块列表
    virtual QList<BlockStruct> getCanCreateBlockList();

    /// @brief 直接在画板上修改模块命名时有日志提示
    /// @param oldname
    /// @param newname
    virtual void changeTextNameHint(const QString &oldname, const QString &newname);

    virtual QList<showDataStruct> getPowerFlowData(QSharedPointer<SourceProxy> source);

    /// @brief 判断选择的模块是否为用户自定义控制模块
    /// @param source
    /// @return
    virtual bool isBlockCtrlUserCombined(QSharedPointer<SourceProxy> source);

    virtual bool sourceNameIsShowWhenSelectedChanged(bool isSelected);

    /// @brief 获取母线的接线端口编号
    /// @param source
    /// @return 母线的接线端口编号
    virtual int getBusBarPortNumber(QSharedPointer<SourceProxy> source);

    /// @brief 生成PECAD要求的case
    virtual void generatePecadCase();

    /// @brief 拷贝资源对应的模型参数
    virtual void copyModelVariableGroups(PSourceProxy srcSource, PSourceProxy dstSource);

    /// @brief source对应的模型数据是否已经存在于画板中
    virtual bool isModelDataAlreadyExist(PSourceProxy srcSource);

    /// @brief 全屏状态切换
    virtual void workAreaInFullScreenChange(const QString &boradName);

    /// @brief 打开帮助文档
    virtual void openShowHelpWidget(const QString &name);

    /// @brief 创建空白子系统
    virtual void createEmptySubsystem(const QRectF &rc, QString &newModelUUID);

    virtual void createContructorSubSystem(const QRectF &rc, QList<PSourceProxy> selectedSources,
                                           QList<PConnectorWireContext> doubleSelectedWireContext,
                                           QList<PConnectorWireContext> singleSideSelectedWireContext,
                                           QString &newModelUUID, QMap<QString, QString> &newLinkMap);

    /// @brief 销毁子系统(用于撤销重做)
    /// @param selectedSources
    virtual void destroyContructorSubSystem(QString subSystemModelUUID, QList<PSourceProxy> selectedSources);

    virtual void renameSubsystem(const QString &sourceUUID);

    /// @brief 获取当前项目标志
    /// @return
    virtual QString getCurProjectSign();

    /// @brief 获取最顶层ModelUUID,若无顶层Model返回本身的UUID
    /// @return
    virtual QString getTopParentModelUUID();
    /// @brief 获取母线电压等级所对应的显示颜色
    virtual QColor GetBusbarVoltageLevelColor(const double &voltageLevel);

    /// @brief 判断是否为系统内建
    /// @param source
    /// @return
    virtual bool isBlockCtrlCodeType(QSharedPointer<SourceProxy> source);

    /// @brief 获取当前激活项目的路径
    /// @return
    virtual QString getCurrentProjectDir();

    /// @brief 调试使用
    virtual void PrintErrorInfos();

    /// @brief 将一个模块的关联关系拷贝至另一模块,目前用于母线类型切换,将原母线关联关系绑定到新母线上
    /// @param boardUUID 画板uuid
    /// @param srcBlockUUID 原模块uuid
    /// @param dstBlockUUID 目标模块uuid
    virtual void copyModelAssociationRelation(const QString &boardUUID, const QString &srcBlockUUID,
                                              const QString &dstBlockUUID);

    /// @brief 画板是否显示模块的运行顺序
    /// @return
    virtual bool isShowRunningSort();

    /// @brief 是否是电气构造模块
    /// @param blockUUID 模块uuid
    /// @return
    virtual bool isElecCombineModel(const QString &blockUUID);

    /// @brief 资源source所对应的模型是否包含画板(子系统模块)
    /// @return
    virtual QString isContainCanvasContext(PSourceProxy source);

protected:
    QSharedPointer<CanvasContext> m_canvascontext;
};
