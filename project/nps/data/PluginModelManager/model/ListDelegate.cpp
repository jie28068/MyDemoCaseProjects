#include "ListDelegate.h"
#include "ModelToolkit.h"
#include "ModelViewDefine.h"
#include "TreeItem.h"
#include <QPainter>
#include <QPen>
#include <QSvgRenderer>

ListDelegate::ListDelegate(QObject *parent) : QStyledItemDelegate(parent) { }

ListDelegate::~ListDelegate() { }

void ListDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (painter == nullptr || !index.isValid()) {
        return;
    }

    painter->save();

    if (option.state.testFlag(QStyle::State_Selected)) {
        painter->fillRect(option.rect, ModelViewDefine::BGCOLOR_SELECTED);
    } else if (option.state.testFlag(QStyle::State_MouseOver)) {
        painter->fillRect(option.rect, ModelViewDefine::BGCOLOR_MOUSEOVER);
    } else if (!option.state.testFlag(QStyle::State_Enabled)) {
        painter->fillRect(option.rect, ModelViewDefine::BGCOLOR_DISABLE);
    } else {
        painter->fillRect(option.rect, QColor("#FFFFFF"));
    }
    drawNameText(painter, option, index);

    QRect imageRect(option.rect.x() + ((option.rect.width() - 65) / 2), option.rect.y(), 65, 45);
    QRect boxRect(option.rect.x() + ((option.rect.width() - 46) / 2), option.rect.y() + 4, 47, 37);
    QPixmap pixmap;
    if (index.data(ModelViewDefine::ModelType) == CombineBoardModel::Type
        || index.data(ModelViewDefine::ModelType) == ElecCombineBoardModel::Type) {
        // 如果是构造型分组
        if (index.data(ModelViewDefine::TypeRole) == TreeItem::BranchNode) {
            pixmap = QPixmap(":/PluginModelManager/group");
        } else {
            if (!drawBackGround(painter, option, index)) {
                pixmap = QPixmap(":/PluginModelManager/composition");
            }
        }
        painter->drawPixmap(imageRect, pixmap);
        drawPort(painter, option, index.data(Qt::DisplayRole).toString());
    } else {
        // 取消显示类型关键字，直接用有没有图片资源判断
        if (index.data(ModelViewDefine::ImageRole).isNull()) {
            if (index.data(ModelViewDefine::TypeRole) == TreeItem::BranchNode) {
                pixmap = QPixmap(":/PluginModelManager/group");
            } else {
                pixmap = QPixmap(":/PluginModelManager/module");
            }
            painter->drawPixmap(imageRect, pixmap);
            drawPort(painter, option, index.data(Qt::DisplayRole).toString());
        } else {
            QImage image = qvariant_cast<QImage>(index.data(ModelViewDefine::ImageRole));
            if (!image.isNull()) {
                painter->drawPixmap(imageRect, QPixmap::fromImage(image));
            }
        }
    }
    painter->restore();
}

QSize ListDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return ModelViewDefine::ListItemSize;
}

void ListDelegate::drawNameText(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->save();
    QPen pen;
    pen.setColor(ModelViewDefine::TEXTCOLOR_NORMAL);
    pen.setWidth(1);
    painter->setPen(pen);
    QFont nameFont("Arial");
    nameFont.setPixelSize(12);
    painter->setFont(nameFont);
    painter->setBrush(Qt::NoBrush);
    QTextOption my_option(Qt::AlignLeft | Qt::AlignCenter);

    // 绘制模块名称
    QRectF nameTextRect(option.rect.x(), option.rect.y() + ModelViewDefine::InnerBoxSize.height() + 9,
                        option.rect.width(), 23);
    // painter->drawRect(nameTextRect);
    // 用于保存换行后的文本行
    QStringList nameLines;
    QString name = index.data(Qt::DisplayRole).toString();
    if (name.isEmpty()) {
        return;
    }
    QFontMetricsF fontMetrics(painter->font());
    int maxWidth = 74;
    // 行间距
    int space = 2;
    // 中英文 行间距不同
    if (name.contains(QRegExp("[\\x4e00-\\x9fa5]+"))) {
        space = 4;
    }
    // 逐个字符添加到当前行，直到超过最大宽度
    nameLines = getTextLines(name, fontMetrics, maxWidth);
    // 当第二剩余文本是一个或两个字符时要重新计算
    if (nameLines.size() == 2 && (nameLines[1].size() <= 2)) {
        nameLines.clear();
        nameTextRect = QRectF(option.rect.x(), option.rect.y() + ModelViewDefine::InnerBoxSize.height() + 12,
                              option.rect.width() + 6, 20);
        nameLines = getTextLines(name, fontMetrics, maxWidth + 6);
        // 重新计算还是剩余字符小于2 则直接加入最后一行
        if (nameLines.size() == 2 && (nameLines[1].size() <= 2)) {
            nameLines[0] = nameLines[0] + nameLines[1];
            nameLines.removeAt(1);
        }
    }

    // 字体高度 减去两个像素为字体顶部到上基线的距离
    int fontHeight = fontMetrics.ascent() - 2;
    // 文本宽度
    int firLineW = nameTextRect.x() + (nameTextRect.width() - fontMetrics.width(nameLines[0])) / 2;
    int secLineW;
    if (nameLines.size() > 1) {
        secLineW = nameTextRect.x() + (nameTextRect.width() - fontMetrics.width(nameLines[1])) / 2;
    }
    // 手动一行行绘制
    painter->drawText(firLineW, nameTextRect.y() + fontMetrics.ascent(), nameLines[0]);
    if (nameLines.size() == 2) {
        painter->drawText(secLineW, nameTextRect.y() + fontMetrics.ascent() * 2 + space, nameLines[1]);
    } else if (nameLines.size() > 2) {
        QString elidnote = fontMetrics.elidedText(nameLines[1], Qt::ElideRight, maxWidth - 2);
        painter->drawText(nameTextRect.x() + (nameTextRect.width() - fontMetrics.width(elidnote)) / 2,
                          nameTextRect.y() + fontMetrics.ascent() * 2 + space, elidnote);
    }
}

QStringList ListDelegate::getTextLines(const QString &name, const QFontMetricsF &fontMetrics, const int maxWidth) const
{
    QStringList textList;
    // 逐个字符添加到当前行，直到超过最大宽度
    QString currentLine;
    for (const QChar &ch : name) {
        currentLine += ch;
        if (fontMetrics.width(currentLine) > maxWidth) {
            // 当前行超过最大宽度，添加到换行文本列表中
            textList.append(currentLine);
            currentLine.clear();
        }
    }
    if (!currentLine.isEmpty()) {
        textList.append(currentLine);
    }

    return textList;
}

void ListDelegate::drawPort(QPainter *painter, const QStyleOptionViewItem &option, const QString &name) const
{
    QRect boxRect(option.rect.x() + ((option.rect.width() - 47) / 2), option.rect.y() + 4, 47, 37);
    QList<QVector<QPointF>> allPortPoints = getAllPortPoints(boxRect, name);
    if (!allPortPoints.isEmpty()) {
        for (QVector<QPointF> points : allPortPoints) {
            painter->save();
            QPen pen;
            pen.setWidthF(1.3);
            pen.setColor(ModelViewDefine::TEXTCOLOR_NORMAL);
            pen.setJoinStyle(Qt::RoundJoin);
            pen.setCapStyle(Qt::RoundCap);
            pen.setCosmetic(true);
            painter->setPen(pen);
            painter->setRenderHint(QPainter::Antialiasing);
            painter->drawPolyline(points);
            painter->restore();
        }
    }
}

QVector<QPointF> ListDelegate::getPoints(const qreal &pointX, const qreal &pointY) const
{
    QVector<QPointF> points;
    QRectF arrowsRect = QRectF(pointX, pointY, 3.75, 4.5);
    points.append(QPointF(arrowsRect.x(), arrowsRect.y()));
    points.append(QPointF(arrowsRect.x() + arrowsRect.width(), arrowsRect.y() + arrowsRect.height() / 2));
    points.append(QPointF(arrowsRect.x(), arrowsRect.y() + arrowsRect.height()));
    return points;
}

QList<QVector<QPointF>> ListDelegate::getAllPortPoints(const QRectF &rect, const QString &name) const
{
    QVector<QPointF> points;
    QList<QVector<QPointF>> allPortPoints;

    PModel model = getModel(name);
    if (model == nullptr) {
        return QList<QVector<QPointF>>();
    }
    PVariableGroup groupIn = model->getVariableGroup(RoleDataDefinition::InputSignal);
    PVariableGroup groupOut = model->getVariableGroup(RoleDataDefinition::OutputSignal);
    if (!groupIn.isNull()) {
        QList<PVariable> inParamList =
                model->getVariableGroup(RoleDataDefinition::InputSignal)->getVariableSortByOrder();
        allPortPoints.append(getSinglePortPoints(inParamList, portType::inPut, rect));
    }
    if (!groupOut.isNull()) {
        QList<PVariable> outParamList =
                model->getVariableGroup(RoleDataDefinition::OutputSignal)->getVariableSortByOrder();
        allPortPoints.append(getSinglePortPoints(outParamList, portType::outPut, rect));
    }

    return allPortPoints;
}

QList<QVector<QPointF>> ListDelegate::getSinglePortPoints(const QList<PVariable> &paramList, const portType &portType,
                                                          const QRectF &rect) const
{
    qreal pointX, pointY;
    QList<QVector<QPointF>> allPortPoints;
    if (portType == portType::outPut) {
        pointX = rect.x() + rect.width() + 1;
    } else if (portType == portType::inPut) {
        pointX = rect.x() - 5;
    }
    if (paramList.size() == 1) {
        QVector<QPointF> points = getPoints(pointX, rect.y() + rect.height() / 2 - 3);
        allPortPoints.append(points);
    } else if (paramList.size() == 2) {
        QVector<QPointF> points1 = getPoints(pointX, rect.y() + 3);
        QVector<QPointF> points2 = getPoints(pointX, rect.y() + rect.height() - 6);
        allPortPoints << points1 << points2;
        // 端口数量大于三就使用平均分配的方式（目前端口大于三也只画三个接口）
    } else if (paramList.size() >= 3) {
        QVector<QPointF> points1 = getPoints(pointX, rect.y() + (rect.height() - 6) / 2);
        QVector<QPointF> points2 = getPoints(pointX, rect.y() + 3);
        QVector<QPointF> points3 = getPoints(pointX, rect.y() + rect.height() - 6);
        allPortPoints << points1 << points2 << points3;
    }
    return allPortPoints;
}

PModel ListDelegate::getModel(const QString &name) const
{
    auto m_pModelServer = RequestServer<IModelManagerServer>();
    auto m_pProjectServer = RequestServer<IProjectManagerServer>();
    if (!m_pProjectServer || !m_pModelServer) {
        return PModel();
    }

    PModel model = m_pModelServer->GetToolkitModel(name, KL_TOOLKIT::CONTROL_TOOLKIT);
    if (!model) {
        model = m_pModelServer->GetToolkitModel(name, KL_TOOLKIT::ELECTRICAL_TOOLKIT);
    }
    if (!model) {
        PKLProject project = m_pProjectServer->GetCurProject();
        model = project->getModel(name);
    }

    if (!model) {
        return PModel();
    }
    return model;
}

bool ListDelegate::drawBackGround(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    bool isDepict = false;
    // 构造模块背景色
    PModel model = nullptr;
    model = getModel(index.data(Qt::DisplayRole).toString());
    if (model) {
        QString modelColor = model->getResource()[CANVAS_BACKGROUND_COLOR].toString();
        if (!modelColor.isEmpty()) {
            QRect boxRect(option.rect.x() + ((option.rect.width() - 46) / 2), option.rect.y() + 4, 46, 36);
            painter->fillRect(boxRect, QColor(modelColor));
            isDepict = true;
        }
    } else {
        return false;
    }

    // 构造模块如果有封面
    QVariant pixv = model->getResource()[CANVAS_BACKGROUND_IMAGE];
    if (!pixv.isNull()) {
        QImage image = qvariant_cast<QImage>(pixv);
        if (!image.isNull()) {
            QRect pixRect(option.rect.x() + ((option.rect.width() - 41) / 2), option.rect.y() + 6, 41, 31);
            QPixmap pixMap = QPixmap::fromImage(image);
            QPixmap drawpix = pixMap.scaled(QSize(41, 31), Qt::KeepAspectRatio);
            QSize drawpixSize = drawpix.size();
            QPoint pos;
            pos.setX(pixRect.x() + (pixRect.width() - drawpixSize.width()) / 2);
            pos.setY(pixRect.y() + (pixRect.height() - drawpixSize.height()) / 2);
            painter->drawPixmap(pos, drawpix);
            isDepict = true;
        }
    }
    if (isDepict) {
        // 画内部框
        QRect innerRect(option.rect.x() + ((option.rect.width() - 41) / 2), option.rect.y() + 6, 41, 31);
        painter->drawRect(innerRect);
        // 画外部框
        QRect boxRect(option.rect.x() + ((option.rect.width() - 46) / 2), option.rect.y() + 4, 45, 35);
        painter->drawRect(boxRect);
    }

    return isDepict;
}