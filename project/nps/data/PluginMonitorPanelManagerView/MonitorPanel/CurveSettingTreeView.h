#ifndef CURVESETTINGTREEVIEW_H
#define CURVESETTINGTREEVIEW_H

#include "CurveSettingTreeModel.h"
#include <QTreeView>

class CurveSettingTreeView : public QTreeView
{
    Q_OBJECT

public:
    CurveSettingTreeView(QWidget *parent);
    ~CurveSettingTreeView();

protected:
    void dropEvent(QDropEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void keyPressEvent(QKeyEvent *event)
    {
        if (event->key() == Qt::Key_Return)
            return;
        QTreeView::keyPressEvent(event);
    }

private:
    void addVarible(PIElementBase pBase);
    virtual void mouseMoveEvent(QMouseEvent *event);
    // void autoCurveColor(PlotVaribleInfo& varibleInfo);
private:
    PIDataDictionaryServer _dataDictionaryServer;
};

#endif // CURVESETTINGTREEVIEW_H
