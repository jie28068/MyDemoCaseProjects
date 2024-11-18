#include "GISWorkAreaContentWidget.h"
#include "GISWebPage.h"
#include "GISWebView.h"
#include "JSChannel.h"

#include <QCoreApplication>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWebChannel>
#include <QWebEngineProfile>
#include <QWebEngineSettings>

REG_WORKAREA(GISWorkAreaContentWidget)

GISWorkAreaContentWidget::GISWorkAreaContentWidget(QWidget *parent, QString title) : BasicWorkareaContentWidget(title)
{
    initUI();
}

GISWorkAreaContentWidget::~GISWorkAreaContentWidget() { }

void GISWorkAreaContentWidget::initUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    GISWebView *webView = new GISWebView(nullptr);
    GISWebPage *webPage = new GISWebPage(QWebEngineProfile::defaultProfile(), webView);

    JSChannel *jsChannel = new JSChannel(this);
    QWebChannel *webChannel = new QWebChannel(this);
    webChannel->registerObject("JSChannel", jsChannel);
    webPage->setWebChannel(webChannel);

    webView->setPage(webPage);

#ifdef _DEBUG
    QPushButton *btn1 = new QPushButton("send to js");
    connect(btn1, &QPushButton::clicked, this, [webView, webPage, jsChannel](bool) {
        // webPage->runJavaScript(QString("processMessageFromCPP('111111111')"));
        jsChannel->sendMessageToJS("111231312");
    });
    mainLayout->addWidget(btn1);

#endif
    mainLayout->addWidget(webView);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(mainLayout);

    QString appPath = QCoreApplication::applicationDirPath();
    QString htmlPath = appPath + "/GISWeb/index.html";
    QString urlPath = "file:///" + htmlPath;
    webView->load(QUrl(urlPath));
}
