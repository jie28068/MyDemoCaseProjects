#include "exportimage.h"

#include <QApplication>
#include <QTimer>
#include <windows.h>

QString ExportImage::lastpath = "/";
QQueue<QString> *ExportImage::paths = new QQueue<QString>;

ExportImage::ExportImage(QSharedPointer<CanvasContext> canvasContext, CanvasViewDefaultImpl *m_exportimage,
                         QWidget *parent)
    : p_CanvasViewDefaultImpl(m_exportimage), QWidget(parent), data(canvasContext)
{
    InitUi();
}

ExportImage::~ExportImage() { }

void ExportImage::falgclose(int close)
{
    data->setGridFlag(falg);
    if (lastpath == NULL || lastpath.isEmpty()) {
        if (paths->size() == 0) {
            lastpath = "/";
            return;
        }
        lastpath = paths->head();
    }
}

void ExportImage::onLabelIsHide(int value)
{
    if (mComboBox->currentIndex() == 0) {
        m_isborad->setVisible(true);
        m_isfull->setVisible(true);
        m_isline->setVisible(true);
    } else {
        m_isborad->setVisible(false);
        m_isfull->setVisible(false);
        m_isline->setVisible(false);
    }
}

void ExportImage::setBroad()
{
    if (m_isborad->isChecked()) {
        QPainter painter(&pix);
        QPen pen;
        pen.setColor(Qt::black);
        pen.setWidth(2);
        painter.setPen(pen);
        painter.drawRect(0, 0, pix.width(), pix.height());
        painter.end();
        this->setImage(pix);
    }
}

void ExportImage::changefull(int checkState)
{
    if (checkState == checkStateChecked) {
        if (m_isline->isChecked()) {
            rendererImage(true, true);
        } else {
            rendererImage(true, false);
        }
    } else {
        if (m_isline->isChecked()) {
            rendererImage(false, true);
        } else {
            rendererImage(false, false);
        }
    }
    setBroad();
}

void ExportImage::changeLine(int checkState)
{
    if (checkState == checkStateChecked) {
        if (m_isfull->isChecked() && m_isborad->isChecked()) {
            rendererImage(true, true);
            setBroad();
        } else if (m_isfull->isChecked() && !m_isborad->isChecked()) {
            rendererImage(true, true);
        } else if (!m_isfull->isChecked() && m_isborad->isChecked()) {
            rendererImage(false, true);
            setBroad();
        } else if (!m_isfull->isChecked() && !m_isborad->isChecked()) {
            rendererImage(false, true);
        }
    } else {
        if (m_isfull->isChecked() && m_isborad->isChecked()) {
            rendererImage(true, false);
            setBroad();
        } else if (m_isfull->isChecked() && !m_isborad->isChecked()) {
            rendererImage(true, false);
        } else if (!m_isfull->isChecked() && m_isborad->isChecked()) {
            rendererImage(false, false);
            setBroad();
        } else if (!m_isfull->isChecked() && !m_isborad->isChecked()) {
            rendererImage(false, false);
        }
    }
}

void ExportImage::changeBorad(int checkState)
{
    if (m_isfull->isChecked() && m_isline->isChecked()) {
        rendererImage(true, true);
    } else if (m_isfull->isChecked() && !m_isline->isChecked()) {
        rendererImage(true, false);
    } else if (!m_isfull->isChecked() && m_isline->isChecked()) {
        rendererImage(false, true);
    } else if (!m_isfull->isChecked() && !m_isline->isChecked()) {
        rendererImage(false, false);
    }
    if (checkState == checkStateChecked) {
        setBroad();
    }
}

void ExportImage::setImage(QPixmap pixmap)
{
    m_label->setScaledContents(true);
    // 整图是否选中
    pixmap.scaled(pixmap.width(), pixmap.height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    m_label->setPixmap(pixmap);
}

void ExportImage::InitUi()
{
    mComboBox = new QComboBox();
    mComboBox->setView(new QListView());
    mComboBox->addItems(QStringList() << tr("Image") << tr("PDF"));
    if (mComboBox->view()) {
        if (mComboBox->view()->window()) {
            mComboBox->view()->window()->setWindowFlags(Qt::Popup | Qt::FramelessWindowHint
                                                        | Qt::NoDropShadowWindowHint);
        }
    }
    m_isline = new QCheckBox(tr("Mesh"));       // 网格
    m_isborad = new QCheckBox(tr("Frame"));     // 边框
    m_isfull = new QCheckBox(tr("Full Image")); // 整图
    m_label = new MyLable(p_CanvasViewDefaultImpl);
    m_label->setMinimumSize(900, 500);
    QHBoxLayout *layout1 = new QHBoxLayout;
    layout1->setSpacing(6);
    layout1->setContentsMargins(11, 11, 11, 11);
    layout1->addWidget(m_label);
    QGridLayout *mygridLayout = new QGridLayout();
    mygridLayout->setSpacing(6);
    mygridLayout->addWidget(mComboBox, 0, 0, 1, 1);
    mygridLayout->addWidget(m_isfull, 3, 0, 1, 1);
    mygridLayout->addWidget(m_isline, 1, 0, 1, 1);
    mygridLayout->addWidget(m_isborad, 2, 0, 1, 1);
    QSpacerItem *verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    mygridLayout->addItem(verticalSpacer, 4, 0, 1, 1);
    layout1->addLayout(mygridLayout);
    m_ui = new QWidget;
    m_ui->setLayout(layout1);

    connect(m_isfull, SIGNAL(stateChanged(int)), this, SLOT(changefull(int)));
    connect(m_isline, SIGNAL(stateChanged(int)), this, SLOT(changeLine(int)));
    connect(m_isborad, SIGNAL(stateChanged(int)), this, SLOT(changeBorad(int)));
    connect(mComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onLabelIsHide(int)));
}

void ExportImage::saveFilePath(QString filepath)
{
    QFileInfo fileInfo = QFileInfo(filepath);
    lastpath = fileInfo.absolutePath();
    if (lastpath.size() != 0) {
        paths->enqueue(lastpath);
    }
    if (paths->size() > 2) {
        paths->dequeue();
    }
}

void ExportImage::CreatSaveImage()
{
    // 记录网格
    falg = data->gridFlag();
    my_saveimage =
            new KCustomDialog(QObject::tr("Export the artboard as an image"), m_ui, KBaseDlgBox::StandardButton::Cancel,
                              KBaseDlgBox::StandardButton::Cancel, p_CanvasViewDefaultImpl);
    my_saveimage->setStandardReturnCodeFlag(false);
    m_pushbutton_export = new QPushButton();
    m_pushbutton_export->setText(tr("Export"));
    my_saveimage->addCustomButton(m_pushbutton_export);
    my_saveimage->setDefaultButton(m_pushbutton_export);
    my_saveimage->installEventFilter(this);
    data->setGridFlag(CanvasContext::kGridNoneFlag);
    // 默认无网格，有边框，全图
    m_isfull->setChecked(true);
    m_isborad->setChecked(true);
    rendererImage(true, false);
    setBroad();
    connect(m_pushbutton_export, SIGNAL(clicked()), this, SLOT(ButtonExportImage()));
    connect(my_saveimage, SIGNAL(closed(int)), this, SLOT(falgclose(int)));
    my_saveimage->exec();
}

void ExportImage::rendererImage(bool flag, bool falgLine)
{
    if (!falgLine) {
        data->setGridFlag(CanvasContext::kGridNoneFlag);
    } else {
        if (falg == CanvasContext::kGridNoneFlag) {
            data->setGridFlag(CanvasContext::kGridShowDotGrid);
        } else {
            data->setGridFlag(falg);
        }
    }
    // 非整图时
    if (!flag) {
        QRectF viewRect = p_CanvasViewDefaultImpl->rect();
        QRectF senceRect = QRectF(0, 0, p_CanvasViewDefaultImpl->sceneRect().width() * data->scale(),
                                  p_CanvasViewDefaultImpl->sceneRect().height() * data->scale());
        QRectF result = viewRect.intersected(senceRect);
        auto asc = p_CanvasViewDefaultImpl->mapFromScene(0, 0);
        QPixmap pi(result.width(), result.height());
        pix = pi;
        pix.fill(Qt::white);
        QPainter painter /*(&pix)*/;
        painter.begin(&pix);
        painter.setRenderHint(QPainter::HighQualityAntialiasing);
        if (asc.x() < 0 && asc.y() < 0) { // 渲染时要分四种情况
            p_CanvasViewDefaultImpl->render(&painter, QRectF(), QRect(0, 0, result.width(), result.height()));
        } else if (asc.x() > 0 && asc.y() < 0) {
            p_CanvasViewDefaultImpl->render(&painter, QRectF(), QRect(asc.x(), 0, result.width(), result.height()));
        } else if (asc.x() < 0 && asc.y() > 0) {
            p_CanvasViewDefaultImpl->render(&painter, QRectF(), QRect(0, asc.y(), result.width(), result.height()));
        } else {
            p_CanvasViewDefaultImpl->render(&painter, QRectF(),
                                            QRect(asc.x(), asc.y(), result.width(), result.height()));
        }
        painter.end();
    } else {
        QPixmap pi(p_CanvasViewDefaultImpl->sceneRect().width() - 10,
                   p_CanvasViewDefaultImpl->sceneRect().height() - 10);
        pix = pi;
        pix.fill(Qt::white);
        QPainter painter(&pix);
        painter.setRenderHint(QPainter::HighQualityAntialiasing);
        p_CanvasViewDefaultImpl->getCanvasScene()->render(&painter);
        painter.end();
    }
    this->setImage(pix);
}

void ExportImage::ButtonExportImage()
{
    if (mComboBox->currentIndex() == 0) {
        // 记录保存路径
        QString filePath =
                QFileDialog::getSaveFileName(&QWidget(), tr("Export As Image"), lastpath, "BMP(*.bmp);;PNG(*.png)");
        saveFilePath(filePath);
        if (filePath == "" && pix.isNull())
            return;
        pix.save(filePath);
        my_saveimage->close();
    } else {
        pdfPrint();
    }
}

void ExportImage::pdfPrint()
{
    // 无网格
    data->setGridFlag(CanvasContext::kGridNoneFlag);
    // 用文件对话框设置输出路径
    QString file_path = QFileDialog::getSaveFileName(this, tr("Export As PDF"), lastpath, "*.pdf");
    saveFilePath(file_path);
    // 创建QPdfWriter,指定保存路径
    QPdfWriter pdfWriter(file_path);
    // 设置页面大小
    pdfWriter.setPageSize(QPagedPaintDevice::A4);
    // 创建QPainter
    QPainter painter(&pdfWriter);
    // 启用抗锯齿
    painter.setRenderHint(QPainter::Antialiasing);
    //  绘制
    p_CanvasViewDefaultImpl->getCanvasScene()->render(&painter);
    // 关闭
    my_saveimage->close();
}

MyLable::MyLable(QWidget *parent)
    : QLabel(parent),
      m_scaleValue(1.0),
      m_mousePoint(0, 0),
      m_drawPoint(0, 0),
      m_rectPixmap(0, 0, 0, 0),
      m_isMousePress(0),
      SCALE_MAX_VALUE(10.0),
      SCALE_MIN_VALUE(0.5)
{
}

void MyLable::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    double width = this->width() * m_scaleValue;
    double height = this->height() * m_scaleValue;
    QPixmap scalePixmap =
            this->pixmap()->scaled(width, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation); // 饱满缩放
    m_rectPixmap = QRect(m_drawPoint.x(), m_drawPoint.y(), width, height);                          // 图片区域
    painter.drawPixmap(m_rectPixmap, scalePixmap);
}

void MyLable::mouseMoveEvent(QMouseEvent *event)
{
    if (m_isMousePress) {
        int x = event->pos().x() - m_mousePoint.x();
        int y = event->pos().y() - m_mousePoint.y();
        m_mousePoint = event->pos();
        m_drawPoint = QPointF(m_drawPoint.x() + x, m_drawPoint.y() + y);
        update();
    }
}

void MyLable::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_isMousePress = true;
        m_mousePoint = event->pos();
    }
}

void MyLable::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton) {
        m_drawPoint = QPointF(0, 0);
        m_scaleValue = 1.0;
        update();
    }
    if (event->button() == Qt::LeftButton)
        m_isMousePress = false;
}

void MyLable::wheelEvent(QWheelEvent *event)
{
    changeWheelValue(event->pos(), event->delta());
    event->accept();
}

void MyLable::resizeEvent(QResizeEvent *event)
{
    m_drawPoint = QPointF(0, 0);
    m_scaleValue = 1.0;
    update();
}

void MyLable::changeWheelValue(QPoint event, int numSteps)
{
    double oldScale = m_scaleValue;
    if (numSteps > 0) {
        m_scaleValue *= 1.1;
    } else {
        m_scaleValue *= 0.9;
    }
    if (m_scaleValue > (SCALE_MAX_VALUE)) {
        m_scaleValue = SCALE_MAX_VALUE;
    }
    if (m_scaleValue < (SCALE_MIN_VALUE)) {
        m_scaleValue = SCALE_MIN_VALUE;
    }

    if (m_rectPixmap.contains(event)) {
        double x = m_drawPoint.x()
                - (event.x() - m_drawPoint.x()) / m_rectPixmap.width() * (this->width() * (m_scaleValue - oldScale));
        double y = m_drawPoint.y()
                - (event.y() - m_drawPoint.y()) / m_rectPixmap.height() * (this->height() * (m_scaleValue - oldScale));
        m_drawPoint = QPointF(x, y);
    } else {
        double x = m_drawPoint.x() - (this->width() * (m_scaleValue - oldScale)) / 2;
        double y = m_drawPoint.y() - (this->height() * (m_scaleValue - oldScale)) / 2;
        m_drawPoint = QPointF(x, y);
    }
    update();
}
