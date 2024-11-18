#ifndef GRAPHICSMODELINGTOOL_H
#define GRAPHICSMODELINGTOOL_H

#include "GraphicsModelingKernel/CanvasContext.h"
#include "GraphicsModelingKernel/graphicsmodelingkernel.h"
#include "KLModelDefinitionCore/KLModelDefinitionCore.h"

using namespace Kcc::BlockDefinition;

namespace GraphicsModelingTool {
QString getBlockStateString(int blockType); // 根据BlockState获取对应状态字符串 （SourceProxyPrivate.h
Block::ModelState getBlockState(QString strBlockType);
void roundEven(QSizeF &size, int round); // 向上偶数取整 round-取整倍数

bool isPortLinked(PModel pModel, PVariableGroup portGroup, QList<PBlockConnector> connectors); // 端口是否有连接
PBlockConnector
whichConnectorBlockPort(PModel pModel, PVariableGroup portGroup,
                        QList<PBlockConnector> connectors); // 获取诸多连接线中与模块的特定端口连接的那一条

bool getSourceProxyEnableScale(PModel pModel);

bool getSourceProxyShowInsideBorder(PModel pModel);

////查找函数
QList<PModel> findBlockByPrototypeName(const QString &prototypeName, PModel pBoardModel);

void refreshUserDefinedBlockPorts(PModel pBoradModel); // 构造型模块刷新对外端口
bool calculateBlockUuid(QString &uuid, QString &name, PModel pModel,
                        PModel pBoradModel);                     // 计算block的name、uuid
bool enableBlockAdd(PModel pModel, PModel pBoardModel);          // 模块是否允许放入
bool containsUserDefinedBlock(PModel model, PModel pBoardModel); // 构造型模块是否包含模块
} // end namespace

#endif // GRAPHICSMODELINGTOOL_H