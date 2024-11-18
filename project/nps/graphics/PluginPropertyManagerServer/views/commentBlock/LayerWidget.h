#ifndef LAYERWIDGET_H
#define LAYERWIDGET_H

#include <QCheckBox>
#include <QComboBox>
#include <QFontDialog>
#include <QLabel>
#include <QMap>
#include <QPushButton>
#include <QVariantMap>
#include <QWidget>

#include "GraphicsModelingKernel/CanvasContext.h"
class LayerWidget : public QWidget
{
    Q_OBJECT

public:
    LayerWidget(PCanvasContext canvasCtx, const QMap<QString, bool> &layerItemsMap, QWidget *parent = nullptr);
    ~LayerWidget();
    QMap<QString, bool> getOldVisibleMap();
    QMap<QString, bool> getNewVisibleMap();

private:
    void initUI();

private:
    QList<QCheckBox *> m_CheckBoxslist;
    QMap<QString, bool> m_layerItemsMap;
    QMap<QString, bool> m_oldVisibleMap;
    PCanvasContext canvasCtx;
};

class LayerSettingWidget : public QWidget
{
    Q_OBJECT
public:
    LayerSettingWidget(PCanvasContext canvasCtx, const QStringList &names, QWidget *parent = nullptr);
    ~LayerSettingWidget();
    QMap<QString, QFont> getOldFontMap();
    QMap<QString, QFont> getNewFontMap();
    void setCurrentLayer(const QString &layername);

private:
    void initUI();
    void initOldSeting();
    QFont getLayerFont(const QString &layerstr);

private slots:
    void onBoxIndexChanged(const QString &layername);
    void onFontBtnClicked();

private:
    // 界面
    QLabel *m_LayerName;
    QComboBox *m_LayerBox;
    QPushButton *m_pFontBtn;
    // 数据
    // QMap<QString, QStringList> m_canChangeFontItemsMap; // 限定可以修改字体的图层
    QStringList m_canChangeFontNames;
    QMap<QString, QFont> m_layerOldFontMap; // key是图层显示名
    QMap<QString, QFont> m_layerNewFontMap; // key是图层显示名
    PCanvasContext canvasCtx;
};
#endif // LAYERWIDGET_H
