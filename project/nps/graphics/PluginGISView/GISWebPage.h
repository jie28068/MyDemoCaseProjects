#pragma once

#include <QWebEnginePage>
#include <QWebEngineRegisterProtocolHandlerRequest>

class GISWebPage : public QWebEnginePage
{
    Q_OBJECT

public:
    GISWebPage(QWebEngineProfile *profile, QObject *parent = NULL);
};