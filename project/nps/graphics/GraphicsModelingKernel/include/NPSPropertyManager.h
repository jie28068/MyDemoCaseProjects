#pragma once

#include "Global.h"

#include <QObject>
#include <QVariantMap>

/********** CanvasProperty  **********/

#define BASE_LAYER_VISIBLE_PROPERTY_KEY "layerVisibility.baseLayer"
#define SOURCENAME_VISIBLE_PROPERTY_KEY "layerVisibility.sourceName"
#define ANNOTATION_VISIBLE_PROPERTY_KEY "layerVisibility.annotation"
#define LEGEND_LAYER_VISIBLE_PROPERTY_KEY "layerVisibility.legend"
#define BUSBAR_PHAPSE_VISIBLE_PROPERTY_KEY "layerVisibility.busbarPhapse"
#define CONNECTOR_WIRE_VISIBLE_PROPERTY_KEY "layerVisibility.connectWire"

#define HIGHLIGHT_PROPERTY_SOURCELIST_KEY "highLight.sourceList"
#define HIGHLIGHT_PROPERTY_COLOR_KEY "highLight.color"
#define HIGHLIGHT_PROPERTY_BRUSH_KEY "highLight.brush"
#define HIGHLIGHT_GOTOFROM_PROPERTY_SOURCELIST_KEY "GotoFromHighLight.sourceList" // goto 和from模块高亮

#define FONT_SOURCE_NAME_PROPERTY_KEY "font.sourceName"
#define FONT_COPYRIGHT_PROPERTY_KEY "font.copyright"

#define GRID_FLAG_PROPERTY_KEY "gridFlag"

/************  LayerProperty **************/

#define LAYER_VISIBLE_PROPERTY_KEY "visibility"
#define LAYER_HIGHLIGHT_PROPERTY_KEY "hightLight"
#define LAYER_GOTOFROM_HIGHLIGHT_PROPERTY_KEY "gotoFromHightLight" // goto 和from模块高亮
#define LAYER_FONT_PROPERTY_KEY "font"

/*************  SourceProperty  ***************/
#define BUSBAR_TYPE_PROPERTY_KEY "busbarType"
#define SOURCE_CREATE_TYPE_PROPERTY_KEY "source.createType"

#define SOURCE_BACKGROUND_COLOR_PROPERTY_KEY "source.backgroundColor"

#define SOURCE_ENABLE_SCALE_PROPERTY_KEY "source.enable.scale"
#define SOURCE_ENABLE_INSIDE_BORDER_PROPERTY_KEY "source.enable.insideBorder"
#define SOURCE_ENABLE_OUTSIDE_BORDER_PROPERTY_KEY "source.enable.outsideBorder"

#define SOURCE_DISPLAYDATA_PROPERTY_KEY "source.displayData"

#define SOURCE_INPUTOUTPUT_INDEX_PROPERTY_KEY "source.inputoutput.index"
#define SOURCE_INPUTOUTPUT_DISPLAY_INDEX_PROPERTY_KEY "source.inputoutput.displayIndex"

#define SOURCE_IMAGE_BACKGROUND_PROPERTY_KEY "source.image.background"
#define SOURCE_IMAGE_BACKGROUNDISUSERSET_PROPERTY_KEY "source.image.backgroundIsUserSet"
#define SOURCE_IMAGE_FOREGROUND_PROPERTY_KEY "source.image.foreground"

#define SOURCE_IMAGE_ANNOTATION_PROPERTY_KEY "source.image.annotation"
#define SOURCE_STEP_DIFFERENCE_PROPERTY_KEY "source.step.difference"
#define SOURCE_PHASE_DEGRESS_KEY "source.phase.degress"

class CanvasContext;
class ICanvasScene;

class GRAPHICSMODELINGKERNEL_EXPORT NPSPropertyManager : public QObject
{
    Q_OBJECT
public:
    NPSPropertyManager();
    ~NPSPropertyManager();

    NPSPropertyManager &operator=(const NPSPropertyManager &other);

    QVariantMap &getProperties();

    /// @brief 设置属性
    /// @param name 属性名称，名称之间可以用"." 分隔，表示多级属性
    /// @param value 属性值
    void setProperty(const QString &name, const QVariant &value);

    /// @brief 获取属性值
    /// @param name 属性名称，名称之间可以用"." 分隔，表示多级属性
    /// @param defaultValue 如果不存在属性，返回的默认值
    /// @return
    QVariant getProperty(const QString &name, QVariant &defaultValue = QVariant()) const;

    /// @brief 设置默认属性，只有属性不存在时才设置
    void setDefaultProperty(const QString &name, const QVariant &value);

    QString toJson();

signals:
    void propertyChanged(const QString &name, const QVariant &oldValue, const QVariant &newValue);

public slots:

private:
    void setNestProperty(QStringList keys, QVariantMap &map, const QVariant &value);

    QVariant getNestProperty(QStringList &keys, const QVariantMap &map) const;

private:
    QVariantMap properties;
};

/// @brief 画板属性
class GRAPHICSMODELINGKERNEL_EXPORT CanvasProperty : public NPSPropertyManager
{
    Q_OBJECT
public:
    CanvasProperty();

    /// @brief 设置基础图层是否可见
    /// @param visible
    void setBaseLayerVisible(bool visible);

    bool isBaseLayerVisible();

    /// @brief 设置资源名称是否可见
    /// @param visible
    void setSourceNameVisble(bool visible);

    bool isSourceNameVisible();

    /// @brief 设置注解图层是否可见
    /// @param visible
    void setAnnotationVisible(bool visible);

    bool isAnnotationVisible();

    void setLegendVisible(bool visible);

    bool isLegendVisible();

    void setBusbarPhapseVisible(bool visible);

    bool isBusbarPhapseVisible();

    void setConnectWireVisible(bool visible);

    bool isConnectWireVisible();

    /// @brief 设置高亮显示的资源ID
    /// @param sourceIDList 资源ID列表
    void setHighLightSourcceList(QStringList sourceIDList);

    /// @brief 设置高亮显示颜色
    /// @param color
    void setHighLightColor(QColor color);

    QColor getHighLightColor();

    void setHighLightBrush(QBrush brush);

    QBrush getHighLightBrush();

    void setSourceNameFont(QFont font);

    QFont getSourceNameFont();

    void setGotoFromHighLightSourcceList(QStringList sourceIDList);

    void setCopyRightFont(QFont font);

    QFont getCopyRightFont();
};

class GRAPHICSMODELINGKERNEL_EXPORT GraphicsLayerProperty : public NPSPropertyManager
{
    Q_OBJECT
public:
    GraphicsLayerProperty();

    void setVisible(bool visible);

    bool getVisible();

    void setHightLight(bool value);

    bool getHightLight();

    void setFont(QFont font);

    QFont getFont();

    void setGotoFromHightLight(bool value);

    bool getGotoFromHightLight();
};

class GRAPHICSMODELINGKERNEL_EXPORT SourceProperty : public NPSPropertyManager
{
    Q_OBJECT
public:
    enum CreateType {
        kNormal = 0, // 正常途径，画板拖拽添加
        kCopy,       // 拷贝
        kCut,        // 剪切
        kRevert      // 删除之后撤销重做恢复
    };

    SourceProperty();

    void setBusbarType(QString busbarType);

    QString getBusbarType();

    /// @brief 设置资源的创建类型
    void setCreateType(CreateType type);

    CreateType getCreateType();

    /// @brief 设置资源图形的背景颜色
    /// @param color
    void setBackgroundColor(const QColor &color);

    QColor getBackgroundColor();

    /// @brief 设置资源模块是否允许缩放
    void setEnableScale(bool enable);

    bool isEnableScale();

    void setShowOutSideBorder(bool show);

    bool isShowOutSideBorder();

    /// @brief 设置是否显示内边框
    /// @param show
    void setShowInsideBorder(bool show);

    bool isShowInsideBorder();

    void setDisplayData(QVariant data);

    QVariant getDisplayData();

    void setInputOutputIndex(int index);

    int getInputOutputIndex();

    void setInputOutputDisplayIndex(int index);

    int getInputOutputDisplayIndex();

    // 第二个参数为:该背景图片是否为用户设置: 设置自定义背景图为true,设置缩略图作为背景为false
    void setBackgroundImage(const QByteArray &imageData, bool isUserSet);

    QByteArray getBackgroundImage();

    void setForegroundImage(const QByteArray &imageData);

    QByteArray getForegroundImage();

    void setAnnotationImage(const QVariant &imageData);

    QVariant getAnnotationImage();
    void setStepDifference(QVariant stepDifference);

    QVariant getStepDifference();

    void setPhaseDegress(QVariant phaseDegress);

    QVariant getPhaseDegress();

private:
    void setBackgroundImageIsUserSet(bool userSet);

    bool getBackgroundImageIsUserSet();
};