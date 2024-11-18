#include "ViewToolBar.h"
#include "ActionManager.h"
#include "BookMark.h"

#include <QHBoxLayout>
#include <QListView>
#include <QMouseEvent>
#include <QPainter>
#include <QSizePolicy>
#include <QToolBar>
#include <QToolButton>

ViewToolBar::ViewToolBar(QWidget *parent, QSharedPointer<CanvasContext> ctx, QString title)
    : QWidget(parent), canvasCtx(ctx), _toolbar(new QToolBar(title, this))
{
    setWindowFlags(Qt::Widget);
    setVisible(true);
    setFixedHeight(35);
    _toolbar->move(10, 6);
    _toolbar->setObjectName("boardToolbar");
    _toolbar->setMaximumHeight(21);
    _toolbar->setIconSize(QSize(20, 20));
    _toolbar->setOrientation(Qt::Horizontal);
    //_toolbar->layout()->setContentsMargins(2, 2, 2, 2);
    //_toolbar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    _toolbar->setToolButtonStyle(Qt::ToolButtonIconOnly);
    //_toolbar->setStyleSheet("QToolBar::separator { background-color: #CECECE; width: 1px;}");

    QStringList strphaseTechText;
    strScaleText << tr("Page") << "20%"
                 << "50%"
                 << "75%"
                 << "100%"
                 << "125%"
                 << "150%"
                 << "200%"
                 << "400%"
                 << "500%" << tr("reserved");

    _scaleCombobox = new CustomComboBox(this);
    _scaleCombobox->setAccessibleName("ScalingRatio");
    if (_scaleCombobox->view()) { // 去除阴影
        if (_scaleCombobox->view()->window()) {
            _scaleCombobox->view()->window()->setWindowFlags(Qt::Popup | Qt::FramelessWindowHint
                                                             | Qt::NoDropShadowWindowHint);
        }
    }
    _scaleCombobox->setFixedSize(120, 21);
    _scaleCombobox->addItems(strScaleText);
    _scaleCombobox->setCurrentIndex(_scaleCombobox->count() - 1);
    connect(_scaleCombobox, SIGNAL(currentIndexChanged(const QString &)), this,
            SLOT(onScaleBoxIndexChanged(const QString &)));

    _volLevelCombobox = new QComboBox(this);
    _volLevelCombobox->setEditable(false);
    _volLevelCombobox->setMinimumWidth(100);
    _volLevelCombobox->setFixedHeight(21);
    _volLevelCombobox->setView(new QListView());
    //_volLevelCombobox->addItems(strVolLevelText);
    connect(_volLevelCombobox, SIGNAL(currentIndexChanged(const QString &)), this,
            SLOT(onVolLevelBoxIndexChanged(const QString &)));

    _phaseTechCombobox = new QComboBox(this);
    _phaseTechCombobox->setFixedSize(100, 21);
    _phaseTechCombobox->setEditable(false);
    _phaseTechCombobox->addItems(strphaseTechText);
    _phaseTechCombobox->setView(new QListView());
    connect(_phaseTechCombobox, SIGNAL(currentIndexChanged(const QString &)), this,
            SLOT(onPhaseTechBoxIndexChanged(const QString &)));

    bookMarkBtn = new QPushButton(this);

    initActions();
    _toolbar->setToolButtonStyle(Qt::ToolButtonIconOnly);

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setStyleSheet("QToolBar{border:0px solid #CDCDCD;spacing:8px;}");
}

ViewToolBar::~ViewToolBar() { }

bool ViewToolBar::eventFilter(QObject *obj, QEvent *e)
{
    if (obj == this) {
        static bool ispress(false);
        static int pressx(0), pressy(0);
        switch (e->type()) {
        case QEvent::MouseButtonPress:
            ispress = true;
            pressx = ((QMouseEvent *)e)->x();
            pressy = ((QMouseEvent *)e)->y();
            break;
        case QEvent::MouseMove:
            if (ispress) {
                int movex = ((QMouseEvent *)e)->x() - pressx;
                int movey = ((QMouseEvent *)e)->y() - pressy;
                move(x() + movex, y() + movey);
            }
            break;
        case QEvent::MouseButtonRelease:
            ispress = false;
            break;
        }
    }
    return QWidget::eventFilter(obj, e);
}

void ViewToolBar::addAction(QAction *action)
{
    _toolbar->addAction(action);
}

void ViewToolBar::addPushButton(QWidget *action)
{
    _toolbar->addWidget(action);
}

void ViewToolBar::insertComboBox(QAction *before, ComboBoxType BoxType)
{
    switch (BoxType) {
    case ZoomLevlel:
        _toolbar->insertWidget(before, _scaleCombobox);
        break;
    case DefaultVoltageLevel:
        _toolbar->insertWidget(before, _volLevelCombobox);
        break;
    case DefaultPhaseTechnologies:
        _toolbar->insertWidget(before, _phaseTechCombobox);
        break;
    default:
        break;
    }
}

void ViewToolBar::setAddSeparator()
{
    _toolbar->addSeparator();
}

void ViewToolBar::insertSeparator(QAction *before)
{
    _toolbar->insertSeparator(before);
}

void ViewToolBar::setScaleBoxIndexFitIn()
{
    _scaleCombobox->setCurrentIndex(0);
}

void ViewToolBar::setComboBoxEnableState(ComboBoxType BoxType, bool isEnable)
{
    switch (BoxType) {
    case ZoomLevlel:
        _scaleCombobox->setEnabled(isEnable);
        break;
    case DefaultVoltageLevel:
        _volLevelCombobox->setEnabled(isEnable);
        break;
    case DefaultPhaseTechnologies:
        _phaseTechCombobox->setEnabled(isEnable);
        break;
    default:
        break;
    }
}

void ViewToolBar::setComboBoxItemsList(QStringList list, ComboBoxType BoxType)
{
    switch (BoxType) {
    case ZoomLevlel:
        _scaleCombobox->addItems(list);
        break;
    case DefaultVoltageLevel: {
        QString currenttext = _volLevelCombobox->currentText();
        _volLevelCombobox->clear();
        _volLevelCombobox->addItems(list);
        int index = _volLevelCombobox->findText(currenttext);
        _volLevelCombobox->setCurrentIndex(index);
    } break;
    case DefaultPhaseTechnologies: {
        QString currenttext = _phaseTechCombobox->currentText();
        _phaseTechCombobox->clear();
        _phaseTechCombobox->addItems(list);
        int index = _phaseTechCombobox->findText(currenttext);
        _phaseTechCombobox->setCurrentIndex(index);
    } break;
    default:
        break;
    }
}

void ViewToolBar::setComboBoxVisibleState(bool isVisible)
{
    if (isVisible) {
        _volLevelCombobox->show();
        _phaseTechCombobox->show();
    } else {
        _volLevelCombobox->hide();
        _phaseTechCombobox->hide();
    }
    _volLevelCombobox->setVisible(isVisible);
    _phaseTechCombobox->setVisible(isVisible);
}

void ViewToolBar::setScaleComboboxText(double zoomSize)
{
    _scaleCombobox->setZoomSize(zoomSize);
}

void ViewToolBar::setReadOnly(bool readOnly)
{
    //_scaleCombobox->setReadOnly(readOnly);
}

QStringList ViewToolBar::getScaleCoefficient()
{
    return strScaleText;
}

QString ViewToolBar::getToolBarBoxValue(ComboBoxType boxtype)
{
    switch (boxtype) {
    case ViewToolBar::ZoomLevlel:
        if (_scaleCombobox != nullptr) {
            return _scaleCombobox->currentText();
        }
        break;
    case ViewToolBar::DefaultVoltageLevel:
        if (_volLevelCombobox != nullptr) {
            return _volLevelCombobox->currentText();
        }
        break;
    case ViewToolBar::DefaultPhaseTechnologies:
        if (_phaseTechCombobox != nullptr) {
            return _phaseTechCombobox->currentText();
        }
        break;
    default:
        break;
    }
    return QString("");
}

void ViewToolBar::setInsertWidget(QAction *before, QWidget *widget)
{
    _toolbar->insertWidget(before, widget);
}

#include <QStyleOption>
void ViewToolBar::paintEvent(QPaintEvent *e)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    QRect geo = geometry();
    p.fillRect(geo, QColor(Qt::white));
    p.setPen(QColor("#FFFFFF"));
    QPoint reserve = QPoint(8, 0);
    p.drawLine(geo.bottomLeft(), geo.bottomLeft() + reserve);

    // fixme 不符合UI设计 暂时屏蔽代码
    // p.setPen(QColor("#C8C8C8"));
    // p.drawLine(geo.bottomLeft() + reserve, geo.bottomRight());

    return QWidget::paintEvent(e);
}

void ViewToolBar::initActions()
{
    ActionManager &mgr = ActionManager::getInstance();
    addAction(mgr.getAction(ActionManager::Lock));
    setAddSeparator();
    addAction(mgr.getAction(ActionManager::Undo));
    addAction(mgr.getAction(ActionManager::Redo));
    setAddSeparator();
    addAction(mgr.getAction(ActionManager::Magnify));
    addAction(mgr.getAction(ActionManager::ScreenDrag));
    addAction(mgr.getAction(ActionManager::FitIn));
    setAddSeparator();
    insertComboBox(mgr.getAction(ActionManager::FullScreen), ViewToolBar::ZoomLevlel);
    setAddSeparator();

    // 书签
    QPushButton *bookMarkBtn = getBookMarkButton();
    bookMarkBtn->setMenu(BookMark::getInstance().getBookMarkMenu());
    bookMarkBtn->setObjectName("bookmark");
    bookMarkBtn->setAccessibleName("BookMark");
    bookMarkBtn->setToolTip(QObject::tr("BookMark"));
    bookMarkBtn->setFixedSize(25, 25);
    addPushButton(bookMarkBtn);
    // end

    addAction(mgr.getAction(ActionManager::Search));
    addAction(mgr.getAction(ActionManager::FullScreen));
    setAddSeparator();
    addAction(mgr.getAction(ActionManager::Set));
    addAction(mgr.getAction(ActionManager::LayoutManager));
    setAddSeparator();

    if (canvasCtx && canvasCtx->type() == CanvasContext::kElectricalType) {
        insertComboBox(ActionManager::getInstance().getAction(ActionManager::PrintDrawBoard),
                       ViewToolBar::DefaultVoltageLevel);
        QFrame *vframe = new QFrame(); // 分割线
        vframe->setFrameShape(QFrame::VLine);
        vframe->setStyleSheet("QFrame{color:white;}");
        setInsertWidget(ActionManager::getInstance().getAction(ActionManager::PrintDrawBoard), vframe);
        // end
        insertComboBox(ActionManager::getInstance().getAction(ActionManager::PrintDrawBoard),
                       ViewToolBar::DefaultPhaseTechnologies);

        setAddSeparator();
    }

    addAction(mgr.getAction(ActionManager::SaveImage));
    addAction(mgr.getAction(ActionManager::PrintDrawBoard));
    setAddSeparator();
    addAction(mgr.getAction(ActionManager::Icon));
    addAction(mgr.getAction(ActionManager::Textbox));
    addAction(mgr.getAction(ActionManager::Rectangle));
    addAction(mgr.getAction(ActionManager::Circular));
    addAction(mgr.getAction(ActionManager::Segment));
    addAction(mgr.getAction(ActionManager::BrokenLine));
    addAction(mgr.getAction(ActionManager::Arrows));
    addAction(mgr.getAction(ActionManager::LineArrow));
    addAction(mgr.getAction(ActionManager::Arc));
    addAction(mgr.getAction(ActionManager::Polygon));

    auto list = findChildren<QToolButton *>();
    foreach (auto action, list) {
        if (action->defaultAction() == ActionManager::getInstance().getAction(ActionManager::Rectangle)) {
            action->setObjectName("Rectangle");
            action->setAccessibleName("InsertRectangle");
        } else if (action->defaultAction() == ActionManager::getInstance().getAction(ActionManager::Circular)) {
            action->setObjectName("Circular");
            action->setAccessibleName("InsertCircular");
        } else if (action->defaultAction() == ActionManager::getInstance().getAction(ActionManager::Polygon)) {
            action->setObjectName("Polygon");
            action->setAccessibleName("InsertPolygon");
        } else if (action->defaultAction() == ActionManager::getInstance().getAction(ActionManager::LineArrow)) {
            action->setObjectName("LineArrow");
            action->setAccessibleName("InsertLineArrow");
        } else if (action->defaultAction() == ActionManager::getInstance().getAction(ActionManager::Arrows)) {
            action->setObjectName("Arrows");
            action->setAccessibleName("InsertArrows");
        } else if (action->defaultAction() == ActionManager::getInstance().getAction(ActionManager::BrokenLine)) {
            action->setObjectName("BrokenLine");
            action->setAccessibleName("InsertBrokenLine");
        } else if (action->defaultAction() == ActionManager::getInstance().getAction(ActionManager::Segment)) {
            action->setObjectName("Segment");
            action->setAccessibleName("InsertSegment");
        } else if (action->defaultAction() == ActionManager::getInstance().getAction(ActionManager::Textbox)) {
            action->setObjectName("Textbox");
            action->setAccessibleName("InsertTextbox");
        } else if (action->defaultAction() == ActionManager::getInstance().getAction(ActionManager::Icon)) {
            action->setObjectName("Icon");
            action->setAccessibleName("InsertIcon");
        } else if (action->defaultAction() == ActionManager::getInstance().getAction(ActionManager::PrintDrawBoard)) {
            action->setObjectName("PrintDrawBoard");
            action->setAccessibleName("PrintDrawBoard");
        } else if (action->defaultAction() == ActionManager::getInstance().getAction(ActionManager::SaveImage)) {
            action->setObjectName("SaveImage");
            action->setAccessibleName("SaveImage");
        } else if (action->defaultAction() == ActionManager::getInstance().getAction(ActionManager::Set)) {
            action->setObjectName("Set");
            action->setAccessibleName("Set");
        } else if (action->defaultAction() == ActionManager::getInstance().getAction(ActionManager::FitIn)) {
            action->setObjectName("FitIn");
            action->setAccessibleName("InsertFitIn");
        } else if (action->defaultAction() == ActionManager::getInstance().getAction(ActionManager::ScreenDrag)) {
            action->setObjectName("ScreenDrag");
            action->setAccessibleName("ScreenDrag");
        } else if (action->defaultAction() == ActionManager::getInstance().getAction(ActionManager::Magnify)) {
            action->setObjectName("Magnify_n");
            action->setAccessibleName("Magnify");
        } else if (action->defaultAction() == ActionManager::getInstance().getAction(ActionManager::Arc)) {
            action->setObjectName("Arc");
            action->setAccessibleName("InsertArc");
        } else if (action->defaultAction() == ActionManager::getInstance().getAction(ActionManager::LayoutManager)) {
            action->setObjectName("LayoutManager");
            action->setAccessibleName("LayoutManager");
        } else if (action->defaultAction() == ActionManager::getInstance().getAction(ActionManager::FullScreen)) {
            action->setObjectName("FullScreen");
            action->setAccessibleName("FullScreen");
            actionToolButtonMap.insert(action->defaultAction(), action);
        } else if (action->defaultAction() == ActionManager::getInstance().getAction(ActionManager::Undo)) {
            action->setObjectName("Undo");
            action->setAccessibleName("Undo");
        } else if (action->defaultAction() == ActionManager::getInstance().getAction(ActionManager::Redo)) {
            action->setObjectName("Redo");
            action->setAccessibleName("Redo");
        } else if (action->defaultAction() == ActionManager::getInstance().getAction(ActionManager::Lock)) {
            action->setObjectName("Lock");
            action->setAccessibleName("Lock");
            actionToolButtonMap.insert(action->defaultAction(), action);
        }
    }
}

void ViewToolBar::onScaleBoxIndexChanged(const QString &text)
{
    emit scaleBoxIndexChanged(text);
}

void ViewToolBar::onVolLevelBoxIndexChanged(const QString &text)
{
    emit volLevelBoxIndexChanged(text);
}

void ViewToolBar::onPhaseTechBoxIndexChanged(const QString &text)
{
    emit phaseTechBoxIndexChanged(text);
}

CustomComboBox::CustomComboBox(QWidget *parent /*= 0*/) : QComboBox(parent)
{
    setView(new QListView());
    this->setEditable(false);
}

void CustomComboBox::setZoomSize(double size)
{
    int rowNum = this->count();
    this->setItemText(rowNum - 1, QString("%1%").arg(size));

    // 隐藏最后一个Item
    QListView *view = qobject_cast<QListView *>(this->view());
    view->setRowHidden(rowNum - 1, true);

    this->setCurrentIndex(rowNum - 1);
}

CustomComboBox::~CustomComboBox() { }
