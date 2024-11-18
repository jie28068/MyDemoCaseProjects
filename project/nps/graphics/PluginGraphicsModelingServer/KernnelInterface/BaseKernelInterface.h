#pragma once

#include <QObject>
#include <QSharedPointer>
#include <QVariant>

#include "GraphicsModelingKernel/graphicsmodelingkernel.h"
#include "KernelTouch.h"

// 图形建模核心接口基类
class BaseKernelInterface : public QObject
{
    Q_OBJECT

public:
    BaseKernelInterface();

    virtual PCanvasContext loadCanvas(PModel pBoardModel);
    virtual void saveCanvas();

    void readRTCanvasContext(PModel pBoardModel);

    void addBlockToView(PModel pModel, QSharedPointer<ICanvasView>, bool needUndo); // 添加模块
    void deleteBlockFromView(PModel pModel, QSharedPointer<ICanvasView>);           // 删除模块
    void addConnector(PBlockConnector, QSharedPointer<ICanvasView>);
    void deleteConnector(QString, QSharedPointer<ICanvasView>);

    //////
    void emitContentsChanged(); // 发送画板内容变更信号给WorkareaWidget(置*

    void saveDataPointMap(QString uuid); // 实时保存source中的数据模块的坐标位置 不触发*号

protected:
    virtual void loadCanvasAfter(PModel pBoardModel, PCanvasContext pCanvasCtx); // 加载画板，后处理
    void clearPowerFlowResult();

signals:
    void contentsChanged(); // 画板内容发生改变

protected:
    PModel m_boardModel;
    PCanvasContext m_pCanvasContextCache; // 刚开画板的数据缓存
    PCanvasContext m_pRTCanvasContext;    // 画板实时数据

    PKernelTouch m_pKernelTouch;
};

typedef QSharedPointer<BaseKernelInterface> PBaseKernelInterface;
