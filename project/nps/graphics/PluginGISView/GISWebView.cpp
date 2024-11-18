#include "GISWebView.h"
#include "GISWebPage.h"

GISWebView::GISWebView(QWidget *parent) : QWebEngineView(parent) { }

void GISWebView::setGISPage(GISWebPage *page)
{
    QWebEngineView::setPage(page);
}
