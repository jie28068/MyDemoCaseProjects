#include "CoverWidget.h"

#include "CommonModelAssistant.h"
#include "KLModelDefinitionCore/BlockDefinition.h"
#include <QFileDialog>

using namespace Kcc::BlockDefinition;

static const int PICLABEL_WIDTH = 160;
static const int PICLABEL_HEIGHT = 80;
static const int PICLABEL_INNERPADING = 6;
static const QString BoardBGColor = QObject::tr("Backgroud Color"); // 背景色
static const QString DEFAULT_BGCOLOR = QString("#FFFFFF");          // 默认颜色

CoverWidget::CoverWidget(QSharedPointer<Kcc::BlockDefinition::DrawingBoardClass> drawboard, QWidget *parent)
    : CWidget(parent),
      m_drawboard(drawboard),
      m_tableWidget(nullptr),
      m_bgLabel(nullptr),
      m_previewLabel(nullptr),
      m_onlyPicLabel(nullptr),
      m_PicAndBgLabel(nullptr),
      m_isChangeData(false)
{
    initUI();
}

CoverWidget::~CoverWidget() { }

bool CoverWidget::saveData()
{
    if (m_drawboard == nullptr || m_tableWidget == nullptr || m_onlyPicLabel == nullptr) {
        return false;
    }
    m_isChangeData = false;
    QString oldcolor = m_drawboard->getResource()[CANVAS_BACKGROUND_COLOR].toString();
    CustomModelItem coloritem = m_tableWidget->getItemData(BoardBGColor);
    if (oldcolor != coloritem.value.toString()) {
        m_drawboard->setResource(CANVAS_BACKGROUND_COLOR, coloritem.value.toString());
        if (oldcolor.isEmpty() && DEFAULT_BGCOLOR == coloritem.value.toString()) {
            m_isChangeData = false;
        } else {
            m_isChangeData = true;
        }
    }

    if (m_onlyPicLabel->isPixChange()) {
        m_isChangeData = true;
        m_drawboard->setResource(CANVAS_BACKGROUND_IMAGE, m_onlyPicLabel->getPixmap());
    }
    return m_isChangeData;
}

void CoverWidget::setCWidgetReadOnly(bool bReadOnly)
{
    if (m_tableWidget != nullptr) {
        m_tableWidget->setCWidgetReadOnly(bReadOnly);
    }
    if (m_bgLabel != nullptr) {
        m_bgLabel->setDisabled(bReadOnly);
    }
    if (m_previewLabel != nullptr) {
        m_previewLabel->setDisabled(bReadOnly);
    }
    if (m_onlyPicLabel != nullptr) {
        m_onlyPicLabel->setDisabled(bReadOnly);
    }
    if (m_PicAndBgLabel != nullptr) {
        m_PicAndBgLabel->setDisabled(bReadOnly);
    }
}

bool CoverWidget::isDataChanged()
{
    return m_isChangeData;
}

void CoverWidget::onPixmapChange(const QPixmap &pix)
{
    if (m_PicAndBgLabel != nullptr) {
        m_PicAndBgLabel->updatePixmap(pix);
    }
}

void CoverWidget::onModelDataChanged(const CustomModelItem &olditem, const CustomModelItem &newitem)
{
    if (m_PicAndBgLabel != nullptr && newitem.keywords == BoardBGColor) {
        m_PicAndBgLabel->updateBgColor(newitem.value.toString());
    }
}

void CoverWidget::initUI()
{
    if (m_drawboard == nullptr) {
        return;
    }
    m_colorStr = m_drawboard->getResource()[CANVAS_BACKGROUND_COLOR].toString();
    QVariant pixv = m_drawboard->getResource()[CANVAS_BACKGROUND_IMAGE];
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setMargin(0);
    m_tableWidget = new CustomTableWidget(this);
    m_tableWidget->setListData(QList<CustomModelItem>() << CustomModelItem(
                                       BoardBGColor, BoardBGColor, m_colorStr.isEmpty() ? DEFAULT_BGCOLOR : m_colorStr,
                                       RoleDataDefinition::ControlTypeColor, true, ""),
                               QStringList() << CMA::ATTRIBUTE_NAME << CMA::ATTRIBUTE_VALUE);
    m_bgLabel = new QLabel(this);
    m_bgLabel->setText(tr("Background image")); // 背景图
    m_previewLabel = new QLabel(this);
    m_previewLabel->setText(tr("Preview")); // 预览
    m_onlyPicLabel = new PicLabel(PicLabel::LabelType_OnlyPic, pixv.value<QPixmap>());
    m_PicAndBgLabel = new PicLabel(PicLabel::LabelType_PicAndBG, pixv.value<QPixmap>(), "",
                                   m_colorStr.isEmpty() ? DEFAULT_BGCOLOR : m_colorStr);
    QSpacerItem *vSpacer = new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Expanding);
    QGridLayout *pixLayout = new QGridLayout(this);
    pixLayout->addWidget(m_bgLabel, 0, 0, 1, 1);
    pixLayout->addWidget(m_onlyPicLabel, 1, 0, 1, 1);
    pixLayout->addWidget(m_previewLabel, 0, 1, 1, 1);
    pixLayout->addWidget(m_PicAndBgLabel, 1, 1, 1, 1);
    mainLayout->addWidget(m_tableWidget);
    mainLayout->addLayout(pixLayout);
    mainLayout->addSpacerItem(vSpacer);
    setLayout(mainLayout);
    connect(m_tableWidget, SIGNAL(tableDataItemChanged(const CustomModelItem &, const CustomModelItem &)), this,
            SLOT(onModelDataChanged(const CustomModelItem &, const CustomModelItem &)));
    connect(m_onlyPicLabel, SIGNAL(pixmapChange(const QPixmap &)), this, SLOT(onPixmapChange(const QPixmap &)));
}

PicLabel::PicLabel(LabelType type, const QPixmap &pix, const QString &tooltips, const QString &bgcolor, QWidget *parent)
    : QLabel(parent), m_oldPixMap(pix), m_newPixMap(pix), m_menu(nullptr), m_bgColor(bgcolor), m_type(type)
{
    setFixedSize(PICLABEL_WIDTH, PICLABEL_HEIGHT);
    setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    // setFrameStyle(QFrame::Panel | QFrame::Plain);
    setToolTip(tooltips);
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    intUI(pix);
}

PicLabel::~PicLabel() { }

void PicLabel::updateBgColor(const QString &color)
{
    m_bgColor = color;
    update();
}

void PicLabel::updatePixmap(const QPixmap &pix)
{
    m_newPixMap = pix;
    update();
}

bool PicLabel::isPixChange()
{
    if (m_oldPixMap != m_newPixMap) {
        return true;
    }
    return false;
}

QPixmap PicLabel::getPixmap(bool newpix)
{
    if (newpix) {
        return m_newPixMap;
    }
    return m_oldPixMap;
}

void PicLabel::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QPen pen;
    pen.setWidth(1);
    pen.setColor(Qt::black);
    painter.setPen(pen);
    const QPixmap *pixmap = this->pixmap();
    QRect rec = QRect(0, 0, this->width() - 1, this->height() - 1);
    if (PicLabel::LabelType_PicAndBG == m_type) {
        painter.setBrush(QColor(m_bgColor));
        painter.drawRect(rec);
    }
    painter.drawRect(rec);
    QRect recin = QRect(PICLABEL_INNERPADING, PICLABEL_INNERPADING, this->width() - 2 * PICLABEL_INNERPADING - 1,
                        this->height() - 2 * PICLABEL_INNERPADING - 1);
    painter.drawRect(recin);
    QPixmap drawpix = m_newPixMap.scaled(
            QSize(this->width() - 2 * PICLABEL_INNERPADING - 2, this->height() - 2 * PICLABEL_INNERPADING - 2),
            Qt::KeepAspectRatio);
    QSize drawpixSize = drawpix.size();
    QPoint pos = QPoint(7, 7);
    if (drawpixSize.height() == this->height() - 2 * PICLABEL_INNERPADING - 2) {
        pos.setX(7 + (this->width() - 2 * PICLABEL_INNERPADING - drawpixSize.width()) / 2);
        pos.setY(7);
    } else {
        pos.setX(7);
        pos.setY(7 + (this->height() - 2 * PICLABEL_INNERPADING - drawpixSize.height()) / 2);
    }
    painter.drawPixmap(pos, drawpix);
}

void PicLabel::onSelectPic()
{
    QString pixpath = QFileDialog::getOpenFileName(this, tr("Select Image"), "", "图片文件(*png; *bmp)");
    if (pixpath.isEmpty()) {
        return;
    }
    m_newPixMap = QPixmap(pixpath);
    update();
    emit pixmapChange(m_newPixMap);
}

void PicLabel::onRemovePic()
{
    m_newPixMap = QPixmap();
    update();
    emit pixmapChange(m_newPixMap);
}

void PicLabel::onMenuRequest(const QPoint &pos)
{
    if (PicLabel::LabelType_OnlyPic != m_type) {
        return;
    }
    if (m_menu != nullptr) {
        m_menu->exec(cursor().pos());
    }
}

void PicLabel::intUI(const QPixmap &pix)
{
    m_menu = new QMenu(this);
    QAction *selectPic = new QAction(tr("Select Image"), this); // 选择图片
    QAction *removePic = new QAction(tr("Remove Image"), this); // 移除图片
    m_menu->addAction(selectPic);
    m_menu->addAction(removePic);
    connect(selectPic, SIGNAL(triggered()), this, SLOT(onSelectPic()));
    connect(removePic, SIGNAL(triggered()), this, SLOT(onRemovePic()));
    QObject::connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this,
                     SLOT(onMenuRequest(const QPoint &)));
    update();
}