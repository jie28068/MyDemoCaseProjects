#include "NPSPropertyManager.h"
#include "ICanvasScene.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

NPSPropertyManager::NPSPropertyManager() { }

NPSPropertyManager::~NPSPropertyManager() { }

NPSPropertyManager &NPSPropertyManager::operator=(const NPSPropertyManager &other)
{
    if (this != &other) {

        this->properties = other.properties;
    }
    return *this;
}

QVariantMap &NPSPropertyManager::getProperties()
{
    return properties;
}

void NPSPropertyManager::setProperty(const QString &name, const QVariant &value)
{
    QVariant oldValue = getProperty(name, QVariant());
    if (oldValue != value) {
        QStringList parts = name.split('.', QString::SkipEmptyParts);
        setNestProperty(parts, properties, value);
        emit propertyChanged(name, oldValue, value);
    }
}

QVariant NPSPropertyManager::getProperty(const QString &name, QVariant &defaultValue) const
{
    QStringList keys = name.split('.', QString::SkipEmptyParts);
    QVariant ret = getNestProperty(keys, properties);
    if (ret.isNull()) {
        return defaultValue;
    }
    return ret;
}

void NPSPropertyManager::setDefaultProperty(const QString &name, const QVariant &value)
{
    QVariant propertyValue = getProperty(name, QVariant());
    if (propertyValue.isNull()) {
        setProperty(name, value);
    }
}

QString NPSPropertyManager::toJson()
{
    QJsonDocument jsonDoc(QJsonObject::fromVariantMap(properties));
    QString jsonString = jsonDoc.toJson(QJsonDocument::Indented);
    return jsonString;
}

QVariant NPSPropertyManager::getNestProperty(QStringList &keys, const QVariantMap &map) const
{
    if (keys.size() == 1) {
        return map.value(keys.at(0), QVariant());
    } else {
        QString key = keys.first();
        keys.pop_front();
        QVariantMap &subMap = map.value(key).toMap();
        return getNestProperty(keys, subMap);
    }
}

void NPSPropertyManager::setNestProperty(QStringList keys, QVariantMap &map, const QVariant &value)
{
    if (keys.size() == 1) {
        map[keys.at(0)] = value;
    } else {
        QString key = keys.first();
        keys.pop_front();
        QVariantMap subMap = map.value(key).toMap();
        setNestProperty(keys, subMap, value);
        map[key] = subMap;
    }
}

/*********************************CanvasProperty************************************/

CanvasProperty::CanvasProperty() { }

void CanvasProperty::setBaseLayerVisible(bool visible)
{
    setProperty(BASE_LAYER_VISIBLE_PROPERTY_KEY, visible);
}

bool CanvasProperty::isBaseLayerVisible()
{
    return getProperty(BASE_LAYER_VISIBLE_PROPERTY_KEY, QVariant(true)).toBool();
}

void CanvasProperty::setSourceNameVisble(bool visible)
{
    setProperty(SOURCENAME_VISIBLE_PROPERTY_KEY, visible);
}

bool CanvasProperty::isSourceNameVisible()
{
    return getProperty(SOURCENAME_VISIBLE_PROPERTY_KEY, QVariant(true)).toBool();
}

void CanvasProperty::setAnnotationVisible(bool visible)
{
    setProperty(ANNOTATION_VISIBLE_PROPERTY_KEY, visible);
}

bool CanvasProperty::isAnnotationVisible()
{
    return getProperty(ANNOTATION_VISIBLE_PROPERTY_KEY, QVariant(true)).toBool();
}

void CanvasProperty::setLegendVisible(bool visible)
{
    setProperty(LEGEND_LAYER_VISIBLE_PROPERTY_KEY, visible);
}

bool CanvasProperty::isLegendVisible()
{
    return getProperty(LEGEND_LAYER_VISIBLE_PROPERTY_KEY, QVariant(true)).toBool();
}

void CanvasProperty::setBusbarPhapseVisible(bool visible)
{
    setProperty(BUSBAR_PHAPSE_VISIBLE_PROPERTY_KEY, visible);
}

bool CanvasProperty::isBusbarPhapseVisible()
{
    return getProperty(BUSBAR_PHAPSE_VISIBLE_PROPERTY_KEY, QVariant(true)).toBool();
}

void CanvasProperty::setConnectWireVisible(bool visible)
{
    setProperty(CONNECTOR_WIRE_VISIBLE_PROPERTY_KEY, visible);
}

bool CanvasProperty::isConnectWireVisible()
{
    return getProperty(CONNECTOR_WIRE_VISIBLE_PROPERTY_KEY, QVariant(true)).toBool();
}

void CanvasProperty::setHighLightSourcceList(QStringList sourceIDList)
{
    setProperty(HIGHLIGHT_PROPERTY_SOURCELIST_KEY, sourceIDList);
}

void CanvasProperty::setHighLightColor(QColor color)
{
    setProperty(HIGHLIGHT_PROPERTY_COLOR_KEY, color);
}

QColor CanvasProperty::getHighLightColor()
{
    QVariant defaultHighLightColor = QVariant::fromValue<QColor>(QColor("#0079c2"));
    QVariant color = getProperty(HIGHLIGHT_PROPERTY_COLOR_KEY, defaultHighLightColor);
    if (color.canConvert<QColor>()) {
        return color.value<QColor>();
    }
    return QColor("#0079c2");
}

void CanvasProperty::setHighLightBrush(QBrush brush)
{
    setProperty(HIGHLIGHT_PROPERTY_BRUSH_KEY, brush);
}

QBrush CanvasProperty::getHighLightBrush()
{
    return getProperty(HIGHLIGHT_PROPERTY_BRUSH_KEY, QVariant(QBrush(Qt::NoBrush))).value<QColor>();
}

void CanvasProperty::setSourceNameFont(QFont font)
{
    setProperty(FONT_SOURCE_NAME_PROPERTY_KEY, font);
}

QFont CanvasProperty::getSourceNameFont()
{
    return getProperty(FONT_SOURCE_NAME_PROPERTY_KEY, QVariant(QFont("Courier New", 8))).value<QFont>();
}

void CanvasProperty::setGotoFromHighLightSourcceList(QStringList sourceIDList)
{
    setProperty(HIGHLIGHT_GOTOFROM_PROPERTY_SOURCELIST_KEY, sourceIDList);
}

void CanvasProperty::setCopyRightFont(QFont font)
{
    setProperty(FONT_COPYRIGHT_PROPERTY_KEY, font);
}

QFont CanvasProperty::getCopyRightFont()
{

    return getProperty(FONT_COPYRIGHT_PROPERTY_KEY, QVariant(QFont("长仿宋体", 9))).value<QFont>();
}

/******************************LayerProperty*********************************/

GraphicsLayerProperty::GraphicsLayerProperty() { }

void GraphicsLayerProperty::setVisible(bool visible)
{
    setProperty(LAYER_VISIBLE_PROPERTY_KEY, visible);
}

bool GraphicsLayerProperty::getVisible()
{
    return getProperty(LAYER_VISIBLE_PROPERTY_KEY, QVariant(true)).toBool();
}

void GraphicsLayerProperty::setHightLight(bool value)
{
    setProperty(LAYER_HIGHLIGHT_PROPERTY_KEY, value);
}

bool GraphicsLayerProperty::getHightLight()
{
    return getProperty(LAYER_HIGHLIGHT_PROPERTY_KEY, QVariant(false)).toBool();
}

void GraphicsLayerProperty::setFont(QFont font)
{
    setProperty(LAYER_FONT_PROPERTY_KEY, font);
}

QFont GraphicsLayerProperty::getFont()
{
    return getProperty(LAYER_FONT_PROPERTY_KEY, QVariant(QFont(""))).value<QFont>();
}

void GraphicsLayerProperty::setGotoFromHightLight(bool value)
{
    setProperty(LAYER_GOTOFROM_HIGHLIGHT_PROPERTY_KEY, value);
}

bool GraphicsLayerProperty::getGotoFromHightLight()
{
    return getProperty(LAYER_GOTOFROM_HIGHLIGHT_PROPERTY_KEY, QVariant(false)).toBool();
}

/******************************SourceProperty************************************/

SourceProperty::SourceProperty() { }

void SourceProperty::setBusbarType(QString busbarType)
{
    setProperty(BUSBAR_TYPE_PROPERTY_KEY, busbarType);
}

QString SourceProperty::getBusbarType()
{
    return getProperty(BUSBAR_TYPE_PROPERTY_KEY, QVariant("")).toString();
}

void SourceProperty::setCreateType(CreateType type)
{
    setProperty(SOURCE_CREATE_TYPE_PROPERTY_KEY, int(type));
}

SourceProperty::CreateType SourceProperty::getCreateType()
{
    return SourceProperty::CreateType(getProperty(SOURCE_CREATE_TYPE_PROPERTY_KEY, QVariant(0)).toInt());
}

void SourceProperty::setBackgroundColor(const QColor &color)
{
    setProperty(SOURCE_BACKGROUND_COLOR_PROPERTY_KEY, color);
}

QColor SourceProperty::getBackgroundColor()
{
    return getProperty(SOURCE_BACKGROUND_COLOR_PROPERTY_KEY, QVariant(QColor(Qt::white))).value<QColor>();
}

void SourceProperty::setEnableScale(bool enable)
{
    setProperty(SOURCE_ENABLE_SCALE_PROPERTY_KEY, enable);
}

bool SourceProperty::isEnableScale()
{
    return getProperty(SOURCE_ENABLE_SCALE_PROPERTY_KEY, QVariant(true)).toBool();
}

void SourceProperty::setShowOutSideBorder(bool show)
{

    setProperty(SOURCE_ENABLE_OUTSIDE_BORDER_PROPERTY_KEY, show);
}

bool SourceProperty::isShowOutSideBorder()
{
    return getProperty(SOURCE_ENABLE_OUTSIDE_BORDER_PROPERTY_KEY, QVariant(true)).toBool();
}

void SourceProperty::setShowInsideBorder(bool show)
{
    setProperty(SOURCE_ENABLE_INSIDE_BORDER_PROPERTY_KEY, show);
}

bool SourceProperty::isShowInsideBorder()
{
    return getProperty(SOURCE_ENABLE_INSIDE_BORDER_PROPERTY_KEY, QVariant(false)).toBool();
}

void SourceProperty::setDisplayData(QVariant data)
{
    setProperty(SOURCE_DISPLAYDATA_PROPERTY_KEY, data);
}

QVariant SourceProperty::getDisplayData()
{
    return getProperty(SOURCE_DISPLAYDATA_PROPERTY_KEY, QVariant(""));
}

void SourceProperty::setInputOutputIndex(int index)
{
    setProperty(SOURCE_INPUTOUTPUT_INDEX_PROPERTY_KEY, index);
}

int SourceProperty::getInputOutputIndex()
{
    return getProperty(SOURCE_INPUTOUTPUT_INDEX_PROPERTY_KEY, QVariant(-1)).toInt();
}

void SourceProperty::setInputOutputDisplayIndex(int index)
{
    setProperty(SOURCE_INPUTOUTPUT_DISPLAY_INDEX_PROPERTY_KEY, index);
    setDisplayData(QVariant(QString::number(index)));
}

int SourceProperty::getInputOutputDisplayIndex()
{
    return getProperty(SOURCE_INPUTOUTPUT_DISPLAY_INDEX_PROPERTY_KEY, QVariant(0)).toInt();
}

void SourceProperty::setBackgroundImage(const QByteArray &imageData, bool isUserSet)
{
    if (isUserSet) {
        if (imageData.isEmpty()) {
            setBackgroundImageIsUserSet(false);
            setProperty(SOURCE_IMAGE_BACKGROUND_PROPERTY_KEY, imageData);
            return;
        }
        setBackgroundImageIsUserSet(true);
        setProperty(SOURCE_IMAGE_BACKGROUND_PROPERTY_KEY, imageData);
    } else {
        if (!getBackgroundImageIsUserSet()) {
            setProperty(SOURCE_IMAGE_BACKGROUND_PROPERTY_KEY, imageData);
        }
    }
}

QByteArray SourceProperty::getBackgroundImage()
{
    return getProperty(SOURCE_IMAGE_BACKGROUND_PROPERTY_KEY, QVariant(QByteArray())).toByteArray();
}

void SourceProperty::setBackgroundImageIsUserSet(bool userSet)
{
    setProperty(SOURCE_IMAGE_BACKGROUNDISUSERSET_PROPERTY_KEY, userSet);
}

bool SourceProperty::getBackgroundImageIsUserSet()
{
    return getProperty(SOURCE_IMAGE_BACKGROUNDISUSERSET_PROPERTY_KEY, QVariant(false)).toBool();
}

void SourceProperty::setForegroundImage(const QByteArray &imageData)
{
    setProperty(SOURCE_IMAGE_FOREGROUND_PROPERTY_KEY, imageData);
}

QByteArray SourceProperty::getForegroundImage()
{
    return getProperty(SOURCE_IMAGE_FOREGROUND_PROPERTY_KEY, QVariant(QByteArray())).toByteArray();
}

void SourceProperty::setAnnotationImage(const QVariant &imageData)
{
    setProperty(SOURCE_IMAGE_ANNOTATION_PROPERTY_KEY, imageData);
}

QVariant SourceProperty::getAnnotationImage()
{
    return getProperty(SOURCE_IMAGE_ANNOTATION_PROPERTY_KEY, QVariant());
}

void SourceProperty::setStepDifference(QVariant stepDifference) 
{ 
    setProperty(SOURCE_STEP_DIFFERENCE_PROPERTY_KEY,stepDifference);
}

QVariant SourceProperty::getStepDifference()
{
    return getProperty(SOURCE_STEP_DIFFERENCE_PROPERTY_KEY, QVariant(""));
}

void SourceProperty::setPhaseDegress(QVariant phaseDegress) 
{ 
    setProperty(SOURCE_PHASE_DEGRESS_KEY,phaseDegress);
}

QVariant SourceProperty::getPhaseDegress()
{
    return getProperty(SOURCE_PHASE_DEGRESS_KEY, QVariant(""));
}
