#ifndef COMMENTEDITWDIGET_H
#define COMMENTEDITWDIGET_H

#include "GraphicsModelingKernel/SourceProxy.h"
#include <QAbstractListModel>
#include <QComboBox>
#include <QDebug>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QModelIndex>
#include <QObject>
#include <QPainter>
#include <QStandardItemModel>
#include <QStyledItemDelegate>

class ColorLabel : public QLabel
{
    Q_OBJECT
public:
    ColorLabel(QColor color, QWidget *parent = nullptr);
    ~ColorLabel();

    void updateColor(QColor color);

protected:
    void paintEvent(QPaintEvent *) override;

public:
    QColor m_color; // 颜色
};

class ColorWidget : public QLineEdit
{
    Q_OBJECT
public:
    ColorWidget(QColor color, QWidget *parent = nullptr);
    ~ColorWidget();

    QString getColor() const;

protected:
    void mouseDoubleClickEvent(QMouseEvent *e);

private:
    QLabel *m_pLabel;
    ColorLabel *m_pCssLabel;
    QColor m_color;
};

class CommentEditWdiget : public QWidget
{
    Q_OBJECT
public:
    CommentEditWdiget(QSharedPointer<SourceProxy> pSource, QWidget *parent = nullptr);

protected:
    void initDataList();

    void comboBoxFillStyleItems(QComboBox *box);

    void setSourceProxy(QString sourceUUID, QString boardUUID);

protected:
    QGridLayout *m_pGridLayout;
    QVector<qreal> m_vecWidthList;
    // QStringList m_strLineStyleList;
    // QStringList itemNameList;

    QSharedPointer<SourceProxy> m_pSourceProxy;
};

typedef struct infomation {
    QString number;
    int lineValue;
} UserInfo;

class QListmodel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit QListmodel(QObject *parent = 0);
    ~QListmodel();

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    void setModelData(const QList<UserInfo> &);

private:
    QList<UserInfo> InfoList;
};

class comboboxDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit comboboxDelegate(QObject *parent = 0, int type = 0);
    ~comboboxDelegate();

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
    QSize size;
    int ItemHeight;
    int delegateType; // 0:代理线性选择框 1::代理线宽选择框
};
class customLineStyleCombobox : public QComboBox
{
    Q_OBJECT
public:
    customLineStyleCombobox(QWidget *parent = nullptr);
    ~customLineStyleCombobox();

    QList<int> getStyleList();
    int findIndexByItemText(QString text);

protected:
    void paintEvent(QPaintEvent *e);
private slots:
    void onCurIndexChange(int);

private:
    QList<int> m_lineStyleList;
    QStringList itemNameList;
};

class customLineWidthCombobox : public QComboBox
{
    Q_OBJECT
public:
    customLineWidthCombobox(QWidget *parent = nullptr);
    ~customLineWidthCombobox();

    QList<int> getWidthList();
    int findIndexByWidth(int width);

protected:
    void paintEvent(QPaintEvent *e);
private slots:
    void onCurIndexChange(int);

private:
    QList<int> m_lineWidthList;
};

#endif // COMMENTEDITWDIGET_H