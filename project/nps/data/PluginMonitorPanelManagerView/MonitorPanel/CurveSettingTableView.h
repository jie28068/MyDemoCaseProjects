#ifndef CURVESETTINGTABLEVIEW_H
#define CURVESETTINGTABLEVIEW_H

#include "CurveSettingTableModel.h"
#include <QTableView>

class CurveSettingTableView : public QTableView
{
    Q_OBJECT

public:
    CurveSettingTableView(QWidget *parent = nullptr);
    ~CurveSettingTableView();

protected:
    // void dropEvent(QDropEvent * event);
    // void dragEnterEvent(QDragEnterEvent * event);
    // void dragMoveEvent(QDragMoveEvent *event);

    // void addCurve(PIElementBase pBase);
private:
    // PIDataDictionaryServer _dataDictionaryServer;
};

#endif // CURVESETTINGTABLEVIEW_H
