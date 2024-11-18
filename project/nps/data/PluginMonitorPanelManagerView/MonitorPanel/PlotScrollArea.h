#ifndef PLOTSCROLLAREA_H
#define PLOTSCROLLAREA_H

#include "PlotWidget.h"
#include "SyncOperator.h"
#include <QList>
#include <QScrollArea>

#define INVALIDPOS QPoint(-9999999999, -9999999999)
#define INVALIDREC QRect(-9999999999, -9999999999, -9999999999, -9999999999)
#define PLOTMARGIN 0
#define SCROLLAREAMARGIN 8
#define MINPLOTWIDTH 180
#define TOPW(qwidget_ptr) static_cast<PlotWidget *>(qwidget_ptr)

class ScrollAreaWidget : public QWidget
{
    Q_OBJECT
public:
    ScrollAreaWidget(QWidget *parent = nullptr);
    virtual ~ScrollAreaWidget();

    void setRow(int row) { m_rows = row; }
    void setCol(int col) { m_cols = col; }
    void setCursor(const QPoint &p) { mCursor = p; }
    void cancleSelect();
signals:
    void currentIndex(int row, int col);

protected:
    void paintEvent(QPaintEvent *event);

private:
    int m_cols;
    int m_rows;
    QRubberBand *mRub;
    QPoint mCursor;
};

class PosRecord
{
public:
    PosRecord();
    ~PosRecord();

public:
    void setRC(int r, int c);
    const QPair<int, int> &getRc() const;

    void setRealrow(int row) { mRealRow = row; }
    int getRealRow() { return mRealRow; }

    void clear();
    void recordPlotPos(const QString &id, const QPair<int, int> &rc);
    QPair<int, int> getPlotPos(const QString &id) const;

    bool isExit(const QString &id) const;
    bool checkError();

private:
    QPair<int, int> mRc;
    int mRealRow = 0;
    QMap<QString, QPair<int, int>> mPos;
};

class PlotScrollArea : public QScrollArea
{
    Q_OBJECT
public:
    PlotScrollArea(QWidget *parent = nullptr);
    ~PlotScrollArea();

    // 按照传入参数布局，会打乱原有的所有排列
    void relayout(int rows, int cols);

    // 根据图形个数自动判断需要多少行多少列，删除用到
    void intelichangeLayout();

    // 添加图，会率先补空
    void changeLayoutAfterAdd();

    // 根据文件中的位置信息布局，可以留空
    void layoutAllPlotFromFile(int rows, int cols);

    // 将最后添加的图放在空位上（如果有的话）
    bool fillGpas();

    // 只更改每个波形图大小，不改变位置
    void onlyChangeSize();

    // 仅undo/redo使用
    void specificLayout(int realrow, QPair<int, int> &RowAndCol, QMap<QString, QPair<int, int>> &mAllPlotPos);

    // 添加widget对象
    void addWidget(QWidget *w);
    void removeWidget(QWidget *w);
    bool removeWidgetFromTitle(const QString &title); // 通过图表名字去删除
    QList<QWidget *> selectedWidgets(void) const;
    void removeSelectedWidgets(void);
    bool clearWidgets(void);

    void setOnlyShowSelected(bool b);
    bool onlyShowSelected(void);
    void setSync(bool b);
    bool isSync(void);

    void setAutoStop(bool b);
    bool autoStop(void) { return m_isAutoStop; }

    void setStart(bool b);

    QList<QWidget *> widgets(void);
    int widgetCount(void) { return m_widgetList.size(); }

    int splitRowNum(void) { return m_row_split; }
    int cols(void) { return m_cols; }
    int realrow(void) { return mRealRow; }

    void setAllowZoomInXAxis(bool can);
    void setAllowZoomInYAxis(bool can);
    void setMousePoint(bool can);

    void aLignItemPlots();
    void resetSelectedAxis(void);

    void setSelectedXAxisFull(void);
    void setSelectedYAxisFull(void);
    bool isBinging(void) const;

    void allSelect(void);
    void merge(void);
    void unmerge(void);

    void setSelectedSyncMode(bool b) { m_isSelectedSyncMode = b; } // 只适合在初始化时执行
    bool selectedSyncMode(void) const { return m_isSelectedSyncMode; }

    void setCanPopMenu(bool can);
    void setPopMenuState(bool s) { mCanPopMenu = s; }
    bool getPopMenuState() { return mCanPopMenu; }

private:
    QRect calRect(const int row, const int col);
    QPair<int, int> calCusorPos(const QPoint &pos);
    void clearDragValue();
    QPair<int, int> getSingleWithHeight();
    void emitSelecSignal();
    int calRealRow(); // 根据当前m_row_split、m_col计算实际需要几行

signals:
    void modifyNotify(void);
    void selectPlot();
    void dragpPloting();
    void dragPlotFinish();

protected:
    virtual void resizeEvent(QResizeEvent *event);
    virtual void paintEvent(QPaintEvent *event);

    bool eventFilter(QObject *obj, QEvent *e);
    bool event(QEvent *e);

    virtual void mouseMoveEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);

    // 每次拖动后都更新会产生当前布局的记录
    void recordCurrentLayout();

private slots:
    void onUpdateIndex(int row, int col);

private:
    int m_row_split;
    int m_cols;
    int mRealRow;

    bool m_isCtrlPress;
    bool m_isOnlyShowSelected;
    bool m_isAutoStop;
    bool m_isAllowZoomInXAxis;
    bool m_isAllowZoomInYAxis;
    bool m_isSelectedSyncMode;
    bool m_isMousePoint;
    bool mCanPopMenu = true;

    PlotSyncOperator m_plotSyncOperator;
    QList<QWidget *> m_widgetList;

    // 图表拖动相关
    bool mIsLeftBtnClicked;
    bool mMouseMoved;
    QPoint mMouseClickPos;
    PlotWidget *mSrcPlotWidget;
    QPair<int, int> mDstPos;

    // 记录用户拖动影响的图表历史
    QVector<PosRecord> mOldPosRecord;
};

#endif // PLOTSCROLLAREA_H
