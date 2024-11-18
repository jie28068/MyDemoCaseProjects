#ifndef MYSTYLEDITEMDELEGATE_H
#define MYSTYLEDITEMDELEGATE_H

#include <QAbstractTextDocumentLayout>
#include <QObject>
#include <QPainter>
#include <QPen>
#include <QStyledItemDelegate>
#include <QTextBlock>
#include <QTextBlockFormat>
#include <QTextCursor>
#include <QTextDocument>
#include <QToolButton>

#include "CoreLib/ServerManager.h"
#include "ModelManagerServer/IModelManagerServer.h"
#include "server/ProjectManagerServer/IProjectManagerServer.h"

using namespace Kcc::ModelManager;
using namespace Kcc::ProjectManager;

class ComponentDelegate : public QStyledItemDelegate
{

public:
    enum modelType { Elc, Control };

public:
    ComponentDelegate() {};
    PModel getModel(const QString &name, modelType type) const;
    void drawFrame(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index,
                   modelType type) const;
    bool drawBackGround(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index,
                        modelType type) const;
    void drawText(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index,
                  modelType type) const;
    QStringList getTextLines(const QString &name, const QFontMetricsF &fontMetrics, const int maxWidth) const;
    virtual void drawPort(QPainter *painter, const QStyleOptionViewItem &option, const QString &name) const = 0;
};

// 列表视图中的控制模块的delegate，绘制矩形和文本
class ComponentListDelegate : public ComponentDelegate
{
    Q_OBJECT
public:
    ComponentListDelegate(QObject *parent);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    virtual void drawPort(QPainter *painter, const QStyleOptionViewItem &option, const QString &name) const;

public:
    enum portType { inPut = 0, outPut };

private:
    QVector<QPointF> getPoints(const qreal &pointX, const qreal &pointY) const;
    QList<QVector<QPointF>> getAllPortPoints(const QRectF &rect, const QString &name) const;
    QList<QVector<QPointF>> getSinglePortPoints(const QList<PVariable> &paramList, const portType &portType,
                                                const QRectF &rect) const;

private:
    int m_nFontSize;
};

// 列表视图中的电气和测量模块的delegate，绘制图片
class ComponentListIconDelegate : public ComponentDelegate
{
    Q_OBJECT
public:
    ComponentListIconDelegate(QObject *parent);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    virtual void drawPort(QPainter *painter, const QStyleOptionViewItem &option, const QString &name) const {};
};
#endif // MYSTYLEDITEMDELEGATE_H
