#pragma once

#include "GraphicsModelingKernel/graphicsmodelingkernel.h"
#include "KernnelInterface/BaseKernelInterface.h"

class CanvasWidget;
class ModelWidget;
class SimuNPSBusinessHooksServer : public BusinessHooksServer
{
public:
    SimuNPSBusinessHooksServer(QSharedPointer<CanvasContext> pcanvascontext,
                               QSharedPointer<Kcc::BlockDefinition::Model> model);

    virtual void createImageAnnotationDialog(PSourceProxy sourceProxy);
    virtual void setNewSourceProxy(QSharedPointer<SourceProxy> orignalSouce, QSharedPointer<SourceProxy> newSource,
                                   QVector<QString> &allSourceName = QVector<QString>());

    virtual QString createNewBlock(QString prototypeName, QPointF centerPoint, bool needUndo = true,
                                   QString defaultName = QString(), QString blockUUID = "");
    virtual bool isFullScreenWorkArea();

    virtual QList<BlockStruct> getCanCreateBlockList();

    /// @brief source对应的模型数据是否已经存在于画板中
    virtual bool isModelDataAlreadyExist(PSourceProxy srcSource);

    virtual void workAreaInFullScreenChange(const QString &boradName);

    virtual void openShowHelpWidget(const QString &name);

    virtual QString getCurProjectSign();

    virtual QString getTopParentModelUUID();

    virtual QString getCurrentProjectDir();

    virtual void PrintErrorInfos();

    virtual void createEmptySubsystem(const QRectF &rc, QString &newModelUUID);

    virtual void createContructorSubSystem(const QRectF &rc, QList<PSourceProxy> selectedSources,
                                           QList<PConnectorWireContext> doubleSelectedWireContext,
                                           QList<PConnectorWireContext> singleSideSelectedWireContext,
                                           QString &newModelUUID, QMap<QString, QString> &newLinkMap);

    virtual void destroyContructorSubSystem(QString subSystemModelUUID, QList<PSourceProxy> selectedSources);

    virtual void renameSubsystem(const QString &sourceUUID);

    virtual QString isContainCanvasContext(PSourceProxy source);

protected:
    PDrawingBoardClass findBoardModel();
    CanvasWidget *findCanvasWidget();
    ModelWidget *findModelWidget();

    QString getRandomString(int length);

protected:
    PModel m_pBoardClass;
};
