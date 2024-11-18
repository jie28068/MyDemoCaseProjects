#ifndef ITEMPLOTCANVAS_H
#define ITEMPLOTCANVAS_H

#include "../QtWidgets/QMenu"
#include "DataAnalyseTreeView.h"
#include "ItemPlotCurve.h"
#include "KLProject/klproject_global.h"
#include "ProjectManagerServer/IProjectManagerServer.h"
#include "def.h"
#include <QList>
#include <QMap>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <qwt_plot.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_zoomer.h>
#include <qwt_scale_div.h>
using namespace Kcc::ProjectManager;

#define ZOOMIN_VALUE 1.0526315789f
#define ZOOMOUT_VALUE 0.95f
#define NUM_SCALE 1000000
#define DATADECIMAL 6

class ItemPlotMarker;

class ItemPlotCanvas : public QwtPlotCanvas
{
    Q_OBJECT

public:
    ItemPlotCanvas(QwtPlot *plot);
    ~ItemPlotCanvas();

    // typedef struct SelectRange
    //{
    //	double left;
    //	double right;
    //	double top;
    //	double bottom;
    // }*PSelectRange;

    // struct

    typedef struct SelectPoint {
        SelectPoint() : otherData(0), isOtherData(false) { }
        QPoint ScreenPoint;
        QPointF DataPoint;
        QColor color;
        QString name;
        qreal otherData;
        bool isOtherData;
    } *PSelectPoint;

    void addMarker(const MarkerInfo &info);
    void setMarkerInfo(const QList<MarkerInfo> &infos);
    QList<MarkerInfo> getMarkerInfo(void) const;
    void refreshAllMarkerPoint(void);

    void setMouseFollow(bool b) { m_isMouseFollow = b; }

    void drawSelected(const AxisRanges &rect);

    void drawMouseMovePoint(QPointF &point);

    void setCovered(bool iscover) { mIsCovered = iscover; }

    // 放大当前选中
    void zoomSelected(const AxisRanges &rect /*,bool bRecord = true*/);
    // 缩放到上次选中
    void zoomReback(void);
    // 缩放到原始
    void zoomBase(bool bforce = false);

    void clearSelected(void);

    // 清除历史放大选区，重置Plot内所有曲线数据的放大选曲范围
    // void setXAxisScale(double nMin,double nMax);
    // void setYAxisScale(double nMin,double nMax);
    void setAxisRange(QwtPlot::Axis axis, qreal nMin, qreal nMax);

    // nleft是选区left占总区间的百分比，nRight同样是选区………
    // void getSelectedRange(double& nLeft,double& nRight);

    // 放大后，设置Plot里所有曲线数据的范围，用于优化放大速度
    void setPlotCurveDataSelectRange(double nLeft, double nRight);

    void setAllowZoomInXAxis(bool b) { m_isAllowSelectXAxis = b; }
    void setAllowZoomInYAxis(bool b) { m_isAllowSelectYAxis = b; }
    void setMousePoint(bool b) { m_isMouseFollow = b; }
    void setCanPopMenu(bool can) { mCanPopMenu = can; }

    void setAxisScale(QwtPlot::Axis axis, double nMin, double nMax);

    // bool setIsLog(bool xBottomIsLog);

    void setDecimal(int xDecimal, int yDecimal)
    {
        m_xDecimal = xDecimal;
        m_yDecimal = yDecimal;
    }

    void setPlotInfo(const PlotInfo &info) { m_PlotInfo = info; } ////2022.11.14

    void setDataAnalyseObserver(IDataAnalyseObserver *dataAnalyseObserver)
    {
        m_dataAnalyseObserver = dataAnalyseObserver;
    }

    static QList<SelectPoint> getNeerPoint(QwtPlot *plot, qreal x, qreal y = INVALID_VALUE);
signals:
    void notifyCommand(int nCommand, QMap<QString, QVariant> paramMap);
    void refreshMarkerPoint(const QString &markerId, qreal pos, const QList<MarkerPoint> &points);

    void addMarkerSignal(const MarkerInfo &info);
    void setMarkerInfoSignal(const QList<MarkerInfo> &infos);

    void modifyNotify(void);

protected slots:
    void onInitCoordinate();
    void onCopyAllDataToClipBoard();
    void onCopyVisualAreaDataToClipBoard();

private:
    QString getCurveDataString(bool isCopyAllData);

protected:
    virtual void paintEvent(QPaintEvent *event);

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    // void enterEvent(QEvent * event);
    void leaveEvent(QEvent *event);
    void wheelEvent(QWheelEvent *event);
    virtual void contextMenuEvent(QContextMenuEvent *event);

    // bool event(QEvent* e);

    void clearPlotCurveDataSelectRange(void);
    void getPlotCurveDataNearPoint(QPointF &point);

    void wheelZoom(bool b);

    void resizeEvent(QResizeEvent *e) override;

    // void contextMenuEvent(QContextMenuEvent *e)override;
    bool eventFilter(QObject *obj, QEvent *e);

    void updateDataAnalyseDataInfo(void);
public slots:
    void onDataAnalyseActionTriggered(void);

    void onModifyMarkerName(const QString &markerId, const QString &name);
    void onModifyMarkerColor(const QString &markerId, const QColor &color);
    void onModifyMarkerIsShow(const QString &markerId, bool show);
    void onRemoveMarker(const QString &markerId);

private:
    AxisRanges m_selectRect;     // 鼠标选中的窗口坐标范围
    AxisRanges m_tempSelectRect; //
    AxisRanges m_baseAxisRect;   // 原始坐标范围

    QList<AxisRanges> m_selectRectList; // 用于记录历史放大选区

    bool m_isCtrlPress;

    double m_wheelZoomValue; // 只用于滚轮放大缩小的系数,非滚轮改变坐标范围一定要将m_wheelZoomValue=1.0f;

    // 滚轮放大缩小系数为1时保存的坐标范围值，
    // 主要用于在缩放时系数约等于1是能将坐标还原，
    // 因为缩放系数的计算值可能永远也回不到只能近似1，所以当近似1时设这个坐标范围，而不用计算的值范围
    AxisRanges m_wheelZoomRect;

    QPointF m_mousePos;
    QPointF m_tempMousePos;

    QList<SelectPoint> m_selectPointList;

    bool m_isAllowSelectXAxis;
    bool m_isAllowSelectYAxis;
    bool mCanPopMenu = true;

    // bool m_isAllowMouse

    QPoint m_pannerStartPos; // 平移时的起始标记点

    DataAnalyseTreeView *m_dataAnalyseTreeView;

    QAction *m_dataAnalyseAction;
    QMenu *m_menu;

    int m_maxLegendWidth;
    bool m_isNotNeedRecord;

    bool m_isMouseFollow;
    // bool m_xBottomIsLog;

    int m_xDecimal; // x轴小数点
    int m_yDecimal; // y轴小数点

    QList<ItemPlotMarker *> m_markerList;

    ItemPlotMarker *m_currentMarker;
    PlotInfo m_PlotInfo; // 2022.11.14

    IDataAnalyseObserver *m_dataAnalyseObserver;

    QMenu *mMainMenu;
    QMenu *mSubMenu;
    QAction *mInitCoordinate;
    QAction *mCopyAllData;
    QAction *mCopyVisualData;
    PIProjectManagerServer mPProjectMng;
    QFutureWatcher<QString> mFutureWatcher;
    bool mIsCovered = false;
};

class ItemPlotMarker : public QObject
{
    Q_OBJECT

public:
    ItemPlotMarker(const MarkerInfo &info, ItemPlotCanvas *canvas);
    ~ItemPlotMarker();

    MarkerInfo getMarkerInfo(void) const;

    MarkerType markerType() { return m_markerType; }
    QString markerId(void) const { return m_id; }
    void setScale(qreal scale);
    void setPos(qreal pos);
    void setColor(const QColor &clr);
    void setName(const QString &name);
    void setShow(bool show);
    bool isShow(void) const { return m_isShow; }

    QRectF rect(void) const;
    bool contains(const QPointF &pos);
    void resize(bool bCalcValue = false);
    void refresh(bool bCalcValue);

    void startMove(const QPointF &startPos);
    void moveTo(const QPointF &endPos);

signals:
    void refreshMarkerPoint(const QString &markerId, qreal x, const QList<MarkerPoint> &points);

protected:
    void draw(QPainter *painter, const QRectF &canvasRect);

private:
    void update(void);
    // void refreshValue(void);

    friend class ItemPlotCanvas;

private:
    MarkerType m_markerType;
    qreal m_scale;
    qreal m_pos;
    qreal m_value;
    ItemPlotCanvas *m_canvas;
    QColor m_color;

    QPointF m_startPos;

    QList<ItemPlotCanvas::SelectPoint> m_points;

    QString m_id;

    bool m_isShow;
    QString m_name;
};

#endif // ITEMPLOTCANVAS_H
