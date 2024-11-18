#ifndef SYNCOPERATOR_H
#define SYNCOPERATOR_H

#include "ItemPlot.h"
#include <QObject>

///////////////////////////////////////////////////////////////////////////////////////////////

// class PlotSyncOperatorBase
//{
// public:
//	PlotSyncOperatorBase(){}
//	~PlotSyncOperatorBase(){}
//
//	void addPlotObServer(IPlotObServer* ob);
//	void removePlotObServer(IPlotObServer* ob);
//
//	virtual void notifyPlotObServer(void);
// private:
//	QList<IPlotObServer*> m_obServerList;
// };
//////////////////////////////////////////////////////////////////////////////////////////////
class PlotSyncOperator : public QObject /*, public PlotSyncOperatorBase*/
{
    Q_OBJECT

public:
    PlotSyncOperator(QObject *parent = nullptr);
    ~PlotSyncOperator();

    void add(ItemPlot *item);
    void remove(ItemPlot *item);

    // virtual void notify(void);
    void setSync(bool b = true);
    bool isSync(void) { return m_isSync; }
protected slots:
    void onNotifyCommand(int nCommand, QMap<QString, QVariant> paramMap);

private:
    QList<ItemPlot *> m_itemPlotList;
    bool m_isSync;
};

#endif // SYNCOPERATOR_H
