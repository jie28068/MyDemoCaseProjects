#pragma once

#include <QList>
#include <QMap>
#include <QObject>
#include <QStringList>

class GraphicsConfigPrivate;
class GraphicsConfig : public QObject
{
    Q_OBJECT
public:
    static GraphicsConfig &getInstance();

    QStringList getTransparentBackgroundModules();

private:
    GraphicsConfig();
    ~GraphicsConfig();

    void parseConfig();

private:
    QScopedPointer<GraphicsConfigPrivate> dataPtr;
};