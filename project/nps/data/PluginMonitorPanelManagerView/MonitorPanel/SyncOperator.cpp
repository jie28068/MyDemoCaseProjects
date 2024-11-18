#include "SyncOperator.h"

// void PlotSyncOperatorBase::addPlotObServer(IPlotObServer* ob)
//{
//
// }
//
// void PlotSyncOperatorBase::removePlotObServer(IPlotObServer* ob)
//{
//
// }
//
// void PlotSyncOperatorBase::notifyPlotObServer(void)
//{
//
// }

PlotSyncOperator::PlotSyncOperator(QObject *parent) : QObject(parent), m_isSync(false) { }

PlotSyncOperator::~PlotSyncOperator()
{
    // foreach(ItemPlot* iter,m_itemPlotList)
    //{
    //	disconnect(iter,SIGNAL(notifyCommand(int,QMap<QString,QVariant>)),this,SLOT(onNotifyCommand(int,QMap<QString,QVariant>)));
    // }
}

void PlotSyncOperator::add(ItemPlot *item)
{
    if (m_itemPlotList.contains(item))
        return;

    m_itemPlotList.push_back(item);
    connect(item, SIGNAL(notifyCommand(int, QMap<QString, QVariant>)), this,
            SLOT(onNotifyCommand(int, QMap<QString, QVariant>)));
}

void PlotSyncOperator::remove(ItemPlot *item)
{
    if (!m_itemPlotList.contains(item))
        return;
    disconnect(item, SIGNAL(notifyCommand(int, QMap<QString, QVariant>)), this,
               SLOT(onNotifyCommand(int, QMap<QString, QVariant>)));
    m_itemPlotList.removeOne(item);
}

void PlotSyncOperator::setSync(bool b /*=true*/)
{
    m_isSync = b;
}

// void PlotSyncOperator::notify(void)
//{
//
// }

void PlotSyncOperator::onNotifyCommand(int nCommand, QMap<QString, QVariant> paramMap)
{
    // if(!m_isSync)return;
    // QObject *objSender=sender();
    switch (nCommand) {
    case ItemPlotCanvasCommand_Selecting: {
        if (!m_isSync) {
            if (m_itemPlotList.contains((ItemPlot *)sender())) {
                ((ItemPlot *)sender())->notifySelecting(paramMap["SelectRect"].value<AxisRanges>());
            }
        } else {
            for (ItemPlot *iter : m_itemPlotList) {
                // if(iter==objSender)continue;
                iter->notifySelecting(paramMap["SelectRect"].value<AxisRanges>());
            }
        }

    } break;
    case ItemPlotCanvasCommand_Selected: {
        if (!m_isSync) {
            if (m_itemPlotList.contains((ItemPlot *)sender())) {
                ((ItemPlot *)sender())->notifySelected(paramMap["SelectRect"].value<AxisRanges>());
            }
        } else {
            for (ItemPlot *iter : m_itemPlotList) {
                // if(iter==sender())continue;
                iter->notifySelected(paramMap["SelectRect"].value<AxisRanges>());
            }
        }

    } break;
    case ItemPlotCanvasCommand_MouseMove: {
        if (!m_isSync) {
            if (m_itemPlotList.contains((ItemPlot *)sender())) {
                ((ItemPlot *)sender())->notifyMouseMove(paramMap["MousePos"].value<QPointF>());
            }
        } else {
            for (ItemPlot *iter : m_itemPlotList) {
                // if(iter==objSender)continue;
                iter->notifyMouseMove(paramMap["MousePos"].value<QPointF>());
            }
        }

    } break;
    case ItemPlotCanvasCommand_ResetCoordinate: {
        if (!m_isSync) {
            if (m_itemPlotList.contains((ItemPlot *)sender())) {
                ((ItemPlot *)sender())->notifyResetCoordinate();
            }
        } else {
            for (ItemPlot *iter : m_itemPlotList) {
                // if(iter==objSender)continue;
                iter->notifyResetCoordinate();
            }
        }
    } break;
    default:
        break;
    }
}
