#include "WizardIamgeSourceWidget.h"
USE_LOGOUT_("IamgeSourceInfoPage")
IamgeSourceInfoPage::IamgeSourceInfoPage(PControlBlock contrlBlock, PControlBlock ncontrlBlock, QWidget *parnt)
    : WizardPageBase(contrlBlock, ncontrlBlock, parnt), isElec(false)
{
    inItVariable();
}

IamgeSourceInfoPage::IamgeSourceInfoPage(PElectricalBlock elecBlock, PElectricalBlock nelecBlock, QWidget *parnt)
    : WizardPageBase(elecBlock, nelecBlock, parnt), isElec(true)
{
    inItVariable();
}

void IamgeSourceInfoPage::inIt(QVariantMap pic, bool isElec)
{
    this->isElec = isElec;
    pngGroupBox = new QGroupBox(QObject::tr("PNG Images"));
    QGridLayout *pnglayout = new QGridLayout(this);
    pngNormalLabel = new MyLable(Global::picNames.at(0), this);
    setInitLabelImage(pic.value(Kcc::BlockDefinition::PNG_NORMAL_PIC), pngNormalLabel);

    pngDisableLabel = new MyLable(Global::picNames.at(1), this);
    setInitLabelImage(pic.value(Kcc::BlockDefinition::PNG_DISABLE_PIC), pngDisableLabel);

    pngWarningLabel = new MyLable(Global::picNames.at(2), this);
    setInitLabelImage(pic.value(Kcc::BlockDefinition::PNG_WARNING_PIC), pngWarningLabel);

    pngErrorLabel = new MyLable(Global::picNames.at(3), this);
    setInitLabelImage(pic.value(Kcc::BlockDefinition::PNG_ERROR_PIC), pngErrorLabel);

    pnglayout->addWidget(pngNormalLabel, 0, 0, 1, 1);
    pnglayout->addWidget(pngDisableLabel, 0, 1, 1, 1);
    pnglayout->addWidget(pngWarningLabel, 1, 0, 1, 1);
    pnglayout->addWidget(pngErrorLabel, 1, 1, 1, 1);
    pngGroupBox->setLayout(pnglayout);

    svgGroupBox = new QGroupBox(QObject::tr("SVG Images"));
    QGridLayout *svglayout = new QGridLayout(this);
    svgNormalLabel = new MyLable(Global::svgNames.at(0), this);
    setInitLabelImage(pic.value(Kcc::BlockDefinition::SVG_0_PIC), svgNormalLabel, true);

    svgDisableLabel = new MyLable(Global::svgNames.at(1), this);
    setInitLabelImage(pic.value(Kcc::BlockDefinition::SVG_90_PIC), svgDisableLabel, true);

    svgWarningLabel = new MyLable(Global::svgNames.at(2), this);
    setInitLabelImage(pic.value(Kcc::BlockDefinition::SVG_180_PIC), svgWarningLabel, true);

    svgErrorLabel = new MyLable(Global::svgNames.at(3), this);
    setInitLabelImage(pic.value(Kcc::BlockDefinition::SVG_270_PIC), svgErrorLabel, true);

    svglayout->addWidget(svgNormalLabel, 0, 0, 1, 1);
    svglayout->addWidget(svgDisableLabel, 0, 1, 1, 1);
    svglayout->addWidget(svgWarningLabel, 1, 0, 1, 1);
    svglayout->addWidget(svgErrorLabel, 1, 1, 1, 1);
    svgGroupBox->setLayout(svglayout);

    QHBoxLayout *hlayout = new QHBoxLayout;
    hlayout->addWidget(pngGroupBox);
    hlayout->addWidget(svgGroupBox);

    QHBoxLayout *hlayout2 = new QHBoxLayout;
    QSpacerItem *spacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);
    moreImageBtn = new QPushButton(QObject::tr("Setting Up More Images"));
    if (moreImageBtn->text().size() > 10) {
        moreImageBtn->setMinimumSize(150, 25);
    } else {
        moreImageBtn->setMinimumSize(100, 25);
    }
    hlayout2->addItem(spacer);
    hlayout2->addWidget(moreImageBtn);

    QVBoxLayout *vlayout = new QVBoxLayout;
    vlayout->addLayout(hlayout);
#ifdef COMPILER_DEVELOPER_MODE
    vlayout->addLayout(hlayout2);
#endif

    setLayout(vlayout);

    connect(moreImageBtn, &QPushButton::clicked, this, &IamgeSourceInfoPage::creatMoreImageWidget);
}

void IamgeSourceInfoPage::inItVariable()
{
    names.append(QStringList() << Kcc::BlockDefinition::PNG_NORMAL_PIC << Kcc::BlockDefinition::PNG_DISABLE_PIC
                               << Kcc::BlockDefinition::PNG_WARNING_PIC << Kcc::BlockDefinition::PNG_ERROR_PIC
                               << Kcc::BlockDefinition::SVG_0_PIC << Kcc::BlockDefinition::SVG_90_PIC
                               << Kcc::BlockDefinition::SVG_180_PIC << Kcc::BlockDefinition::SVG_270_PIC);
    otherNames = names;
    messagelabel = nullptr;
    addButton = nullptr;
    dialog = nullptr;
}

QVariantMap IamgeSourceInfoPage::getSource()
{
    QVariantMap map;
    if (isElec) {
        map = m_pElecBloc->getResource();
    } else {
        map = m_pBlock->getResource();
    }
    if (pngNormalLabel->pixmap()) {
        map.insert(Kcc::BlockDefinition::PNG_NORMAL_PIC, getSource(pngNormalLabel));
    }
    if (pngDisableLabel->pixmap()) {
        map.insert(Kcc::BlockDefinition::PNG_DISABLE_PIC, getSource(pngDisableLabel));
    }
    if (pngWarningLabel->pixmap()) {
        map.insert(Kcc::BlockDefinition::PNG_WARNING_PIC, getSource(pngWarningLabel));
    }
    if (pngErrorLabel->pixmap()) {
        map.insert(Kcc::BlockDefinition::PNG_ERROR_PIC, getSource(pngErrorLabel));
    }

    if (svgNormalLabel->pixmap()) {
        map.insert(Kcc::BlockDefinition::SVG_0_PIC, getSource(svgNormalLabel, true));
    }
    if (svgDisableLabel->pixmap()) {
        map.insert(Kcc::BlockDefinition::SVG_90_PIC, getSource(svgDisableLabel, true));
    }
    if (svgWarningLabel->pixmap()) {
        map.insert(Kcc::BlockDefinition::SVG_180_PIC, getSource(svgWarningLabel, true));
    }
    if (svgErrorLabel->pixmap()) {
        map.insert(Kcc::BlockDefinition::SVG_270_PIC, getSource(svgErrorLabel, true));
    }
    return map;
}

QVariant IamgeSourceInfoPage::getSource(MyLable *label, bool isSvg)
{
    QVariant var;
    const QPixmap *pixmap = label->pixmap();
    if (pixmap) {
        if (isSvg) {
            var.setValue(label->getSvgData());
        } else {
            var.setValue(pixmap->toImage());
        }
    }
    return var;
}

void IamgeSourceInfoPage::setInitLabelImage(const QVariant &var, MyLable *label, bool isSvg)
{
    // png图片
    if (!var.isNull() && !isSvg) {
        QImage image = qvariant_cast<QImage>(var);
        label->setPixmap(QPixmap::fromImage(image));
    } else if (!var.isNull() && isSvg) {
        // Svg图片
        QSvgRenderer render;
        QPixmap pixmap(LABLE_WIDTH - 2, LABLE_HEIGHT - 2);
        pixmap.fill(Qt::transparent);
        QPainter painter(&pixmap);
        label->setSvgData(var.toByteArray());
        render.load(label->getSvgData());
        render.render(&painter);
        label->setPixmap(pixmap);
    }
}

bool IamgeSourceInfoPage::getIsElec()
{
    return isElec;
}

void IamgeSourceInfoPage::setMessageLabel(const QString &str)
{
    if (messagelabel) {
        messagelabel->setText(str);
    }
}

void IamgeSourceInfoPage::setButtonEnable(bool fale)
{
    if (addButton) {
        addButton->setEnabled(fale);
    }
}

void IamgeSourceInfoPage::setDialogEndble(bool falg)
{
    if (dialog) {
        for (auto button : dialog->buttons()) {
            button->setEnabled(falg);
        }
    }
}

void IamgeSourceInfoPage::saveMoreImage(PModel block)
{
    QVariantMap originalMap = block->getResource(); // 原始数据
    QVariantMap otherMap;                           // 八张图片
    QVariantMap newMap;                             // 新数据
    // 循环遍历原始的QVariantMap
    for (auto it = originalMap.begin(); it != originalMap.end(); ++it) {
        QString key = it.key();
        QVariant value = it.value();
        if (otherNames.contains(key)) {
            otherMap.insert(key, value);
        }
    }
    // 将表格中的数据读出
    int rowCount = table1->rowCount();
    for (int row = 0; row < rowCount; ++row) {
        QTableWidgetItem *item = table1->item(row, 0);
        MyMoreLabel *label = dynamic_cast<MyMoreLabel *>(table1->cellWidget(row, 1));
        QVariant var;
        if (label) {
            var.setValue(label->getSvgData());
            if (!newMap.contains(item->text())) {
                newMap.insert(item->text(), var);
            }
        }
    }

    int rowCount2 = table2->rowCount();
    for (int row = 0; row < rowCount2; ++row) {
        QTableWidgetItem *item = table2->item(row, 0);
        MyMoreLabel *label = dynamic_cast<MyMoreLabel *>(table2->cellWidget(row, 1));
        QVariant var;
        if (label) {
            var.setValue(label->getSvgData());
            if (!newMap.contains(item->text())) {
                newMap.insert(item->text(), var);
            }
        }
    }
    // 赋值给新map
    newMap.unite(otherMap);
    block->setResource(newMap);
}

void IamgeSourceInfoPage::inItMoreImage()
{
    table1 = new TableWidget(this);
    table1->setAlternatingRowColors(true);

    table2 = new TableWidget(this);
    table2->verticalHeader()->setVisible(false);
    table2->setAlternatingRowColors(true);

    QVariantMap map;
    if (isElec) {
        map = m_pElecBloc->getResource();
    } else {
        map = m_pBlock->getResource();
    }
    if (map.size() != 0) {
        int i = 0, j = 0;
        QVariantMap::iterator iter;
        for (iter = map.begin(); iter != map.end(); ++iter) {
            if (otherNames.contains(iter.key())) {
                continue;
            }
            if (i <= j) {
                table1->InitImage((iter.key()), iter.value(), i);
                names.append(iter.key());
                ++i;
            } else {
                table2->InitImage((iter.key()), iter.value(), j);
                names.append(iter.key());
                ++j;
            }
        }
    }

    addButton = new QPushButton(QObject::tr("Adding Images"));
    QObject::connect(addButton, &QPushButton::clicked, this, &IamgeSourceInfoPage::addRowTable);
    QObject::connect(table1, &TableWidget::menuEnableChanged, this, &IamgeSourceInfoPage::onMenuEnableChanged);
    QObject::connect(table2, &TableWidget::menuEnableChanged, this, &IamgeSourceInfoPage::onMenuEnableChanged);

    QGridLayout *layout = new QGridLayout;
    messagelabel = new QLabel;
    layout->addWidget(table1, 0, 0);
    layout->addWidget(table2, 0, 1);
    layout->addWidget(addButton, 2, 0, 1, 2);
    layout->addWidget(messagelabel, 1, 0);

    QWidget *widget = new QWidget;
    widget->setLayout(layout);
    widget->setMinimumSize(480, 400);
    dialog = new KCustomDialog(QObject::tr("Edit More Images"), widget);
}

void IamgeSourceInfoPage::initializePage()
{
    setSubTitle(QObject::tr("Double click add picture, right click edit picture, picture can be dragged, zoomed to "
                            "preview.")); // 双击添加图片，右键编辑图片，图片可以拖动，缩放方式进行预览。
    if (isElec) {
        inIt(m_pElecBloc->getResource(), true);
        return;
    }
    inIt(m_pBlock->getResource());
}

bool IamgeSourceInfoPage::validatePage()
{
    if (isElec) {
        m_pElecBloc->setResource(getSource());
    } else {
        m_pBlock->setResource(getSource());
    }
    return true;
}

void IamgeSourceInfoPage::addRowTable()
{
    // 获取两个表格的行数
    int rowCount1 = table1->rowCount();
    int rowCount2 = table2->rowCount();

    // 比较哪个表格行数较少
    if (rowCount1 <= rowCount2) {
        // 在表格1中添加一行
        table1->addRow();
    } else {
        // 在表格2中添加一行
        table2->addRow();
    }
}

void IamgeSourceInfoPage::onMenuEnableChanged(bool fagle)
{
    if (fagle) {
        table1->setContextMenuPolicy(Qt::ContextMenuPolicy::DefaultContextMenu);
        table2->setContextMenuPolicy(Qt::ContextMenuPolicy::DefaultContextMenu);
    } else {
        table1->setContextMenuPolicy(Qt::ContextMenuPolicy::NoContextMenu);
        table2->setContextMenuPolicy(Qt::ContextMenuPolicy::NoContextMenu);
    }
}

void IamgeSourceInfoPage::creatMoreImageWidget()
{
    inItMoreImage();
    if (dialog->exec() == KBaseDlgBox::Ok) {
        if (isElec) {
            saveMoreImage(m_pElecBloc);
        } else {
            saveMoreImage(m_pBlock);
        }
    }
}

void MyLable::switchFileImage()
{
    QString static filePath;
    if (Global::svgNames.contains(strs)) {
        filePath = QFileDialog::getOpenFileName(&QWidget(), strs, filePath, "SVG(*.svg)");
        if (!filePath.isEmpty()) {
            QFile file(filePath);
            file.open(QFile::ReadOnly);
            svgData = file.readAll();
            QSvgRenderer renderer(svgData);
            QPixmap pixmap(LABLE_WIDTH - 2, LABLE_HEIGHT - 2);
            pixmap.fill(Qt::transparent);
            QPainter painter(&pixmap);
            renderer.render(&painter);
            setPixmap(pixmap);
            show();
        }
    } else {
        filePath = QFileDialog::getOpenFileName(&QWidget(), strs, filePath, "PNG(*.png)");
        if (!filePath.isEmpty()) {
            QImage image;
            QFile file(filePath);
            file.open(QFile::ReadOnly);
            image.loadFromData(file.readAll());
            setPixmap(QPixmap::fromImage(image));
            show();
        }
    }
}

void MyLable::setSvgData(QByteArray data)
{
    svgData = data;
}

QByteArray MyLable::getSvgData()
{
    return svgData;
}

MyWidget::MyWidget(MyLable *label, QWidget *parnt) : QWidget(parnt), mLabel(label)
{
    QVBoxLayout *vlayout = new QVBoxLayout(this);
    QHBoxLayout *hlayout = new QHBoxLayout(this);
    edit = new KLineEdit;
    edit->setEnabled(false);
    edit->setPlaceholderText(QObject::tr("Please enter the formula"));
    soureButton = new QPushButton(QObject::tr("Resource Images"));
    drawButton = new QPushButton(QObject::tr("Self-drawn Images"));

    QString str = soureButton->text();
    QFontMetrics metrics(str);
    int with = metrics.width(str);
    if (with > 64) {
        soureButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        drawButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        drawButton->setMinimumSize(130, 25);
        soureButton->setMinimumSize(130, 25);
    }

    vlayout->addWidget(edit);
    hlayout->addWidget(soureButton);
    hlayout->addWidget(drawButton);
    vlayout->addLayout(hlayout);
    setLayout(vlayout);

    connect(soureButton, &QPushButton::clicked, this, &MyWidget::onSoureButton);
    connect(drawButton, &QPushButton::clicked, this, &MyWidget::onDrawButton);
}

void MyWidget::setKCustomDialog(KCustomDialog *dialog)
{
    if (dialog != nullptr) {
        mDialog = dialog;
    }
}

void MyWidget::onSoureButton()
{
    edit->setEnabled(false);
    mLabel->switchFileImage();
    // 选择完图片后二级弹窗直接关闭
    mDialog->close();
}

void MyWidget::onDrawButton()
{
    edit->setEnabled(true);
}

MyLable::MyLable(QString str, IamgeSourceInfoPage *image, QWidget *parent)
    : QLabel(parent),
      m_scaleValue(1.0),
      m_mousePoint(0, 0),
      m_drawPoint(0, 0),
      m_rectPixmap(0, 0, 0, 0),
      m_isMousePress(0),
      SCALE_MAX_VALUE(10.0),
      SCALE_MIN_VALUE(0.5)
{
    strs = str;
    imagePage = image;

    setAcceptDrops(true);
    setFixedSize(LABLE_WIDTH, LABLE_HEIGHT);
    setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setFrameStyle(QFrame::Panel | QFrame::Plain);
}

void MyLable::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QPen pen;
    pen.setWidth(1);
    pen.setColor(Qt::black);
    painter.setPen(pen);
    const QPixmap *pixmap = this->pixmap();
    painter.drawRect(1, 1, LABLE_WIDTH - 2, LABLE_HEIGHT - 2);
    if (!pixmap || pixmap->isNull()) {
        if (strs.size() >= 15) {
            painter.drawText(5, LABLE_HEIGHT / 2, strs);
        } else {
            painter.drawText(LABLE_WIDTH / 4, LABLE_HEIGHT / 2, strs);
        }
        return;
    }
    double width = this->width() * m_scaleValue;
    double height = this->height() * m_scaleValue;
    QPixmap scalePixmap = this->pixmap()->scaled(width - 10, height - 10, Qt::KeepAspectRatioByExpanding,
                                                 Qt::SmoothTransformation); // 饱满缩放
    m_rectPixmap =
            QRect(m_drawPoint.x() + 5, m_drawPoint.y() + 5, width - 10, height - 10); // 图片区域,不占满矩形且居中
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
    if (event->type() == QEvent::MouseButtonDblClick) {
        if (event->button() == Qt::LeftButton) {
            inputImage();
            return;
        }
    }
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

bool MyLable::event(QEvent *e)
{
    if (e->type() == QEvent::ToolTip) {
        QToolTip::showText(mapToGlobal(QPoint(0, 0)), strs);
        return true;
    }
    return QLabel::event(e);
}

void MyLable::contextMenuEvent(QContextMenuEvent *ev)
{
    QMenu *menu = new QMenu(this);
    QAction *input = new QAction(tr("import picture"), this);  // 导入图片
    QAction *output = new QAction(tr("export picture"), this); // 导出图片
    QAction *delect = new QAction(tr("delect picture"), this); // 删除图片
    menu->addAction(input);
    menu->addAction(output);
    menu->addAction(delect);
    QAction *act = menu->exec(QCursor::pos());
    if (act && act->text() == tr("import picture")) {
        inputImage();
    } else if (act && act->text() == tr("export picture")) {
        QString fileName;
        if (Global::svgNames.contains(strs)) {
            if (svgData.isEmpty()) {
                return LOGOUT(tr("Image does not exist!"), LOG_WARNING); // 图片不存在！
            }
            fileName = QFileDialog::getSaveFileName(&QWidget(), tr("export picture"), QString(), "SVG(*.svg)");
        } else {
            if (!pixmap() || pixmap()->isNull()) {
                return LOGOUT(tr("Image does not exist!"), LOG_WARNING);
            }
            fileName = QFileDialog::getSaveFileName(&QWidget(), tr("export picture"), QString(), "PNG(*.png)");
        }
        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly)) {
            return LOGOUT(tr("Failed to export the image!"), LOG_WARNING); // 导出图片失败！
        } else {
            if (Global::svgNames.contains(strs)) {
                file.write(svgData);
            } else {
                pixmap()->save(fileName);
            }
            file.close();
        }
    } else if (act && act->text() == tr("delect picture")) {
        if ((!pixmap() || pixmap()->isNull()) && svgData.isEmpty()) {
            return LOGOUT(tr("Image does not exist!"), LOG_WARNING);
        }
        if (!svgData.isEmpty()) {
            svgData = NULL;
        }
        if (KMessageBox::warning(this, tr("Warning"),
                                 tr("It cannot be restored after deletion. Do you want to confirm deletion?"),
                                 KMessageBox::Yes | KMessageBox::No) // 删除后不可恢复，是否确认删除？
            == KMessageBox::Yes) {
            setPixmap(QPixmap());
            update();
        }
    }
}

void MyLable::inputImage(bool checked)
{
    if (!imagePage->getIsElec() && Global::svgNames.contains(strs)) {
        // 二级弹窗
        MyWidget *myWidget = new MyWidget(this, nullptr);
        KCustomDialog *imageDialog = new KCustomDialog(QObject::tr("Image Type Selection"), myWidget,
                                                       KBaseDlgBox::Ok | KBaseDlgBox::Cancel, KBaseDlgBox::Ok);
        myWidget->setKCustomDialog(imageDialog);
        if (imageDialog->exec() == KBaseDlgBox::Ok) { }
    } else {
        switchFileImage();
    }
}

void IamgeSourceInfoPageOnModify::initializePage()
{
    setSubTitle(QObject::tr("Double click add picture, right click edit picture, picture can be dragged, zoomed to "
                            "preview.")); // 双击添加图片，右键编辑图片，图片可以拖动，缩放方式进行预览。
    if (isElec) {
        inIt(m_pElecBloc->getResource(), true);
    } else {
        inIt(m_pBlock->getResource());
    }
}

TableWidget::TableWidget(IamgeSourceInfoPage *page, QWidget *parent) : QTableWidget(parent), m_page(page)
{
    setColumnCount(2);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setHorizontalHeaderLabels(QStringList() << QObject::tr("Name") << QObject::tr("Image"));
    connect(this, &QTableWidget::cellDoubleClicked, this, &TableWidget::addImage);
    connect(this, &QTableWidget::cellChanged, this, &TableWidget::onCellChanged);
}

void TableWidget::InitImage(const QString str, QVariant &value, int order)
{
    // 创建表格
    insertRow(order);
    QTableWidgetItem *item0 = new QTableWidgetItem;
    item0->setText(str);
    setItem(order, 0, item0);
    QTableWidgetItem *item1 = new QTableWidgetItem;
    setItem(order, 1, item1);
    setRowHeight(order, CELL_SIZE);

    // 读取并设置图片
    MyMoreLabel *label = new MyMoreLabel();
    QSvgRenderer render;
    QPixmap pixmap(CELL_SIZE, CELL_SIZE);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    render.load(value.toByteArray());
    render.render(&painter);
    label->setPixmap(pixmap);
    label->setSvgData(value.toByteArray());
    setCellWidget(order, 1, label);
}

void TableWidget::addRow()
{
    int row = rowCount();
    int cloum = columnCount();
    insertRow(row);
    QTableWidgetItem *item0 = new QTableWidgetItem;
    setItem(row, 0, item0);
    QTableWidgetItem *item1 = new QTableWidgetItem;
    setItem(row, 1, item1);
    setRowHeight(row, CELL_SIZE);
    setRowHeight(cloum, CELL_SIZE);
}

void TableWidget::deleteRow()
{
    removeRow(row);
}

void TableWidget::addImage(int row, int column)
{
    if (column == 1) {
        QString fileName =
                QFileDialog::getOpenFileName(this, QObject::tr("select image"), "", tr("Image Files (*.svg)"));
        if (!fileName.isEmpty()) {
            MyMoreLabel *label = new MyMoreLabel();
            QFile file(fileName);
            file.open(QFile::ReadOnly);
            auto svgData = file.readAll();
            QSvgRenderer renderer(svgData);
            QPixmap pixmap(CELL_SIZE, CELL_SIZE);
            pixmap.fill(Qt::transparent);
            QPainter painter(&pixmap);
            renderer.render(&painter);
            label->setPixmap(pixmap);
            label->setSvgData(svgData);
            setCellWidget(row, column, label);
        }
    }
}

void TableWidget::contextMenuEvent(QContextMenuEvent *event)
{
    QPoint pos = event->pos();
    QTableWidgetItem *item = itemAt(pos);
    if (item) {
        QMenu *menu = new QMenu(this);
        menu->addAction(QObject::tr("delete row"));
        QAction *act = menu->exec(QCursor::pos());
        QList<QTableWidgetItem *> selected_items = this->selectedItems(); // 获取选中行
        for (auto item : selected_items) {
            row = item->row();
        }
        if (act && act->text() == QObject::tr("delete row")) {
            deleteRow();
        }
    }
}

void TableWidget::onCellChanged(int row, int column)
{
    auto mitem = item(row, column);
    if (!mitem->text().isEmpty()) {
        if (m_page->names.contains(mitem->text())) {
            m_page->setMessageLabel(QString(QObject::tr("Name [%1] Repeat!")).arg(mitem->text()));
            m_page->setButtonEnable(false);
            m_page->setDialogEndble(false);
            emit menuEnableChanged(false);
        } else {
            m_page->names.append(mitem->text());
            m_page->setMessageLabel("");
            m_page->setButtonEnable(true);
            m_page->setDialogEndble(true);
            emit menuEnableChanged(true);
        }
    } else {
        m_page->setMessageLabel(QObject::tr("The name cannot be empty!"));
        m_page->setButtonEnable(false);
        m_page->setDialogEndble(false);
        emit menuEnableChanged(false);
    }
}