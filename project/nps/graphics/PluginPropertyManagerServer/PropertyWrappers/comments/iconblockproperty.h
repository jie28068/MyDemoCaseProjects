#ifndef ICONBLOCKPROPERTY_H
#define ICONBLOCKPROPERTY_H

#include "CommonWrapper.h"
#include "ElideLineText.h"
#include "KLineEdit.h"
#include "commenteditwdiget.h"

#define TITLE_ICON QObject::tr("Image Properties") // 图片属性
#define ICON_EDIT QObject::tr("Set Picture")       // 图片设定

class IconEditWidget;
class SourceProxy;

class IconBlockProperty : public CommonWrapper
{
    Q_OBJECT

public:
    IconBlockProperty(QSharedPointer<SourceProxy> sourceProxy, QSharedPointer<CanvasContext> cancasContext);
    ~IconBlockProperty();
    virtual void init() override;
    IconEditWidget *getIconEditWidget();

    // CommonWrapper
    QString getTitle() override;
    void onDialogExecuteResult(QDialog::DialogCode code) override;
    bool checkValue(QString &errorinfo = QString()) override;

private:
    IconEditWidget *m_pIconEditWidget;
    QLineEdit *m_pNameLineEdit;
    QSharedPointer<SourceProxy> m_sourceProxy;
    QSharedPointer<CanvasContext> m_canvasContext;
};

class IconEditWidget : public CommentEditWdiget
{
    Q_OBJECT
public:
    IconEditWidget(QSharedPointer<SourceProxy> pSource, QWidget *parent = nullptr);

    QString getIconName();

    QString getIconPath();
    QString getIconData();
    QVariant getIconDatas();

    bool getIsChanged() { return isChanged; };

private:
    void initUI();

private slots:
    void onLoadIcon();

signals:
    void updateIconPath();

private:
    KLineEdit *m_pNameEdit;
    ElideLineText *m_pIconPathElide;
    QString m_strIconPath;
    bool isChanged;
    // QLineEdit *m_pWidthEdit;
    // QLineEdit *m_pHeightEdit;
};

#endif // ICONBLOCKPROPERTY_H
