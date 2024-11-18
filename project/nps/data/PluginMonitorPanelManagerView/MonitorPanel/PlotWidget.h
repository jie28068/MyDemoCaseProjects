#ifndef PLOTWIDGET_H
#define PLOTWIDGET_H

#include <QAbstractTableModel>
#include <QSplitter>
#include <QStyledItemDelegate>
#include <QTabWidget>
#include <QTableView>
#include <QWidget>

#include "ItemPlot.h"
#include "KLWidgets/KCustomDialog.h"

const int gMaxMarketNum = 100;

class MarkerTableView;
class MarkerDialog;

class DataAnalyseTableView;

class PlotWidget : public QWidget
{
    Q_OBJECT

public:
    PlotWidget(QString strTitle, QWidget *parent = nullptr);
    ~PlotWidget();

    ItemPlot *plot() { return m_plot; }

    void setMarkerTableViewShow(bool isShow);
    bool markerTableViewShow(void) const { return m_isShowMarkerTableView; }

    void showDataAnalyseInfoPage(DataInfoType dit);
    // void addMarker(const MarkerInfo& info);
    // void setMarkerInfo(const QList<MarkerInfo>& infos);
    // QList<MarkerInfo> getMarkerInfo(void)const;

    void setSelectState(bool Seleted);
    bool getSelectState() { return mIsSelected; }

    QPair<int, int> getPos();
    void setPos(int row, int col);

protected:
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);

Q_SIGNALS:
    void selectStateChange();

protected slots:
    void onSetMarkerTableViewShow(bool b);

    void hideMarkerTable(); ////2022.11.15

private:
    void privateConnect();
    virtual void paintEvent(QPaintEvent *e);

private:
    ItemPlot *m_plot;
    MarkerTableView *m_markerTableView;
    bool m_isShowMarkerTableView;
    MarkerDialog *m_markerDlg;
    DataAnalyseTableView *m_dataAnalyseTableView;
    QTabWidget *m_tabWidget;
    bool mIsSelected;
    PlotToolbar *m_toolbar;
};

class MarkerDialog : public KCustomDialog
{
    Q_OBJECT

public:
    MarkerDialog(QWidget *p = nullptr) { }
    ~MarkerDialog() { }
};

typedef struct IsModified // 2022.11.11
{
    IsModified() : x(false), y(false), track(false) { }

    bool x;
    bool y;
    bool track;
} IsModified, *pIsModified;

// 游标窗口自定义表格模型
class MarkerTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    MarkerTableModel(QObject *parent = nullptr);
    ~MarkerTableModel();

    virtual int rowCount(const QModelIndex &parent) const override;
    virtual int columnCount(const QModelIndex &parent) const override;
    virtual QVariant data(const QModelIndex &index, int role) const override;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role /*= Qt::EditRole */) override;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    QList<MarkerInfo> getMarkerInfo(void) const;

signals:
    void modifyMarkerName(const QString &markerId, const QString &name);
    void modifyMarkerColor(const QString &markerId, const QColor &color);
    void modifyMarkerIsShow(const QString &markerId, bool show);
    void removeMarker(const QString &markerId);

    void hideView(); // 2022.11.15

public slots:
    void setMarkerInfo(const QList<MarkerInfo> &info);
    void setCurveNames(const QStringList &curveNames);
    void addMarker(const MarkerInfo &info);
    void refreshMarkerPoint(const QString &markerId, qreal pos, const QList<MarkerPoint> &points);

    void setAxisDecimal(int xDecimal, int yDecimal);

protected:
    QString formatValue(int axis, qreal value) const;
    void curveColumnsShowOrHide(void);

private:
    QStringList m_curveNames;
    QStringList m_headerList;
    QList<MarkerInfo> m_markerList;

    QIcon m_typeIcon[3];

    int m_xDecimal;
    int m_yDecimal;

    QList<int> xAxisPos;
    QList<int> yAxisPos;
    QList<int> trackPos;

    pIsModified pModified; // 是否已执行过删除游标操作	//2022.11.11
};

// 游标窗口自定义表格视图
class MarkerTableView : public QTableView
{
    Q_OBJECT

public:
    MarkerTableView(QWidget *parent = nullptr);
    ~MarkerTableView();

    MarkerTableModel *getModel(void) const { return m_model; }

protected:
private:
    MarkerTableModel *m_model;
};

class MarkerTableButtonDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    MarkerTableButtonDelegate(QObject *parent = nullptr);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option,
                     const QModelIndex &index) override;

private:
    void initButton(void);
    QScopedPointer<QStyleOptionButton> m_removeButton;
};

class MarkerTableColorDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    MarkerTableColorDelegate(QObject *parent = nullptr);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option,
                     const QModelIndex &index) override;

private:
};

#endif // PLOTWIDGET_H
