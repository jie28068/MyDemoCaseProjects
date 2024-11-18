#include "iconblockproperty.h"
#include "GlobalAssistant.h"
#include "GraphicsModelingKernel/graphicsmodelingkernel.h"
#include <QFileDialog>
#include <QLabel>
#include <QPushButton>
#include <QStringListIterator>
// 日志
#include "CoreLib/ServerManager.h"
USE_LOGOUT_("IconBlockProperty")

IconBlockProperty::IconBlockProperty(QSharedPointer<SourceProxy> sourceProxy,
                                     QSharedPointer<CanvasContext> CanvasContext)
    : CommonWrapper(), m_sourceProxy(sourceProxy), m_canvasContext(CanvasContext)
{
}

IconBlockProperty::~IconBlockProperty() { }

void IconBlockProperty::init()
{
    CommonWrapper::init();
    m_pIconEditWidget = new IconEditWidget(m_sourceProxy);
    addWidget(m_pIconEditWidget, ICON_EDIT);
}

IconEditWidget *IconBlockProperty::getIconEditWidget()
{
    return m_pIconEditWidget;
}

QString IconBlockProperty::getTitle()
{
    return QString(TITLE_ICON + " " + m_pIconEditWidget->getIconPath());
}

void IconBlockProperty::onDialogExecuteResult(QDialog::DialogCode code)
{
    if (code == QDialog::Accepted) {
        if (m_pIconEditWidget == nullptr) {
            return;
        }
        if (m_pIconEditWidget->getIconPath().isEmpty()) {
            if (!m_sourceProxy || m_sourceProxy->getAnnotationMap()[Annotation::iconPath].toString().isEmpty()) {
                // 加载图片路径为空，加载图片失败！
                LOGOUT(tr("Loading image path is empty, loading image failed!"), LOG_ERROR);
                return;
            }
        }

        if (m_pIconEditWidget->getIconName().contains(QRegExp(NPS::REG_SPACE))) {
            // 图片名称不能为纯空格字符！
            LOGOUT(tr("The image name cannot be a pure space character!！"), LOG_ERROR);
            return;
        }

        QStringList iconType = m_pIconEditWidget->getIconPath().split('.', QString::SkipEmptyParts);
        if (!iconType.last().contains(QRegExp("(png|bmp)", Qt::CaseInsensitive))) {
            // 图片格式只能为“png”或“bmp”！
            LOGOUT(tr("The image format can only be 'png' or 'bmp'!"), LOG_ERROR);
            return;
        }

        bool bvchanged = false;
        QString blockOldName = m_sourceProxy ? m_sourceProxy->name() : QString();
        if (blockOldName != m_pIconEditWidget->getIconName()) {
            bvchanged = true;
            if (m_sourceProxy) {
                m_sourceProxy->setName(m_pIconEditWidget->getIconName());
                if (!blockOldName.isEmpty()) {
                    LOGOUT(tr("The name of image annotation model [%1],from[%2] modify to [%3]")
                                   .arg(blockOldName)
                                   .arg(blockOldName)
                                   .arg(m_sourceProxy->name()),
                           LOG_NORMAL); // 图片注释模块[%1]的名称，从[%2]修改为[%3]
                }
            }
        }

        if ((!m_sourceProxy
             || m_sourceProxy->getAnnotationMap()[Annotation::iconPath].toString() != m_pIconEditWidget->getIconPath())
            && !m_pIconEditWidget->getIconPath().isEmpty()) {
            bvchanged = true;
            if (m_sourceProxy) {
                m_sourceProxy->setAnnotationProperty(Annotation::iconPath, m_pIconEditWidget->getIconPath());
                m_sourceProxy->setAnnotationProperty(Annotation::iconData, m_pIconEditWidget->getIconData());
                m_sourceProxy->getSourceProperty().setAnnotationImage(m_pIconEditWidget->getIconDatas());
            }
        }

        if (bvchanged) {
            m_bPropertyIsChanged = true;
        }
    }
}

bool IconBlockProperty::checkValue(QString &errorinfo /*= QString( ) */)
{
    errorinfo = "";
    if (!m_pIconEditWidget->getIsChanged()) {
        return true;
    }
    if (m_pIconEditWidget == nullptr) {
        return true;
    }

    if (m_canvasContext) {
        QMap<QString, QSharedPointer<SourceProxy>> allsourcemap = m_canvasContext->getAllSource();
        QMapIterator<QString, QSharedPointer<SourceProxy>> iter(allsourcemap);
        foreach (QSharedPointer<SourceProxy> psource, allsourcemap.values()) {
            if (psource != nullptr && psource->name() == m_pIconEditWidget->getIconName()) {
                errorinfo = NPS::ERRORINFO_REPEAT;
                return false;
            }
        }
    }

    if (m_pIconEditWidget->getIconName().isEmpty()) {
        errorinfo = tr("Module name cannot be empty! Please re-enter");
        return false;
    }
    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////
IconEditWidget::IconEditWidget(QSharedPointer<SourceProxy> pSource, QWidget *parent /*= nullptr*/)
    : CommentEditWdiget(pSource, parent), isChanged(false)
{
    initUI();
}

QString IconEditWidget::getIconName()
{
    return m_pNameEdit->text();
}

QString IconEditWidget::getIconPath()
{
    return m_strIconPath;
}

QString IconEditWidget::getIconData()
{
    QString imagestr = "";
    if (m_strIconPath.isEmpty()) {
        return "";
    }
    // 加载图片
    QImage img;
    img.load(m_strIconPath);
    QPixmap pixImg = QPixmap::fromImage(img.scaled(img.size(), Qt::KeepAspectRatio));

    if (!pixImg.isNull()) {
        // 保存图片信息
        QByteArray baData;
        QDataStream ds(&baData, QIODevice::ReadWrite);
        ds << pixImg;
        imagestr = QString::fromLatin1(baData.toBase64());
    }
    return imagestr;
}

QVariant IconEditWidget::getIconDatas()
{
    if (m_strIconPath.isEmpty()) {
        return "";
    }
    QImage img;
    img.load(m_strIconPath);
    QPixmap pixImg = QPixmap::fromImage(img.scaled(img.size(), Qt::KeepAspectRatio));
    QVariant variant = QVariant::fromValue(pixImg);
    return variant;
}

void IconEditWidget::initUI()
{
    QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);

    QGridLayout *pGridLayout = new QGridLayout;
    pGridLayout->setVerticalSpacing(15);
    pGridLayout->setHorizontalSpacing(10);

    QLabel *pNameLabel = new QLabel(this);
    pNameLabel->setText(tr("Name"));
    pGridLayout->addWidget(pNameLabel, 0, 0, 1, 1);

    m_pNameEdit = new KLineEdit(this);
    sizePolicy.setHeightForWidth(m_pNameEdit->sizePolicy().hasHeightForWidth());
    m_pNameEdit->setSizePolicy(sizePolicy);
    m_pNameEdit->setValidator(new QRegExpValidator(QRegExp("^(.{1,128})$")));
    QString blockname = m_pSourceProxy->name();
    if (blockname.isEmpty()) {
        m_pNameEdit->setText("Icon");
    } else {
        m_pNameEdit->setText(blockname);
    }
    pGridLayout->addWidget(m_pNameEdit, 0, 1, 1, 3);

    QPushButton *pImportIconButton = new QPushButton(this);
    pImportIconButton->setText(tr("Load Picture")); // 加载图片
    pImportIconButton->setMinimumSize(80, 25);
    connect(pImportIconButton, SIGNAL(clicked()), this, SLOT(onLoadIcon()));
    pGridLayout->addWidget(pImportIconButton, 1, 1, 1, 1);

    m_pIconPathElide = new ElideLineText(this);
    m_pIconPathElide->setElideText(m_pSourceProxy->getAnnotationMap()[Annotation::iconPath].toString());
    m_strIconPath = m_pSourceProxy->getAnnotationMap()[Annotation::iconPath].toString();
    pGridLayout->addWidget(m_pIconPathElide, 1, 2, 1, 1);

    QSpacerItem *vspacer = new QSpacerItem(20, 80, QSizePolicy::Minimum, QSizePolicy::Expanding);
    pGridLayout->addItem(vspacer, 2, 0, 1, 4);
    setLayout(pGridLayout);
    connect(m_pNameEdit, &QLineEdit::textChanged, this, [this](const QString &str) mutable { this->isChanged = true; });
}

void IconEditWidget::onLoadIcon()
{
    static QString lastpath = "";
    // 文件夹路径
    QString newFilePath = QFileDialog::getOpenFileName(this, tr("Select Image"), lastpath, "图片文件(*png; *bmp);;");
    if (!newFilePath.isEmpty()) {
        m_strIconPath = newFilePath;
        QFileInfo fileInfo = QFileInfo(m_strIconPath);
        // 文件名
        QString strFileName = fileInfo.baseName();
        lastpath = fileInfo.absolutePath();
        if (!(m_strIconPath.isEmpty())) {
            m_pNameEdit->setText(strFileName);
            m_pIconPathElide->setElideText(m_strIconPath);
            emit updateIconPath();
        }
    }
}
