#pragma once

#include <QWebEngineView>

class GISWebPage;
class GISWebView : public QWebEngineView
{
    Q_OBJECT

public:
    GISWebView(QWidget *parent = nullptr);

    void setGISPage(GISWebPage *page);
};
