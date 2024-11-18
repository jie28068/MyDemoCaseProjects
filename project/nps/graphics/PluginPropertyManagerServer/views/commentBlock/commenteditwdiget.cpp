#include "commenteditwdiget.h"
#include "KLWidgets/KColorDialog.h"
#include <QHBoxLayout>
#include <QPainter>

ColorLabel::ColorLabel(QColor color, QWidget *parent) : QLabel(parent), m_color(color) { }

ColorLabel::~ColorLabel() { }

void ColorLabel::updateColor(QColor color)
{
    m_color = color;
}

void ColorLabel::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(m_color);
    painter.setBrush(m_color);
    int startY = (this->size().height() - 20) / 2;
    painter.drawRect(0, startY, 55, 20);
}

ColorWidget::ColorWidget(QColor color, QWidget *parent) : QLineEdit(parent), m_color(color)
{
    m_pCssLabel = new ColorLabel(m_color, this);
    m_pLabel = new QLabel(this);
    // m_pLabel->setMinimumSize(50, 24);
    m_pLabel->setText(m_color.name());
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(m_pCssLabel);
    layout->addWidget(m_pLabel);
    layout->setContentsMargins(5, 0, 5, 0);
    layout->setSpacing(0);
    // layout->addStretch();
    setLayout(layout);
    setReadOnly(true);
}

ColorWidget::~ColorWidget() { }

void ColorWidget::mouseDoubleClickEvent(QMouseEvent *e)
{
    KColorDialog dlg(tr("Select Color"), this);
    dlg.setCurrentColor(m_color);
    if (dlg.exec() == KColorDialog::Ok) {
        m_color = dlg.currentColor();
        m_pCssLabel->updateColor(m_color);
        m_pLabel->setText(m_color.name());
    }
}

QString ColorWidget::getColor() const
{
    return m_color.name();
}

CommentEditWdiget::CommentEditWdiget(QSharedPointer<SourceProxy> pSource, QWidget *parent)
    : QWidget(parent), m_pSourceProxy(pSource)
{
    initDataList();
}

void CommentEditWdiget::initDataList()
{
    m_vecWidthList = QVector<qreal>() << 0.1 << 0.2 << 0.3 << 0.4 << 0.5 << 0.6 << 0.7 << 0.8 << 1.2 << 1.8 << 2.4
                                      << 3.2;
}

void CommentEditWdiget::comboBoxFillStyleItems(QComboBox *box)
{
    // box->addItems(itemNameList);
}

QListmodel::QListmodel(QObject *parent) { }

QListmodel::~QListmodel() { }

QVariant QListmodel::data(const QModelIndex &index, int role) const
{
    if (Qt::DisplayRole == role) {
        return InfoList.at(index.row()).number;

    } else if (Qt::UserRole == role) {
        return InfoList.at(index.row()).lineValue;
    }
    return QVariant();
}

int QListmodel::rowCount(const QModelIndex &parent) const
{
    return InfoList.size();
}

void QListmodel::setModelData(const QList<UserInfo> &data)
{
    if (!InfoList.isEmpty())
        InfoList.clear();

    InfoList = data;
}

comboboxDelegate::comboboxDelegate(QObject *parent /*= 0*/, int type)
{
    ItemHeight = 26;
    delegateType = type;
}

comboboxDelegate::~comboboxDelegate() { }

void comboboxDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.isValid()) {
        painter->save();
        QString number = index.data(Qt::DisplayRole).toString();
        int lintValue = index.data(Qt::UserRole).toInt();

        QRectF rect;
        rect.setX(option.rect.x());
        rect.setY(option.rect.y());
        rect.setWidth(option.rect.width() - 1);
        rect.setHeight(option.rect.height() - 1);

        QPainterPath path;
        path.moveTo(rect.topRight());
        path.lineTo(rect.topLeft());
        path.quadTo(rect.topLeft(), rect.topLeft());
        path.lineTo(rect.bottomLeft());
        path.quadTo(rect.bottomLeft(), rect.bottomLeft());
        path.lineTo(rect.bottomRight());
        path.quadTo(rect.bottomRight(), rect.bottomRight());
        path.lineTo(rect.topRight());
        path.quadTo(rect.topRight(), rect.topRight());

        if (option.state.testFlag(QStyle::State_Selected)) {
            painter->setPen(QPen(QColor("#036EB7")));
            painter->setBrush(QColor("#036EB7"));
            painter->drawPath(path);
        }
        // else if(option.state.testFlag(QStyle::State_Raised))
        //{
        //	painter->setPen(QPen(Qt::green));
        //	painter->setBrush(QColor(0,127,255));
        //	painter->drawPath(path);
        // }
        else {
            painter->setPen(QPen(Qt::gray));
            painter->setBrush(QColor(255, 255, 255));
            painter->drawPath(path);
        }

        int textRectWidth = 10, textRectHeigh = 15;
        // 画编号
        QRect textRect =
                QRect(rect.x() + 10, rect.y() + rect.height() / 2 - textRectHeigh / 2, textRectWidth, textRectHeigh);
        // painter->setPen(QPen(QColor(0, 0, 0)));
        // painter->setFont(QFont("微软雅黑", 9, QFont::Normal));
        // painter->drawText(textRect, Qt::AlignLeft, QString("%1").arg(number));

        // 画直线
        QPen pen;
        pen.setColor(Qt::black);
        if (delegateType == 0) {
            pen.setWidthF(3);
            pen.setStyle((Qt::PenStyle)lintValue);
        }
        if (delegateType == 1) {
            pen.setWidthF(lintValue);
            pen.setStyle(Qt::SolidLine);
        }
        painter->setPen(pen);
        painter->drawLine(QLine(textRect.x() + textRectWidth + 10, textRect.y() + textRectHeigh / 2, rect.width() - 40,
                                textRect.y() + textRectHeigh / 2));
        painter->restore();
    }
}

QSize comboboxDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)
    Q_UNUSED(index)

    return QSize(200, ItemHeight);
}

customLineStyleCombobox::customLineStyleCombobox(QWidget *parent /*= nullptr*/)
{
    this->setParent(parent);
    setStyleSheet("");
    m_lineStyleList = QList<int>() << Qt::SolidLine << Qt::DashLine << Qt::DotLine << Qt::DashDotLine
                                   << Qt::DashDotDotLine;
    itemNameList = QStringList() << tr("solid line")       // 实线
                                 << tr("dashed line")      // 虚线
                                 << tr("dotted line")      // 点线
                                 << tr("dot dash")         // 点划线
                                 << tr("double dot dash"); // 双点划线
    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(onCurIndexChange(int)));
}

customLineStyleCombobox::~customLineStyleCombobox() { }

QList<int> customLineStyleCombobox::getStyleList()
{
    return m_lineStyleList;
}

int customLineStyleCombobox::findIndexByItemText(QString text)
{
    for (int i = 0; i < itemNameList.size(); i++) {
        if (text == itemNameList[i]) {
            return i;
        }
    }
    return 0;
}

void customLineStyleCombobox::paintEvent(QPaintEvent *e)
{
    QComboBox::paintEvent(e);
    QPainter *painter = new QPainter(this);
    painter->save();

    int lintStyle = m_lineStyleList[this->currentIndex()];

    QRectF rect(0, 0, this->rect().width(), 26);

    QPainterPath path;

    int textRectWidth = 10, textRectHeigh = 15;

    QRect wihteRect =
            QRect(rect.x() + 5, rect.y() + rect.height() / 2 - textRectHeigh / 2, textRectWidth + 5, textRectHeigh);
    // painter->setPen(QPen(Qt::white));
    // painter->setBrush(QColor(255, 255, 255));
    painter->eraseRect(wihteRect);

    // 画编号
    QRect textRect =
            QRect(rect.x() + 10, rect.y() + rect.height() / 2 - textRectHeigh / 2, textRectWidth, textRectHeigh);

    // painter->setPen(QPen(QColor(0, 0, 0)));
    // painter->setFont(QFont("微软雅黑", 9, QFont::Normal));
    // painter->drawText(textRect, Qt::AlignLeft, QString("%1").arg(lintStyle));

    // 画直线
    QPen pen;
    pen.setColor(Qt::black);
    pen.setWidthF(3);
    pen.setStyle((Qt::PenStyle)lintStyle);
    painter->setPen(pen);
    painter->drawLine(QLine(textRect.x() + textRectWidth + 10, textRect.y() + textRectHeigh / 2, rect.width() - 40,
                            textRect.y() + textRectHeigh / 2));
    painter->restore();
}

void customLineStyleCombobox::onCurIndexChange(int index)
{
    setCurrentIndex(index);
}

customLineWidthCombobox::customLineWidthCombobox(QWidget *parent /*= nullptr*/)
{
    this->setParent(parent);
    m_lineWidthList = QList<int>() << 1 << 2 << 3 << 4 << 5 << 6 << 7 << 8 << 9;

    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(onCurIndexChange(int)));
}

customLineWidthCombobox::~customLineWidthCombobox() { }

QList<int> customLineWidthCombobox::getWidthList()
{
    return m_lineWidthList;
}

int customLineWidthCombobox::findIndexByWidth(int width)
{
    for (int i = 0; i < m_lineWidthList.size(); i++) {
        if (width == m_lineWidthList[i]) {
            return i;
        }
    }
    return 0;
}

void customLineWidthCombobox::paintEvent(QPaintEvent *e)
{
    QComboBox::paintEvent(e);
    QPainter *painter = new QPainter(this);
    painter->save();

    int lintWidth = m_lineWidthList[this->currentIndex()];

    QRectF rect(0, 0, this->rect().width(), 26);

    QPainterPath path;

    int textRectWidth = 10, textRectHeigh = 15;

    QRect wihteRect =
            QRect(rect.x() + 5, rect.y() + rect.height() / 2 - textRectHeigh / 2, textRectWidth + 5, textRectHeigh);
    // painter->setPen(QPen(Qt::white));
    // painter->setBrush(QColor(255, 255, 255));
    painter->eraseRect(wihteRect);

    // 画编号
    QRect textRect =
            QRect(rect.x() + 10, rect.y() + rect.height() / 2 - textRectHeigh / 2, textRectWidth, textRectHeigh);

    // painter->setPen(QPen(QColor(0, 0, 0)));
    // painter->setFont(QFont("微软雅黑", 9, QFont::Normal));
    // painter->drawText(textRect, Qt::AlignLeft, QString("%1").arg(lintWidth));

    // 画直线
    QPen pen;
    pen.setColor(Qt::black);
    pen.setWidthF(lintWidth);
    pen.setStyle(Qt::SolidLine);
    painter->setPen(pen);
    painter->drawLine(QLine(textRect.x() + textRectWidth + 10, textRect.y() + textRectHeigh / 2, rect.width() - 40,
                            textRect.y() + textRectHeigh / 2));
    painter->restore();
}

void customLineWidthCombobox::onCurIndexChange(int index)
{
    setCurrentIndex(index);
}
