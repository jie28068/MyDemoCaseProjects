#include "CurveSettingTreeView.h"
#include "CoreLib/GlobalConfigs.h"
#include <QToolTip>
USE_LOGOUT_("CurveSettingTreeView")

CurveSettingTreeView::CurveSettingTreeView(QWidget *parent) : QTreeView(parent), _dataDictionaryServer(nullptr)
{
    setAcceptDrops(true);

    setDragEnabled(true);
    setDropIndicatorShown(true);
    setDragDropMode(QAbstractItemView::DropOnly);

    _dataDictionaryServer = RequestServer<IDataDictionaryServer>();
    if (!_dataDictionaryServer) {
        LOGOUT(tr("IDataDictionaryServer is not loaded"), LOG_ERROR); // IDataDictionaryServer 未加载
    }

    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    // setAlternatingRowColors(true);

    // setColumnWidth(0,256);

    this->setIconSize(QSize(80, 20));
    setRootIsDecorated(false);
    this->header()->setSectionResizeMode(QHeaderView::Stretch);
    this->header()->setStretchLastSection(false);
    this->header()->setSectionsMovable(false);
    this->header()->setDefaultAlignment(Qt::AlignCenter);
    setMouseTracking(true);
}

CurveSettingTreeView::~CurveSettingTreeView() { }

void CurveSettingTreeView::dropEvent(QDropEvent *event)
{
    QTreeView::dropEvent(event);
    if (!_dataDictionaryServer || !event->mimeData()->hasText())
        return;

    QString strData = event->mimeData()->text();
    QStringList strDataList = strData.split("|||", QString::SkipEmptyParts);
    if (strDataList.size() < 3 || strDataList.size() % 2 == 0)
        return;
    if (strDataList[0] != "DictionaryDrag" && strDataList[0] != "DictionaryDrags")
        return;

    unsigned long long cid = strDataList.at(1).toULongLong();
    PIDataDictionaryBuilder pBuilder = _dataDictionaryServer->GetDictionaryBuilder(cid);
    if (!pBuilder)
        return;

    for (int i = 0; i < (strDataList.size() - 1) / 2; i++) {
        PIElementBase pBase = pBuilder->FindElement(strDataList[2 + i * 2]);
        if (!pBase)
            continue;
        addVarible(pBase);
    }

    expandAll();
    event->acceptProposedAction();
}

void CurveSettingTreeView::dragEnterEvent(QDragEnterEvent *event)
{
    QTreeView::dragEnterEvent(event);
    if (event->mimeData()->hasText()) {
        event->acceptProposedAction();
        // event->accept();
    }
}

void CurveSettingTreeView::dragMoveEvent(QDragMoveEvent *event)
{
    QTreeView::dragMoveEvent(event);
    if (event->mimeData()->hasText()) {
        event->acceptProposedAction();
        // event->accept();
    }
}

void CurveSettingTreeView::addVarible(PIElementBase pBase)
{
    if (!pBase)
        return;
    if (pBase->Attribute("ParamType").toString() == "InterParam")
        return;

    CurveSettingTreeModel *pModel = dynamic_cast<CurveSettingTreeModel *>(model());
    if (!pModel)
        return;

    // unsigned long long id=pBase->Id();
    QList<PlotVaribleInfo> &varibleInfoList = pModel->getPlotVaribleInfoList();

    for (PlotVaribleInfo &varIter : varibleInfoList) {
        if (pBase->FullName() == varIter.Path) {
            LOGOUT("[" + varIter.Path + "]" + tr("has been added to the curve control and cannot be bound repeatedly"),
                   LOG_WARNING); // 已绑定曲线控件，不能重复绑定
            return;
        }
    }

    PlotVaribleInfo varibleInfo;

    varibleInfo.ElementBase = pBase;
    varibleInfo.Name = pBase->Name();
    varibleInfo.Path = pBase->FullName();
    QStringList &tmpSplit = varibleInfo.Path.split(".");
    if (tmpSplit.size() == 5u) {
        varibleInfo.Alias = tmpSplit[3] + "." + tmpSplit.back();
    }

    if (pBase->ElementType().testFlag(ElementType_Number)) {
        varibleInfo.Type = Number;
        varibleInfo.Rows = 1;
        varibleInfo.Cols = 1;
        PlotCurveInfo curveInfo;
        // curveInfo.Style = gConfGet(DefaultCurveStyle).toInt();
        curveInfo.Index = 0;
        curveInfo.Row = 0;
        curveInfo.Col = 0;
        curveInfo.Name = QString("%1").arg(varibleInfo.Name);
        // 如果info.CurveInfoList不是空，应该是之前保存过的而不是新获取的
        if (varibleInfo.CurveInfoList.size() < 1) {
            varibleInfo.CurveInfoList.push_back(curveInfo);
        }
    } else if (pBase->ElementType().testFlag(ElementType_NumberArray)) {
        PIElementNumberArray parray = QueryInterface<IElementNumberArray>(pBase);

        varibleInfo.Type = NumberArray;
        varibleInfo.Cols = parray->Count();
        varibleInfo.Rows = 1;

        for (int i = 0; i < varibleInfo.Cols; i++) {
            PlotCurveInfo curveInfo;
            // curveInfo.Style = gConfGet(DefaultCurveStyle).toInt();
            curveInfo.Index = i;
            curveInfo.Col = i;
            curveInfo.Row = 0;
            curveInfo.Name = QString("%1[%2]").arg(varibleInfo.Name).arg(curveInfo.Col);
            // 如果info.CurveInfoList不是空，应该是之前保存过的而不是新获取的
            if (varibleInfo.CurveInfoList.size() < i + 1) {
                varibleInfo.CurveInfoList.push_back(curveInfo);
            }
        }
    } else if (pBase->ElementType().testFlag(ElementType_NumberArray2D)) {
        PIElementNumberArray2D parray = QueryInterface<IElementNumberArray2D>(pBase);

        varibleInfo.Type = NumberArray2D;
        varibleInfo.Rows = parray->Row();
        varibleInfo.Cols = parray->Colum();
        for (int i = 0; i < varibleInfo.Rows; i++) {
            for (int j = 0; j < varibleInfo.Cols; j++) {
                PlotCurveInfo curveInfo;
                // curveInfo.Style = gConfGet(DefaultCurveStyle).toInt();
                curveInfo.Index = i * varibleInfo.Cols + j;
                curveInfo.Col = j;
                curveInfo.Row = i;
                curveInfo.Name = QString("%1[%2,%3]").arg(varibleInfo.Name).arg(curveInfo.Row).arg(curveInfo.Col);
                // 如果info.CurveInfoList不是空，应该是之前保存过的而不是新获取的
                if (varibleInfo.CurveInfoList.size() < (i * varibleInfo.Cols + j + 1)) {
                    varibleInfo.CurveInfoList.push_back(curveInfo);
                }
            }
        }
    }

    varibleInfo.ElementBase.clear();

    QVector<QColor> allHasUsed;
    QList<CurveSettingTreeItem *> &allChild = pModel->root()->childs();
    for (CurveSettingTreeItem *item : allChild) {
        allHasUsed << item->getItemData().Color;
    }

    for (auto &curveIter : varibleInfo.CurveInfoList) {
        bool setColor = false;
        for (const auto &x : defaultCurveColorVec) {
            if (allHasUsed.contains(x)) {
                continue;
            } else {
                curveIter.Color = x;
                setColor = true;
                break;
            }
        }
        if (!setColor) {
            curveIter.Color = genRandomColor();
            while (allHasUsed.contains(curveIter.Color)
                   || (curveIter.Color.red() >= 249 && curveIter.Color.green() >= 249
                       && curveIter.Color.blue() >= 249)) {
                curveIter.Color = genRandomColor();
            }
        }
        curveIter.Checked = true;
    }

    pModel->addPlotVaribleInfo(varibleInfo);
}

void CurveSettingTreeView::mouseMoveEvent(QMouseEvent *event)
{
    const QPoint &mousePos = event->pos();

    QModelIndex &mouseIndex = indexAt(mousePos);
    int index = mouseIndex.column();
    if (index != 0 && index != 6 && index != 7) {
        QToolTip::showText(event->globalPos(), "", this);
        return;
    }
    if (!mouseIndex.isValid()) {
        QToolTip::showText(event->globalPos(), "", this);
        return;
    }

    CurveSettingTreeModel *viewModel = static_cast<CurveSettingTreeModel *>(model());
    if (!viewModel) {
        QToolTip::showText(event->globalPos(), "", this);
        return;
    }
    CurveSettingTreeItem *item = viewModel->itemFromIndex(mouseIndex);
    if (!item || item->level() != 1) {
        QToolTip::showText(event->globalPos(), "", this);
        return;
    }
    QString &dataToDisplay = mouseIndex.data(Qt::DisplayRole).toString();
    QToolTip::showText(event->globalPos(), dataToDisplay, this);
}