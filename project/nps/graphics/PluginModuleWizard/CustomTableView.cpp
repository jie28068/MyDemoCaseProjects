#include "CustomTableView.h"

MyTableView::MyTableView(QLabel *label, QSortFilterProxyModel *sortModel, QWidget *parent)
    : QTableView(parent), messageLabel(label)
{
    // 设置代理模型
    setModel(sortModel);
    frozenTableView = new QTableView(this);
    frozenTableView->setModel(sortModel);
    frozenTableView->horizontalHeader()->setFixedHeight(26);
    frozenTableView->verticalHeader()->setDefaultSectionSize(26);
    horizontalHeader()->setFixedHeight(26);
    verticalHeader()->setDefaultSectionSize(26);
    init();

    connect(horizontalHeader(), &QHeaderView::sectionResized, this, &MyTableView::updateSectionWidth);
    connect(verticalHeader(), &QHeaderView::sectionResized, this, &MyTableView::updateSectionHeight);

    connect(frozenTableView->verticalScrollBar(), &QAbstractSlider::valueChanged, verticalScrollBar(),
            &QAbstractSlider::setValue);
    connect(verticalScrollBar(), &QAbstractSlider::valueChanged, frozenTableView->verticalScrollBar(),
            &QAbstractSlider::setValue);
}

void MyTableView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        QModelIndex index = indexAt(event->pos());
        if (index.isValid()) {
            int column = index.column();
            auto controlType = index.sibling(index.row(), ElecWizardTableModel::ControlTyep).data().toInt();
            auto type = index.sibling(index.row(), ElecWizardTableModel::ParamClassify).data().toString();
            auto str = index.sibling(index.row(), ElecWizardTableModel::ParamType).data().toString();
            if (ElecWizardTableModel::ParamRange == column && (str == Global::DataType_Double)) {
                messageLabel->setText(
                        QObject::tr("Real number entry rule: set expression. For example [-1,5],(0,)..."));

            } else if (ElecWizardTableModel::ParemPortPosition == column && type == QObject::tr("port")) {
                messageLabel->setText(
                        QObject::tr("Port location input rule: Set expression. For example,(1,5),(0,10)..."));
            } else {
                messageLabel->setText("");
            }
        }
        updateFrozenTableGeometry();
    }
    QTableView::mousePressEvent(event);
}

void MyTableView::init()
{
    //**TableView**//
    // 水平表头允许拖拽调整宽度
    horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    // 垂直表头不可见
    verticalHeader()->setVisible(false);
    // 点击表头允许排序
    setSortingEnabled(true);
    // 隐藏排序后出现的箭头
    horizontalHeader()->setSortIndicatorShown(false);
    // 任意键都可以来编辑表格中的单元格
    setEditTriggers(QTableView::AllEditTriggers);
    // 将tableview放到frozenTableView视图下，进行覆盖
    viewport()->stackUnder(frozenTableView);
    // 最后一列自动填充列宽
    horizontalHeader()->setStretchLastSection(true);
    // // 以像素为单位滚动
    // setHorizontalScrollMode(ScrollPerPixel);
    // setVerticalScrollMode(ScrollPerPixel);
    // end

    //**frozenTableView**//
    // 升序
    frozenTableView->sortByColumn(ElecWizardTableModel::ParamClassify, Qt::AscendingOrder);
    // 设置无焦点
    frozenTableView->setFocusPolicy(Qt::NoFocus);
    // 点击表头允许排序
    frozenTableView->setSortingEnabled(true);
    // 隐藏排序后出现的箭头
    frozenTableView->horizontalHeader()->setSortIndicatorShown(false);
    // 隐藏垂直表头
    frozenTableView->verticalHeader()->hide();
    // frozenTableView水平表头的各个段宽度无法被拖动调整大小
    frozenTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    // 设置和父table同步的光标选择单元格(必要)，修改统一数据
    frozenTableView->setSelectionModel(selectionModel());
    // 隐藏frozenTableView第一列以外的数据
    for (int col = 1; col < ElecWizardTableModel::Column::end; ++col)
        frozenTableView->setColumnHidden(col, true);
    // frozenTableView使用与tableview第一列相同的宽高
    frozenTableView->setColumnWidth(0, columnWidth(0));
    frozenTableView->setRowHeight(0, rowHeight(0));
    // frozenTableView不显示滑动条
    frozenTableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    frozenTableView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    frozenTableView->show();
    // // 以像素为单位滚动
    // frozenTableView->setVerticalScrollMode(ScrollPerPixel);
    // end
    updateFrozenTableGeometry();
}

void MyTableView::resizeEvent(QResizeEvent *event)
{
    QTableView::resizeEvent(event);
    updateFrozenTableGeometry();
}

void MyTableView::scrollTo(const QModelIndex &index, ScrollHint hint)
{
    if (index.column() > 0)
        QTableView::scrollTo(index, hint);
}

void MyTableView::updateFrozenTableGeometry(int row)
{
    // 获取单个单元格的高度
    QModelIndex index = model()->index(0, 0);
    QRect rect = visualRect(index);
    // 行数*行高+表头高+行数(边线1px)+表头边线
    int rows = row != -1 ? row : model()->rowCount();
    int height = rows * rect.height() + horizontalHeader()->height() + rows + 1;
    // 当前视图下的高度
    int height2 = viewport()->height() + horizontalHeader()->height();
    // 当表格未满时使用height，防止第一列出现多余的框
    int resultHeight = height2 < height ? height2 : height;
    // 设置第一列的几何大小(x,y,w,h)
    frozenTableView->setGeometry(verticalHeader()->width() + frameWidth(), frameWidth() - 1, columnWidth(0),
                                 resultHeight);
}

void MyTableView::updateSectionHeight(int logicalIndex, int oldSize, int newSize)
{
    frozenTableView->setRowHeight(logicalIndex, newSize);
}

void MyTableView::updateSectionWidth(int logicalIndex, int oldSize, int newSize)
{
    if (logicalIndex == 0) {
        frozenTableView->setColumnWidth(0, newSize);
        updateFrozenTableGeometry();
    }
}