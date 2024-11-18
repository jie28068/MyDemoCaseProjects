#include "LayerWidget.h"
#include <QApplication>
#include <QButtonGroup>
#include <QGridLayout>
#include <QListView>
#include <QSpacerItem>
#include <QVector>

#include "GraphicsModelingKernel/CanvasContext.h"
#include "GraphicsModelingKernel/GraphicsKernelDefinition.h"
#include "GraphicsModelingKernel/GraphicsLayer.h"

// 属性页服务
#include "PropertyManagerServer/IPropertyManagerServer.h"
using namespace Kcc::PropertyManager;
#include "CoreLib/ServerManager.h"

LayerWidget::LayerWidget(PCanvasContext canvasCtx, const QMap<QString, bool> &layerItemsMap, QWidget *parent)
    : QWidget(parent), canvasCtx(canvasCtx), m_layerItemsMap(layerItemsMap)
{
    initUI();
}

LayerWidget::~LayerWidget() { }

QMap<QString, bool> LayerWidget::getOldVisibleMap()
{
    return m_oldVisibleMap;
}

QMap<QString, bool> LayerWidget::getNewVisibleMap()
{
    QMap<QString, bool> newVisibleMap;
    foreach (QCheckBox *checkbox, m_CheckBoxslist) {
        if (checkbox != nullptr) {
            newVisibleMap.insert(checkbox->text(), checkbox->isChecked());
        }
    }
    return newVisibleMap;
}

void LayerWidget::initUI()
{
    QWidget *pLayersWidget = new QWidget(this);
    pLayersWidget->setFixedSize(505, 395);

    QGridLayout *pGridlayout = new QGridLayout(this);
    QLabel *pname = new QLabel(this);
    pname->setMinimumSize(QSize(120, 0));
    pname->setMaximumSize(QSize(120, 16777215));
    pname->setText(tr("is the layer displayed")); // 图层是否显示
    pGridlayout->addWidget(pname, 0, 0, 1, 1);
    int row = 1;
    int column = 0;
    foreach (QString displayname, m_layerItemsMap.keys()) {
        QCheckBox *pLayerCheckBox = new QCheckBox(this);
        pLayerCheckBox->setFixedSize(170, 35);
        pLayerCheckBox->setText(displayname);
        bool bvisible = m_layerItemsMap[displayname];
        pLayerCheckBox->setChecked(bvisible);
        m_oldVisibleMap[displayname] = bvisible;
        m_CheckBoxslist.append(pLayerCheckBox);

        pGridlayout->addWidget(pLayerCheckBox, ++row, column, 1, 1);

        if (10 == row) {
            row = 0;
            ++column;
        }
    }

    QSpacerItem *pVSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    pGridlayout->addItem(pVSpacer, 11, 0, 1, 1);
    QSpacerItem *pHSpacer = new QSpacerItem(20, 40, QSizePolicy::Expanding, QSizePolicy::Minimum);
    pGridlayout->addItem(pHSpacer, 11, column++, 1, 1);
    pLayersWidget->setLayout(pGridlayout);
}

LayerSettingWidget::LayerSettingWidget(PCanvasContext canvasCtx, const QStringList &names,
                                       QWidget *parent /*= nullptr*/)
    : QWidget(parent),
      canvasCtx(canvasCtx),
      m_canChangeFontNames(names),
      m_LayerName(nullptr),
      m_LayerBox(nullptr),
      m_pFontBtn(nullptr)
{
    // 暂时限定只有source名可以修改文本字体
    // m_canChangeFontItemsMap.clear();
    // foreach (QString displayname, layerItemMap.keys()) {
    //     if (DisplayLayer_SourceNameGraphicsLayer == displayname) {
    //         m_canChangeFontItemsMap.insert(displayname, layerItemMap[displayname]);
    //         break;
    //     }
    // }
    initOldSeting();
    initUI();
}

LayerSettingWidget::~LayerSettingWidget() { }

QMap<QString, QFont> LayerSettingWidget::getOldFontMap()
{
    return m_layerOldFontMap;
}

QMap<QString, QFont> LayerSettingWidget::getNewFontMap()
{
    return m_layerNewFontMap;
}

void LayerSettingWidget::setCurrentLayer(const QString &layername)
{
    if (m_LayerBox != nullptr) {
        int index = m_LayerBox->findText(layername);
        m_LayerBox->setCurrentIndex(index);
    }
}

void LayerSettingWidget::initUI()
{
    QGridLayout *gridlayout = new QGridLayout(this);
    gridlayout->setMargin(0);

    m_LayerName = new QLabel(this);
    m_LayerName->setMinimumSize(QSize(120, 0));
    m_LayerName->setMaximumSize(QSize(120, 16777215));
    m_LayerName->setText(tr("Layer Name")); // 图层名
    gridlayout->addWidget(m_LayerName, 0, 0, 1, 1);

    m_LayerBox = new QComboBox(this);
    m_LayerBox->setView(new QListView());
    m_LayerBox->addItems(m_canChangeFontNames);
    m_LayerBox->setFixedWidth(160);
    m_LayerBox->setCurrentIndex(0);
    gridlayout->addWidget(m_LayerBox, 0, 1, 1, 2);

    m_pFontBtn = new QPushButton(this);
    m_pFontBtn->setFixedWidth(120);
    m_pFontBtn->setText(tr("Font Settings")); // 字体设定
    if (m_canChangeFontNames.size() <= 0) {
        m_pFontBtn->setEnabled(false);
    }
    gridlayout->addWidget(m_pFontBtn, 1, 0, 1, 1);

    QSpacerItem *pverticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    gridlayout->addItem(pverticalSpacer, 2, 0, 1, 1);

    connect(m_pFontBtn, SIGNAL(released()), this, SLOT(onFontBtnClicked()));
    connect(m_LayerBox, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(onBoxIndexChanged(const QString &)));
}

void LayerSettingWidget::initOldSeting()
{
    m_layerOldFontMap.clear();
    m_layerNewFontMap.clear();
    foreach (QString layername, m_canChangeFontNames) {
        if (layername == DisplayLayer_SourceNameGraphicsLayer) {
            m_layerOldFontMap[layername] = getLayerFont(layername);
            m_layerNewFontMap[layername] = getLayerFont(layername);
            ;
        }
    }
}

QFont LayerSettingWidget::getLayerFont(const QString &layerstr)
{
    if (layerstr.isEmpty())
        return GKD::DEFAULT_FONT;
    if (layerstr == DisplayLayer_SourceNameGraphicsLayer) {
        return canvasCtx->getCanvasProperty().getSourceNameFont();
    }
    return GKD::DEFAULT_FONT;
}

void LayerSettingWidget::onBoxIndexChanged(const QString &layername)
{
    // if (m_pGlobalFontWidget != nullptr){
    //	m_pGlobalFontWidget->setFont(m_layerNewFontMap[layername]);
    // }
}

void LayerSettingWidget::onFontBtnClicked()
{
    PIPropertyManagerServer pPropertyServer = RequestServer<IPropertyManagerServer>();
    if (m_LayerBox != nullptr && pPropertyServer != nullptr) {
        QString CutrrentLayerstr = m_LayerBox->currentText();
        QFont defaultfont = GKD::DEFAULT_FONT;
        int pointsize = defaultfont.pointSize();
        QString fastr = defaultfont.family();
        defaultfont = m_layerNewFontMap[CutrrentLayerstr];
        int pointsize2 = defaultfont.pointSize();
        QString fastr2 = defaultfont.family();
        QFont setfont = pPropertyServer->ShowFontSetDialog(m_layerNewFontMap[CutrrentLayerstr]);
        int pointsize3 = setfont.pointSize();
        QString fastr3 = setfont.family();
        m_layerNewFontMap[CutrrentLayerstr] = setfont;
    }
}
