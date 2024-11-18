#ifndef TREEDELEGATE_H
#define TREEDELEGATE_H

#include "CoreLib/ServerManager.h"
#include "ModelManagerServer/IModelManagerServer.h"
#include "server/ProjectManagerServer/IProjectManagerServer.h"
#include <QStyledItemDelegate>

using namespace Kcc::ModelManager;
using namespace Kcc::ProjectManager;
class ListDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    ListDelegate(QObject *parent);
    ~ListDelegate();

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
    enum portType { inPut = 0, outPut };
    enum modelType { Elc, Control };
private:
    void drawNameText(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QStringList getTextLines(const QString &name, const QFontMetricsF &fontMetrics, const int maxWidth) const;
    void drawPort(QPainter *painter, const QStyleOptionViewItem &option, const QString &name) const;
    QList<QVector<QPointF>> getSinglePortPoints(const QList<PVariable> &paramList, const portType &portType,
                                                const QRectF &rect) const;
    QList<QVector<QPointF>> ListDelegate::getAllPortPoints(const QRectF &rect, const QString &name) const;
    QVector<QPointF> ListDelegate::getPoints(const qreal &pointX, const qreal &pointY) const;
    PModel getModel(const QString &name) const;
    bool drawBackGround(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif