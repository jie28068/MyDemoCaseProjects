#include "ComponentListDelegate.h"
#include "GlobalDefinition.h"
#include <QFileInfo>
#include <QStyleOptionViewItemV3>
#include <QtSvg/QSvgRenderer>
#if WIN32
#include <Windows.h>
#endif

#if WIN32
#include <WinGDI.h>
#include <WinUser.h>
#endif

USE_LOGOUT_("ComponentListDelegate")

PModel ComponentDelegate::getModel(const QString &name, modelType type) const
{
    auto m_pModelServer = RequestServer<IModelManagerServer>();
    if (!m_pModelServer) {
        LOGOUT(QObject::tr("IModelManagerServer unregistered"), LOG_ERROR);
        return PModel();
    }
    auto m_pProjectServer = RequestServer<IProjectManagerServer>();
    if (!m_pProjectServer) {
        LOGOUT(QObject::tr("IProjectManagerServer unregistered"), LOG_ERROR);
        return PModel();
    }

    PModel model;
    if (type == modelType::Control) {
        model = m_pModelServer->GetToolkitModel(name, KL_TOOLKIT::CONTROL_TOOLKIT);
        PKLProject project = m_pProjectServer->GetCurProject();
        if (!model) {
            model = project->getModel(name);
        }
    } else {
        model = m_pModelServer->GetToolkitModel(name, KL_TOOLKIT::ELECTRICAL_TOOLKIT);
    }

    if (!model) {
        return PModel();
    }
    return model;
}

void ComponentDelegate::drawFrame(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index,
                                  modelType type) const
{
    QPen pen;
    pen.setWidth(1);
    // 模块边框
    QRectF innerRect(option.rect.x() + 12, option.rect.y() + 7, option.rect.width() - 24, option.rect.height() - 14);
    if (index.data(Global::DragEnabledRole).toBool()) {
        // painter->fillRect(fillRect, QColor(BGCOLOR_SELECTED));
        pen.setColor(Global::TEXTCOLOR_NORMAL);
        painter->setPen(pen);
        painter->drawRect(innerRect);
        pen.setColor(Global::TEXTCOLOR_NORMAL);
        painter->setPen(pen);
    } else {
        painter->fillRect(innerRect, QColor(Global::BGCOLOR_DISABLE));
        pen.setColor(Global::BOARDCOLOR_DISABLE);
        painter->setPen(pen);
        painter->drawRect(innerRect);
        pen.setColor(Global::TEXTCOLOR_DISABLE);
        painter->setPen(pen);
    }
    // 构造型模块有两个框
    QString boardType;
    if (type == modelType::Control) {
        boardType = Global::str_ControllerGroupCustomBoard;
    } else if (type == modelType::Elc) {
        boardType = Global::str_DeviceGroupLocalCombine;
    }
    if (index.data(Global::GroupNameRole).toString() == boardType) {
        QRectF inRect(option.rect.x() + 15, option.rect.y() + 10, option.rect.width() - 30, option.rect.height() - 20);
        if (index.data(Global::DragEnabledRole).toBool()) {
            pen.setColor(Global::TEXTCOLOR_NORMAL);
            painter->setPen(pen);
        } else {
            pen.setColor(Global::BOARDCOLOR_DISABLE);
            painter->setPen(pen);
        }
        painter->drawRect(inRect);
    }
}

bool ComponentDelegate::drawBackGround(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index,
                                       modelType type) const
{
    bool isDepict = false;
    // 构造模块背景色
    PModel model = nullptr;
    QString boardType;
    if (type == modelType::Control) {
        boardType = Global::str_ControllerGroupCustomBoard;
    } else if (type == modelType::Elc) {
        boardType = Global::str_DeviceGroupLocalCombine;
    }

    if (index.data(Global::GroupNameRole).toString() == boardType) {
        model = getModel(index.data(Global::NameRole).toString(), modelType::Control);
        if (model) {
            QString modelColor = model->getResource()[CANVAS_BACKGROUND_COLOR].toString();
            if (!modelColor.isEmpty()) {
                QRect customBoardRect(option.rect.x() + 12, option.rect.y() + 7, option.rect.width() - 24,
                                      option.rect.height() - 14);
                painter->fillRect(customBoardRect, QColor(modelColor));
            }
        }

        if (index.data(Global::UrlRole).isNull() && model != nullptr) {
            // 构造模块如果有封面
            QVariant pixv = model->getResource()[CANVAS_BACKGROUND_IMAGE];
            if (!pixv.isNull()) {
                QImage image = qvariant_cast<QImage>(pixv);
                if (!image.isNull()) {
                    QRect pixRect(option.rect.x() + 16, option.rect.y() + 11, option.rect.width() - 31,
                                  option.rect.height() - 21);
                    QPixmap pixMap = QPixmap::fromImage(image);
                    QPixmap drawpix = pixMap.scaled(QSize(option.rect.width() - 31, option.rect.height() - 21),
                                                    Qt::KeepAspectRatio);
                    QSize drawpixSize = drawpix.size();
                    QPoint pos;
                    pos.setX(pixRect.x() + (pixRect.width() - drawpixSize.width()) / 2);
                    pos.setY(pixRect.y() + (pixRect.height() - drawpixSize.height()) / 2);
                    painter->drawPixmap(pos, drawpix);
                    drawFrame(painter, option, index, type);
                    if (!index.data(Global::ReadableNameRole).toString().isEmpty()
                        && boardType == Global::str_ControllerGroupCustomBoard)
                        drawPort(painter, option, index.data(Global::ReadableNameRole).toString());
                    isDepict = true;
                }
            }
        }
    }
    return isDepict;
}

void ComponentDelegate::drawText(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index,
                                 modelType type) const
{
    if (painter == nullptr || !index.isValid()) {
        return;
    }
    QPen pen;
    pen.setColor(Global::TEXTCOLOR_NORMAL);
    pen.setWidth(1);
    painter->setPen(pen);
    QFont nameFont("Arial");
    nameFont.setPixelSize(10);
    painter->setFont(nameFont);
    painter->setBrush(Qt::NoBrush);

    drawFrame(painter, option, index, type);

    QRectF textRect(option.rect.x() + 15, option.rect.y() + 10, option.rect.width() - 30, option.rect.height() - 20);
    QFontMetricsF fontMetrics(painter->font());

    QStringList nameLines; // 用于保存换行后的文本行
    QString name = index.data(Global::ReadableNameRole).toString();
    int maxWidth = 30;

    // 逐个字符添加到当前行，直到超过最大宽度
    if (!name.isEmpty()) {
        nameLines = getTextLines(name, fontMetrics, 30);
    } else {
        return;
    }
    int endIndex = nameLines.size() - 1;
    // 原型名不为空 size至少为1
    if (endIndex < 0) {
        return;
    }
    // 构造模块名称可以带中文字符 不需要这样处理
    if (index.data(Global::GroupNameRole).toString() != Global::str_ControllerGroupCustomBoard) {
        // 当第二、三行剩余文本是一个或两个字符时要重新计算
        if (nameLines[endIndex].size() <= 2 && (nameLines.size() > 1 && nameLines.size() <= 3)) {
            nameLines.clear();
            textRect = QRectF(option.rect.x() + 12, option.rect.y() + 10, option.rect.width() - 24,
                              option.rect.height() - 20);
            nameLines = getTextLines(name, fontMetrics, 36);
            endIndex = nameLines.size() - 1;
            if (endIndex < 0) {
                return;
            }
            // 重新计算还是剩余字符小于2 则直接加入最后一行
            if (nameLines[endIndex].size() <= 2 && (nameLines.size() > 1 && nameLines.size() <= 3)) {
                if ((endIndex - 1) >= 0) {
                    nameLines[endIndex - 1] = nameLines[endIndex - 1] + nameLines[endIndex];
                    nameLines.removeAt(endIndex);
                }
            }
        }
    }
    // 行间距
    int space;
    // 构造模块可以是汉字 判断如果有汉字字体 设为9号字 间距设成2
    if (index.data(Global::GroupNameRole).toString() == Global::str_ControllerGroupCustomBoard
        || index.data(Global::GroupNameRole).toString() == Global::str_DeviceGroupLocalCombine) {
        bool isChinese = false;
        if (name.contains(QRegExp("[\\x4e00-\\x9fa5]+"))) {
            isChinese = true;
            nameFont.setPixelSize(9);
            painter->setFont(nameFont);
            QFontMetricsF fontMetrics1(painter->font());
            nameLines = getTextLines(name, fontMetrics1, 30);
            fontMetrics = fontMetrics1;
        }
        if (isChinese) {
            space = 4;
        } else {
            space = 1;
        }

    } else {
        space = 1;
    }

    // 字体高度 减去两个像素为字体顶部到上基线的距离
    int fontHeight = fontMetrics.ascent() - 2;
    // 注意字体绘制的基线在底部
    // 文本在中间时y坐标(文本矩形高度+字体高度)/2
    int middlePointY = textRect.y() + (textRect.height() + fontHeight) / 2;
    // 文本宽度
    int firLineW = textRect.x() + (textRect.width() - fontMetrics.width(nameLines[0])) / 2;
    int secLineW;
    int thrLineW;
    if (nameLines.size() > 1) {
        secLineW = textRect.x() + (textRect.width() - fontMetrics.width(nameLines[1])) / 2;
        if (nameLines.size() > 2)
            thrLineW = textRect.x() + (textRect.width() - fontMetrics.width(nameLines[2])) / 2;
    }
    // 绘制矩形的中点y坐标
    int rectMiddleY = textRect.y() + textRect.height() / 2;

    // 手动一行行绘制
    if (nameLines.size() == 1) {
        painter->drawText(firLineW, middlePointY, nameLines[0]);
    } else if (nameLines.size() == 2) {
        painter->drawText(firLineW, rectMiddleY - space / 2, nameLines[0]);
        painter->drawText(secLineW, rectMiddleY + fontHeight + space / 2, nameLines[1]);
    } else if (nameLines.size() >= 3) {
        painter->drawText(firLineW, middlePointY - fontMetrics.ascent() + 2 - space, nameLines[0]);
        painter->drawText(secLineW, middlePointY, nameLines[1]);
        if (nameLines.size() == 3) {
            painter->drawText(thrLineW, middlePointY + space + fontHeight, nameLines[2]);
        } else if (nameLines.size() > 3) {
            QString elidnote = fontMetrics.elidedText(nameLines[2], Qt::ElideRight, maxWidth - 2);
            painter->drawText(textRect.x() + (textRect.width() - fontMetrics.width(elidnote)) / 2,
                              middlePointY + space + fontHeight, elidnote);
        }
    }

    if (type == modelType::Control) {
        // 绘制端口
        drawPort(painter, option, name);
    }
}

QStringList ComponentDelegate::getTextLines(const QString &name, const QFontMetricsF &fontMetrics,
                                            const int maxWidth) const
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
///////////////////////////////////////////
ComponentListDelegate::ComponentListDelegate(QObject *parent) : m_nFontSize(9)
{
#if WIN32
    HDC hd = GetDC(NULL);
    int dotPixX = GetDeviceCaps(hd, LOGPIXELSX);
    switch (dotPixX) {
    case 96: // 100%
        m_nFontSize = 9;
        break;
    case 120: // 125%
        m_nFontSize = 7;
        break;
    case 144: // 150%
        m_nFontSize = 5;
        break;
    case 168: // 175%
        m_nFontSize = 3;
        break;
    }
    ReleaseDC(NULL, hd);
#endif
}

void ComponentListDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->save();
    QPen pen;
    pen.setWidth(1);
    // 外边延伸2px,边框1px,图片大小65*45,QT描绘的时候，宽高都会多1px，所以减5，而不是4；
    QRect drawRect(option.rect.x() + 2, option.rect.y() + 2, option.rect.width() - 5, option.rect.height() - 5);
    if (index.data(Global::DragEnabledRole).toBool()) {
        if (option.state.testFlag(QStyle::State_Selected)) {
            /* pen.setColor(Global::BOARDCOLOR_SELECTED);
             painter->setPen(pen);*/
            painter->fillRect(option.rect, Global::BGCOLOR_SELECTED);
        } else if (option.state.testFlag(QStyle::State_MouseOver)) {
            /*pen.setColor(Global::BOARDCOLOR_UNSELECTED);
            painter->setPen(pen);*/
            // 鼠标移动上去的时候，有外边2px，所以图片大小65*45加上四周外边2px，item大小为71*51
            painter->fillRect(option.rect, Global::BGCOLOR_MOUSEOVER);
        } else {
            /*pen.setColor(Global::BOARDCOLOR_UNSELECTED);
            painter->setPen(pen);*/
            painter->fillRect(drawRect, QColor("#FFFFFF"));
        }
    } else {
        // 不能拖拽置灰
        /*pen.setColor(Global::BOARDCOLOR_DISABLE);
painter->setPen(pen);*/
    }
    // painter->drawRect(drawRect);//取消边框

    // 构造模块背景色
    PModel model = nullptr;
    if (index.data(Global::GroupNameRole).toString() == Global::str_ControllerGroupCustomBoard) {
        model = getModel(index.data(Global::NameRole).toString(), modelType::Control);
        if (model) {
            QString modelColor = model->getResource()[CANVAS_BACKGROUND_COLOR].toString();
            if (!modelColor.isEmpty()) {
                QRect customBoardRect(option.rect.x() + 12, option.rect.y() + 7, option.rect.width() - 24,
                                      option.rect.height() - 14);
                painter->fillRect(customBoardRect, QColor(modelColor));
            }
        }
    }
    bool isDepict = drawBackGround(painter, option, index, modelType::Control);
    // 取消显示类型关键字，直接用有没有图片资源判断
    if (index.data(Global::UrlRole).isNull()) {
        if (isDepict == false) {
            drawText(painter, option, index, modelType::Control);
        }
    } else {
        QImage image = qvariant_cast<QImage>(index.data(Global::UrlRole));
        // png描绘
        if (!image.isNull()) {
            // QT画pix是按照实际大小来，所以宽高减4
            QRect pixRect(option.rect.x() + 3, option.rect.y() + 3, option.rect.width() - 6, option.rect.height() - 6);
            painter->drawPixmap(pixRect, QPixmap::fromImage(image));
        } else {
            drawText(painter, option, index, modelType::Control);
        }
    }

    painter->restore();
}

QSize ComponentListDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QSize(71, 51);
}

QVector<QPointF> ComponentListDelegate::getPoints(const qreal &pointX, const qreal &pointY) const
{
    QVector<QPointF> points;
    QRectF arrowsRect = QRectF(pointX, pointY, 3.75, 4.5);
    points.append(QPointF(arrowsRect.x(), arrowsRect.y()));
    points.append(QPointF(arrowsRect.x() + arrowsRect.width(), arrowsRect.y() + arrowsRect.height() / 2));
    points.append(QPointF(arrowsRect.x(), arrowsRect.y() + arrowsRect.height()));
    return points;
}

QList<QVector<QPointF>> ComponentListDelegate::getAllPortPoints(const QRectF &rect, const QString &name) const
{
    QVector<QPointF> points;
    QList<QVector<QPointF>> allPortPoints;

    PModel model = getModel(name, modelType::Control);
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

QList<QVector<QPointF>> ComponentListDelegate::getSinglePortPoints(const QList<PVariable> &paramList,
                                                                   const portType &portType, const QRectF &rect) const
{
    qreal pointX, pointY;
    QList<QVector<QPointF>> allPortPoints;
    if (portType == portType::outPut) {
        // 4像素是文本矩形到内部框的距离+框线宽+1像素间隔效果
        pointX = rect.x() + rect.width() + 5;
    } else if (portType == portType::inPut) {
        // 8像素是文本框到内部框距离+框线宽+箭头宽
        pointX = rect.x() - 8;
    }
    if (paramList.size() == 1) {
        QVector<QPointF> points = getPoints(pointX, rect.y() + rect.height() / 2 - 3);
        allPortPoints.append(points);
    } else if (paramList.size() == 2) {
        QVector<QPointF> points1 = getPoints(pointX, rect.y());
        QVector<QPointF> points2 = getPoints(pointX, rect.y() + rect.height() - 6);
        allPortPoints << points1 << points2;
        // 端口数量大于三就使用平均分配的方式（目前端口大于三也只画三个接口）
    } else if (paramList.size() >= 3) {
        QVector<QPointF> points1 = getPoints(pointX, rect.y() + (rect.height() - 6) / 2);
        QVector<QPointF> points2 = getPoints(pointX, rect.y());
        QVector<QPointF> points3 = getPoints(pointX, rect.y() + rect.height() - 6);
        allPortPoints << points1 << points2 << points3;
    }
    // else if (paramList.size() > 3) { // 端口数量
    //     int total = paramList.size();
    //     // 每个端口所占空间
    //     int space = 0;
    //     // 剩余空间 平均分到每个间隔中
    //     int remainder = qint16(rect.height()) % total / (total - 1);
    //     for (int count = 0; count < total; count++) {
    //         QVector<QPointF> points = getPoints(pointX, rect.y() + space);
    //         space = rect.height() / total + space + remainder;
    //         allPortPoints.append(points);
    //     }
    // }
    return allPortPoints;
}

void ComponentListDelegate::drawPort(QPainter *painter, const QStyleOptionViewItem &option, const QString &name) const
{
    QRectF textRect =
            QRectF(option.rect.x() + 15, option.rect.y() + 10, option.rect.width() - 30, option.rect.height() - 20);
    QList<QVector<QPointF>> allPortPoints = getAllPortPoints(textRect, name);
    if (!allPortPoints.isEmpty()) {
        for (QVector<QPointF> points : allPortPoints) {
            painter->save();
            QPen pen;
            pen.setWidthF(1.3);
            pen.setColor(Global::TEXTCOLOR_NORMAL);
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
//////////////////////////////////////////////////////////////////////////

ComponentListIconDelegate::ComponentListIconDelegate(QObject *parent) { }

void ComponentListIconDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                                      const QModelIndex &index) const
{
    painter->save();
    QPen pen;
    pen.setWidth(1);
    QRectF drawRect(option.rect.x() + 2, option.rect.y() + 2, option.rect.width() - 5, option.rect.height() - 5);
    if (index.data(Global::DragEnabledRole).toBool()) {
        if (option.state.testFlag(QStyle::State_Selected)) {
            painter->fillRect(option.rect, Global::BGCOLOR_SELECTED);
        } else if (option.state.testFlag(QStyle::State_MouseOver)) {
            painter->fillRect(option.rect, Global::BGCOLOR_MOUSEOVER);
        } else {
            painter->fillRect(drawRect, QColor("#FFFFFF"));
        }
    } else {
        // 不能拖拽置灰
        //  painter->fillRect(drawRect, QColor("#E2E2E2"));
        /*pen.setColor(Global::BOARDCOLOR_DISABLE);
painter->setPen(pen);*/
    }

    bool isDepict = drawBackGround(painter, option, index, modelType::Elc);
    ;
    // 取消显示类型关键字，直接用有没有图片资源判断
    if (index.data(Global::UrlRole).isNull()) {
        if (isDepict == false) {
            drawText(painter, option, index, modelType::Elc);
        }
    } else {
        QImage image = qvariant_cast<QImage>(index.data(Global::UrlRole));
        // png描绘
        if (!image.isNull()) {
            // QT画pix是按照实际大小来，所以宽高减4
            QRect pixRect(option.rect.x() + 3, option.rect.y() + 3, option.rect.width() - 6, option.rect.height() - 6);
            painter->drawPixmap(pixRect, QPixmap::fromImage(image));
        } else {
            // svg描绘
            //  SVG等比放大到绘制区域
            QSvgRenderer render;
            if (index.data(Global::DragEnabledRole).toBool()) {
                render.load(index.data(Global::UrlRole).toByteArray());
            } else {
                render.load(index.data(Global::UrlNoDragRole).toByteArray());
            }
            QSize sizeSVG = render.defaultSize();
            qreal scale = 46.0f / 56.0f; //
            // 56为所有器件中最大SVG图片的高度，46为使用的格子高度（此功能是为了窗口中器件保持SVG器件的比例大小，而不是统一铺满一格）
            // qMin(drawRect.width() / sizeSVG.width(), drawRect.height() / sizeSVG.height());
            QRectF svgRect(0, 0, sizeSVG.width() * scale, sizeSVG.height() * scale);
            // QRectF svgRect(0, 0, sizeSVG.width(), sizeSVG.height());
            svgRect.moveCenter(drawRect.center() + QPoint(1, 1));
            render.render(painter, svgRect);
            // painter->drawPixmap(option.rect, QPixmap(listURL.at(0)));
        }
    }
    painter->restore();
}

QSize ComponentListIconDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QSize(71, 51);
}
