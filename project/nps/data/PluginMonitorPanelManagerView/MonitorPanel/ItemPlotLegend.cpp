#include "ItemPlotLegend.h"
#include <QIcon>
#include <qmath.h>
#include <qwt_plot.h>

static const int ButtonFrame = 2;
static const int Margin = 2;

ItemPlotLegend::ItemPlotLegend(QWidget *parent) : QwtLegend(parent)
{
    // connect(this,SIGNAL(checked(const QVariant&,bool,int)),this,SLOT(onLegendChecked(const QVariant&,bool,int)));
}

ItemPlotLegend::~ItemPlotLegend() { }

QWidget *ItemPlotLegend::createWidget(const QwtLegendData &data) const
{
    Q_UNUSED(data);

    ItemPlotLegendWidget *label = new ItemPlotLegendWidget;
    label->setItemMode(defaultItemMode());

    connect(label, SIGNAL(clicked()), SLOT(itemClicked()));
    connect(label, SIGNAL(checked(bool)), SLOT(itemChecked(bool)));

    return label;
}

void ItemPlotLegend::updateWidget(QWidget *widget, const QwtLegendData &data)
{
    ItemPlotLegendWidget *label = qobject_cast<ItemPlotLegendWidget *>(widget);
    if (label) {
        label->setData(data);
        if (!data.value(QwtLegendData::ModeRole).isValid()) {
            label->setItemMode(defaultItemMode());
        }
    }
}

void ItemPlotLegend::wheelEvent(QWheelEvent *event)
{
    QwtLegend::wheelEvent(event);
    event->accept();
    return;
}

void ItemPlotLegend::onLegendChecked(const QVariant &itemInfo, bool on, int index)
{
    QwtPlot *plot = qobject_cast<QwtPlot *>(parent());
    if (!plot)
        return;

    QwtPlotItem *item = plot->infoToItem(itemInfo);
    if (!item)
        return;
    item->setVisible(on);

    QList<QWidget *> widgets = legendWidgets(itemInfo);
    if (widgets.size() > 0) {
        ItemPlotLegendWidget *w = qobject_cast<ItemPlotLegendWidget *>(widgets[0]);
        if (w) {
            w->setChecked(on);
        }
    }
    plot->replot();
}
////////////////////////////////////////////////////////////////////////////////////////////////
ItemPlotLegendWidget::ItemPlotLegendWidget(QWidget *parent) : QwtLegendLabel(parent)
{
    m_font.setFamily("微软雅黑");
    m_font.setPixelSize(12);
    setSpacing(0);
    setIndent(0);
    setMargin(0);
}

ItemPlotLegendWidget::~ItemPlotLegendWidget() { }

void ItemPlotLegendWidget::setData(const QwtLegendData &data)
{
    QwtLegendLabel::setData(data);
    setIcon(data.icon().toPixmap(QSize(16, 6)));
}

QSize ItemPlotLegendWidget::minimumSizeHint() const
{
    QSize sz = QwtLegendLabel::minimumSizeHint();
    sz.setHeight(sz.height() /*-4*/);
    return sz;

    // QSizeF sz = text().textSize( m_font );

    // int mw = 2 * ( frameWidth() + margin() );
    // int mh = mw;

    // int indent = this->indent();
    // if ( indent <= 0 )
    //{
    //	if ( frameWidth() > 0 )
    //	{
    //		indent = QFontMetrics( m_font ).width( 'x' ) / 2;
    //	}
    // }

    // if ( indent > 0 )
    //{
    //	const int align = text().renderFlags();
    //	if ( align & Qt::AlignLeft || align & Qt::AlignRight )
    //		mw += this->indent();
    //	else if ( align & Qt::AlignTop || align & Qt::AlignBottom )
    //		mh += this->indent();
    // }

    // sz += QSizeF( mw, mh );

    // return QSize( qCeil( sz.width() ), qCeil( sz.height() ) );
}

void ItemPlotLegendWidget::paintEvent(QPaintEvent *e)
{
    const QRect cr = contentsRect();

    QPainter painter(this);
    painter.setClipRegion(e->region());

    painter.setRenderHint(QPainter::Antialiasing, false);

    painter.setBrush(Qt::NoBrush);
    painter.setPen(QPen(Qt::black));

    if (isDown()) {
        // painter.drawRect(cr.adjusted(0,0,-1,-1));
    }

    painter.save();

    painter.setClipRect(cr);

    const QRect r = textRect();
    if (!r.isEmpty()) {
        painter.setFont(m_font);
        QPen pen;
        if (isDown()) {
            pen.setColor("#333333");
            // pen.setColor(palette().color(QPalette::Active, QPalette::Text));
        } else {
            pen.setColor(Qt::gray);
        }
        painter.setPen(pen);

        drawText(&painter, QRectF(r));
    }

    if (!icon().isNull()) {
        QRect iconRect = cr;
        iconRect.setX(iconRect.x() + margin());
        if (itemMode() != QwtLegendData::ReadOnly)
            iconRect.setX(iconRect.x() + ButtonFrame);

        iconRect.setSize(icon().size());
        iconRect.moveCenter(QPoint(iconRect.center().x(), cr.center().y()));
        painter.drawPixmap(iconRect, icon());
    }

    painter.restore();
}
