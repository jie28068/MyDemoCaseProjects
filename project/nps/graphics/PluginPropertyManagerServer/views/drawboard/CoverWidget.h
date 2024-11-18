#ifndef COVERWIDGET_H
#define COVERWIDGET_H

#pragma once

#include "customtablewidget.h"
#include <QAction>
#include <QEvent>
#include <QGridLayout>
#include <QLabel>
#include <QMenu>

class PicLabel;

namespace Kcc {
namespace BlockDefinition {
class DrawingBoardClass;
}
}

class CoverWidget : public CWidget
{
    Q_OBJECT
public:
    CoverWidget(QSharedPointer<Kcc::BlockDefinition::DrawingBoardClass> drawboard, QWidget *parent = nullptr);
    ~CoverWidget();
    virtual bool saveData() override;
    virtual void setCWidgetReadOnly(bool bReadOnly) override;
    bool isDataChanged();
private slots:
    void onModelDataChanged(const CustomModelItem &olditem, const CustomModelItem &newitem);
    void onPixmapChange(const QPixmap &pix);

private:
    void initUI();

private:
    QSharedPointer<Kcc::BlockDefinition::DrawingBoardClass> m_drawboard;
    CustomTableWidget *m_tableWidget;
    QLabel *m_bgLabel;
    QLabel *m_previewLabel;
    PicLabel *m_onlyPicLabel;
    PicLabel *m_PicAndBgLabel;
    QString m_colorStr;
    bool m_isChangeData;
};

class PicLabel : public QLabel
{
    Q_OBJECT
public:
    // label类型，仅图片，图片加背景(预览)
    enum LabelType { LabelType_OnlyPic, LabelType_PicAndBG };
    PicLabel(LabelType type, const QPixmap &pix, const QString &tooltips = "", const QString &bgcolor = "#FFFFFF",
             QWidget *parent = nullptr);
    ~PicLabel();
    void updateBgColor(const QString &color);
    void updatePixmap(const QPixmap &pix);
    bool isPixChange();
    QPixmap getPixmap(bool newpix = true);
signals:
    void pixmapChange(const QPixmap &pix);

protected:
    void paintEvent(QPaintEvent *event) override;
private slots:
    void onSelectPic();
    void onRemovePic();
    void onMenuRequest(const QPoint &pos);

private:
    void intUI(const QPixmap &pix);

private:
    LabelType m_type;
    QPixmap m_oldPixMap;
    QPixmap m_newPixMap;
    QMenu *m_menu;
    QString m_bgColor;
};

#endif