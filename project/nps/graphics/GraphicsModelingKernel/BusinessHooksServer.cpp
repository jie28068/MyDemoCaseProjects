#include "BusinessHooksServer.h"
#include "CanvasContext.h"
#include "PortContext.h"
#include "SourceProxy.h"
#include "Utility.h"

BusinessHooksServer::BusinessHooksServer(QSharedPointer<CanvasContext> canvascontext)
{
    m_canvascontext = canvascontext;
}

void BusinessHooksServer::setNewSourceProxy(QSharedPointer<SourceProxy> orignalSouce,
                                            QSharedPointer<SourceProxy> newSource, QVector<QString> &allSourceName)
{
}

bool BusinessHooksServer::enableCreate(int cpsourcetype, QMap<QString, QSharedPointer<SourceProxy>> cpsourcemap)
{
    return true;
}

bool BusinessHooksServer::enableCreate(int cpsourcetype, QSharedPointer<SourceProxy> cpsource)
{
    return true;
}

QSharedPointer<CanvasContext> BusinessHooksServer::getCanvasContext()
{
    return m_canvascontext;
}
void BusinessHooksServer::setCreatePortContext(QSharedPointer<PortContext> portCtx) { }

int BusinessHooksServer::getElectricalPhaseNumber(QSharedPointer<SourceProxy> source)
{
    return 3;
}

double BusinessHooksServer::getElectricalBlockVoltage(QSharedPointer<SourceProxy> source)
{
    return 0.0;
}

QString BusinessHooksServer::getElectricalBlockPhase(QSharedPointer<SourceProxy> source)
{
    return QString();
}

QString BusinessHooksServer::getElectricalPortType(QSharedPointer<SourceProxy> source, QString portUUID)
{
    return QString();
}

void BusinessHooksServer::setElectricalACDCType(QSharedPointer<SourceProxy> source, int type) { }

bool BusinessHooksServer::isBlockCtrlCombined(QSharedPointer<SourceProxy> source)
{
    return false;
}

void BusinessHooksServer::openModelBoard(SourceProxy *source) { }

void BusinessHooksServer::verifyCanvasStatus() { }

bool BusinessHooksServer::checkNameValid(const QString &nameString)
{
    return true;
}

void BusinessHooksServer::showWaringDialg(const QString &tips) { }

void BusinessHooksServer::showPropertyDialog() { }

void BusinessHooksServer::createImageAnnotationDialog(PSourceProxy sourceProxy) { }

QString BusinessHooksServer::createNewBlock(QString prototypeName, QPointF centerPoint, bool needUndo,
                                            QString defaultName, QString blockUUID)
{
    return QString();
}

bool BusinessHooksServer::isExistBlock(QSharedPointer<SourceProxy> source)
{
    return true;
}

bool BusinessHooksServer::checkBlockState(QSharedPointer<SourceProxy> source)
{
    return true;
}

QList<BlockStruct> BusinessHooksServer::getCanCreateBlockList()
{
    return QList<BlockStruct>();
}

void BusinessHooksServer::changeTextNameHint(const QString &oldname, const QString &newname) { }

QList<showDataStruct> BusinessHooksServer::getPowerFlowData(QSharedPointer<SourceProxy> source)
{
    return QList<showDataStruct>();
}

bool BusinessHooksServer::isBlockCtrlUserCombined(QSharedPointer<SourceProxy> source)
{
    return false;
}

bool BusinessHooksServer::sourceNameIsShowWhenSelectedChanged(bool isSelected)
{
    return true;
}

int BusinessHooksServer::getBusBarPortNumber(QSharedPointer<SourceProxy> source)
{
    return -1;
}

void BusinessHooksServer::generatePecadCase() { }

void BusinessHooksServer::copyModelVariableGroups(PSourceProxy srcSource, PSourceProxy dstSource) { }

bool BusinessHooksServer::isModelDataAlreadyExist(PSourceProxy srcSource)
{
    return false;
}

void BusinessHooksServer::workAreaInFullScreenChange(const QString &boradName) { }

void BusinessHooksServer::openShowHelpWidget(const QString &name) { }

void BusinessHooksServer::createEmptySubsystem(const QRectF &rc, QString &newModelUUID) { }

void BusinessHooksServer::createContructorSubSystem(const QRectF &rc, QList<PSourceProxy> selectedSources,
                                                    QList<PConnectorWireContext> doubleSelectedWireContext,
                                                    QList<PConnectorWireContext> singleSideSelectedWireContext,
                                                    QString &newModelUUID, QMap<QString, QString> &newLinkMap)
{
}

void BusinessHooksServer::destroyContructorSubSystem(QString subSystemModelUUID, QList<PSourceProxy> selectedSources) {
}

void BusinessHooksServer::renameSubsystem(const QString &sourceUUID) { }

QString BusinessHooksServer::getCurProjectSign()
{
    return QString();
}

QColor BusinessHooksServer::GetBusbarVoltageLevelColor(const double &voltageLevel)
{
    return QColor();
}

QString BusinessHooksServer::getTopParentModelUUID()
{
    return QString();
}

bool BusinessHooksServer::isBlockCtrlCodeType(QSharedPointer<SourceProxy> source)
{
    return false;
}

QString BusinessHooksServer::getCurrentProjectDir()
{
    return QString();
}

void BusinessHooksServer::PrintErrorInfos() { }

void BusinessHooksServer::copyModelAssociationRelation(const QString &boardUUID, const QString &srcBlockUUID,
                                                       const QString &dstBlockUUID)
{
}

bool BusinessHooksServer::isShowRunningSort()
{
    return false;
}

bool BusinessHooksServer::isElecCombineModel(const QString &blockUUID)
{
    return false;
}

QString BusinessHooksServer::isContainCanvasContext(PSourceProxy source)
{
    return QString();
}
