#include "CurveSettingTreeModel.h"
#include "KLWidgets/KColorDialog.h"
#include "KLWidgets/KMessageBox.h"
#include <QListView>

// 线型
// #define LINE "曲线"
// #define STEP "折线"
// #define STICKS "柱状线"
// #define DOTTEDLINE "虚线"

ColorLabelDelegate::ColorLabelDelegate(QObject *parent) : QStyledItemDelegate(parent) { }

ColorLabelDelegate::~ColorLabelDelegate() { }

// QWidget *ColorComboBoxItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
//                                                  const QModelIndex &index) const
//{
//     QComboBox *comboBox = new QComboBox(parent);
//     comboBox->setEditable(false);
//     int k = 0;
//     comboBox->setIconSize(m_iconSize);
//
//     for (int i = 0; i < CURVECOLORS_COUNT; i++) {
//         QPixmap pix(m_iconSize);
//         QColor clr(CURVECOLORS[i]);
//         pix.fill(clr);
//         comboBox->addItem(QIcon(pix), QString());
//         comboBox->setItemData(k, clr);
//         k++;
//     }
//
//     return comboBox;
// }

// void ColorComboBoxItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
//{
//     QComboBox *comboBox = static_cast<QComboBox *>(editor);
//     QColor clr = index.model()->data(index, Qt::EditRole).value<QColor>();
//     comboBox->setCurrentIndex(comboBox->findData(clr));
// }

// void ColorComboBoxItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index)
// const
//{
//     QComboBox *comboBox = static_cast<QComboBox *>(editor);
//
//     model->setData(index, comboBox->itemData(comboBox->currentIndex()), Qt::EditRole);
// }

void ColorLabelDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                                              const QModelIndex &index) const
{
    editor->setGeometry(option.rect);
}

void ColorLabelDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyledItemDelegate::paint(painter, option, index);
    QColor color = index.data(Qt::EditRole).value<QColor>();
    painter->fillRect(option.rect.x() + 4, option.rect.y() + 8, option.rect.width() - 7, option.rect.height() - 15,
                      color);
    QPen tempPen(Qt::gray);
    tempPen.setWidth(1);
    painter->setPen(tempPen);
    painter->drawRoundedRect(option.rect.adjusted(2, 6, -2, -6), 0, 0);
}

bool ColorLabelDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option,
                                     const QModelIndex &index)
{
    if (!index.isValid() || index.column() != 2)
        return false;
    switch (event->type()) {
    case QEvent::MouseButtonDblClick: {
        KColorDialog dlg(tr("Select Color"), (QWidget *)(this->parent()));
        QColor color = index.data(Qt::EditRole).value<QColor>();
        dlg.setCurrentColor(color);
        if (dlg.exec() != KColorDialog::Ok)
            return false;
        color = dlg.currentColor();
        if (color.isValid()) {
            model->setData(index, color, Qt::EditRole);
        }
    } break;
    }
    return QStyledItemDelegate::editorEvent(event, model, option, index);
}

//////////////////////////////////////////////////////////////////////////////////////

CurveSettingTreeItem::CurveSettingTreeItem(const PlotCurveInfo &itemData, int type,
                                           CurveSettingTreeItem *parent /*=nullptr*/)
    : m_itemData(itemData), m_parentItem(parent), m_isChecked(false), m_level(0), m_type(type)
{
    if (m_parentItem) {
        m_level = m_parentItem->level() + 1;
    }
}

CurveSettingTreeItem::CurveSettingTreeItem(int type, CurveSettingTreeItem *parent /*=nullptr*/)
    : m_parentItem(parent), m_isChecked(false), m_level(0), m_type(type)
{
    if (m_parentItem) {
        m_level = m_parentItem->level() + 1;
    }
}

CurveSettingTreeItem::~CurveSettingTreeItem()
{
    qDeleteAll(m_childItems);
}

QVariant CurveSettingTreeItem::data(int col, int role)
{
    if (role == Qt::TextAlignmentRole) {
        return Qt::AlignCenter;
    }

    switch (col) {
    case 0: {
        if (role == Qt::DisplayRole) {
            if (m_level > 1)
                return m_itemData.Index;
            else {
                if (m_type == 0) {
                    QString strTemp;
                    QString path = m_varibleInfo.Path;
                    path.replace("Dictionary.", "");
                    switch (m_varibleInfo.Type) {
                    case Number:
                        return QString("%1[%2]").arg(m_varibleInfo.Name).arg(path);
                    case NumberArray:
                        strTemp = QObject::tr("One-dimensional"); // 一维
                        break;
                    case NumberArray2D:
                        strTemp = QObject::tr("Second-dimensional"); // 二维
                        break;
                    default:
                        return QVariant();
                    }
                    return QString("%1[%2][%3]").arg(m_varibleInfo.Name).arg(path).arg(strTemp);
                } else {
                    return m_varibleInfo.Name;
                }
            }
        }

        /*else if(role==Qt::CheckStateRole)			//2022.11.9 modify by huangzhi
        {
        if(m_level>1)
        return m_itemData.Checked ? Qt::Checked : Qt::Unchecked;
        }*/

        /*	else if(role==Qt::ToolTipRole)
        {
        if(m_level == 1)
        {
        QString path = m_varibleInfo.Path;
        path.replace("Dictionary.","");
        return m_varibleInfo.Name+"["+path+"]";
        }
        }*/
    } break;
    case 1: {
        if (role == Qt::DisplayRole && (m_level > 1 || m_varibleInfo.Type == Number)) {

            switch (m_varibleInfo.Type) {
            case Number:
                return "";
            case NumberArray:
                return QString("[%1]").arg(m_itemData.Col);
            case NumberArray2D:
                return QString("[%1,%2]").arg(m_itemData.Row).arg(m_itemData.Col);
            default:
                break;
            }

            // return QString("[%1,%2]").arg(m_itemData.Row).arg(m_itemData.Col);
        }
    } break;
    case 2: {
        if (role == Qt::EditRole && (m_level > 1 || m_varibleInfo.Type == Number)) {
            return m_itemData.Color;
        }
        // if (role == Qt::DecorationRole && (m_level > 1 || m_varibleInfo.Type == Number)) {
        //     QPixmap pixmap(80, 20);
        //     pixmap.fill(m_itemData.Color);
        //     return pixmap;
        // }
    } break;
    case 3: {
        if (m_level > 1 || m_varibleInfo.Type == Number) {
            if ((role == Qt::DisplayRole || role == Qt::EditRole))
                return m_itemData.Width;
        }
    } break;
    case 4: {
        if (m_level > 1 || m_varibleInfo.Type == Number) {
            if (role == Qt::EditRole)
                return m_itemData.Style;
            else if (role == Qt::TextAlignmentRole)
                return Qt::AlignLeft | Qt::AlignVCenter;
            else if (role == Qt::DecorationRole) {
                if (m_itemData.Style == 0)
                    return QIcon(":/straightLine");
                else if (m_itemData.Style == 1)
                    return QIcon(":/dotline1");
                else if (m_itemData.Style == 2)
                    return QIcon(":/dotline2");
                else if (m_itemData.Style == 3)
                    return QIcon(":/dotline3");

            } else if (role == Qt::DisplayRole) {
                // if(m_itemData.Style == 0)
                //	return LINE;
                // else if(m_itemData.Style == 1)
                //	return STEP;
                // else if (m_itemData.Style == 2)
                //	return STICKS;
                // else
                //     return DOTTEDLINE;
            }
        }
    } break;
    case 5: {
        if (m_level > 1 || m_varibleInfo.Type == Number) {
            if ((role == Qt::DisplayRole || role == Qt::EditRole))
                return m_itemData.Name;
        }
    } break;
    case 6: {
        if (m_type != 0) {
            if (role == Qt::DisplayRole && m_level == 1) {
                QString path = m_varibleInfo.Path;
                path.replace("Dictionary.", "");
                int pos = path.lastIndexOf(m_varibleInfo.Name);
                pos--;
                if (pos >= 0) {
                    path = path.left(pos);
                }
                return path;
            }
        }
    } break;
    case 7: {
        if (role == Qt::DisplayRole || role == Qt::EditRole) {
            return m_varibleInfo.Alias;
        }
    } break;
    case 8: {
        if (role == Qt::DisplayRole || role == Qt::EditRole) {
            if (m_itemData.plotType == 0) {
                return QObject::tr("Line");
            } else if (m_itemData.plotType == 1) {
                return QObject::tr("Stairs");
            } else if (m_itemData.plotType == 2) {
                return QObject::tr("Stem");
            } else {
                return QObject::tr("Line");
            }
        } else if (role == Qt::TextAlignmentRole) {
            return Qt::AlignLeft | Qt::AlignVCenter;
        }
        break;
    }
    default:
        break;
    }

    return QVariant();
}

Qt::ItemFlags CurveSettingTreeItem::flags(int col)
{
    Qt::ItemFlags itemFlag = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

    switch (col) {
    case 0:
        if (m_level > 1)
            itemFlag |= Qt::ItemIsUserCheckable;
        break;
    // case 2:
    case 3:
    case 4:
    case 5:
    case 7:
    case 8:
        if (m_level > 1 || m_varibleInfo.Type == Number)
            itemFlag |= Qt::ItemIsEditable;
        break;
    default:
        break;
    }

    return itemFlag;
}

CurveSettingTreeItem *CurveSettingTreeItem::child(int i)
{
    if (i < 0 || i >= m_childItems.size())
        return nullptr;

    return m_childItems[i];
}

CurveSettingTreeItem *CurveSettingTreeItem::parent(void)
{
    return m_parentItem;
}

int CurveSettingTreeItem::childCount(void) const
{
    return m_childItems.size();
}

int CurveSettingTreeItem::pos(void) const
{
    if (!m_parentItem)
        return 0;
    return m_parentItem->m_childItems.indexOf(const_cast<CurveSettingTreeItem *>(this));
}

bool CurveSettingTreeItem::setData(int col, const QVariant &val, int role)
{
    switch (col) {
    case 0: {
        if (role == Qt::CheckStateRole && m_level > 1)
            m_itemData.Checked = val.toInt() == Qt::Checked ? true : false;
    } break;
    case 2: {
        if (role == Qt::EditRole && (m_level > 1 || m_varibleInfo.Type == Number))
            m_itemData.Color = val.value<QColor>();
    } break;
    case 3: {
        if (role == Qt::EditRole && (m_level > 1 || m_varibleInfo.Type == Number))
            m_itemData.Width = val.toInt();
    } break;
    case 4: {
        if (role == Qt::EditRole && (m_level > 1 || m_varibleInfo.Type == Number))
            m_itemData.Style = val.toInt();
    } break;
    case 5: {
        if (role == Qt::EditRole && (m_level > 1 || m_varibleInfo.Type == Number)) {
            m_itemData.Name = val.toString();
        }
    } break;
    case 6: {
        if (role == Qt::EditRole && (m_level > 1 || m_varibleInfo.Type == Number)) {
            m_itemData.Name = val.toString();
        }
    } break;
    case 7: {
        if (role == Qt::EditRole || role == Qt::DisplayRole) {
            if (val.toString().isEmpty()) {
                KMessageBox::warning(QObject::tr("Display name cannot be empty!"), KMessageBox::Ok,
                                     KMessageBox::Ok); // 显示名称不可为空！
                return false;
            }
            m_varibleInfo.Alias = val.toString();
        }
    } break;

    case 8: {
        if (role == Qt::EditRole || role == Qt::DisplayRole) {
            m_itemData.plotType = val.toInt();
        }
        break;
    }

    default:
        break;
    }

    return true;
}

int CurveSettingTreeItem::columnCount(void) const
{
    return 4;
}

bool CurveSettingTreeItem::append(CurveSettingTreeItem *item)
{
    m_childItems.append(item);
    return true;
}

bool CurveSettingTreeItem::remove(int i)
{
    if (i < 0 || i >= m_childItems.size())
        return false;
    delete m_childItems.takeAt(i);
    return true;
}

bool CurveSettingTreeItem::remove(CurveSettingTreeItem *item)
{
    QList<CurveSettingTreeItem *>::iterator iter;
    for (iter = m_childItems.begin(); iter != m_childItems.end(); iter++) {
        if ((*iter) == item)
            break;
    }
    if (iter == m_childItems.end())
        return false;

    delete (*iter);
    m_childItems.erase(iter);
    return true;
}

int CurveSettingTreeItem::level(void) const
{
    return m_level;
}

void CurveSettingTreeItem::setLevel(int n)
{
    m_level = n;
}

void CurveSettingTreeItem::setIcon(QString path)
{
    m_icon.addFile(path);
}

/////////////////////////////////////////////////////////////////////////////////////

CurveSettingTreeModel::CurveSettingTreeModel(int type, QObject *parent)
    : QAbstractItemModel(parent), m_rootItem(nullptr), m_type(type)
{
    if (m_type == 0)
        m_headerList << tr("Variable") //"变量"
                     << tr("Number")   // 索引号
                     << tr("Color")    //"颜色"
                     << tr("Width")    //"线宽"
                     << tr("Style")    //"线形"
                     << tr("Alias");   //"别名"
    else
        m_headerList << tr("Variable")    //"变量名"
                     << tr("Number")      //"索引号"
                     << tr("Color")       //"颜色"
                     << tr("Width")       //"线宽"
                     << tr("Style")       //"线形"
                     << tr("Alias")       //"别名"
                     << tr("Path")        //"路径"
                     << tr("DisplayName") //"显示名称"
                     << tr("Plot Type");  // 绘图类型

    m_rootItem = new CurveSettingTreeItem(m_type);
}

CurveSettingTreeModel::~CurveSettingTreeModel()
{
    if (m_rootItem)
        delete m_rootItem;
}

QVariant CurveSettingTreeModel::headerData(int section, Qt::Orientation orientation,
                                           int role /*= Qt::DisplayRole */) const
{
    if (orientation == Qt::Horizontal) {
        if (role == Qt::DisplayRole) {
            return m_headerList[section];
        }
    }

    return QAbstractItemModel::headerData(section, orientation, role);
}

QVariant CurveSettingTreeModel::data(const QModelIndex &index, int role /* = Qt::DisplayRole */) const
{
    if (!index.isValid())
        return QVariant();

    CurveSettingTreeItem *item = itemFromIndex(index);

    return item->data(index.column(), role);
}

Qt::ItemFlags CurveSettingTreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    CurveSettingTreeItem *item = itemFromIndex(index);

    return item->flags(index.column());
}

QModelIndex CurveSettingTreeModel::index(int row, int column, const QModelIndex &parent /*= QModelIndex() */) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    CurveSettingTreeItem *parentItem = itemFromIndex(parent);
    if (!parentItem)
        return QModelIndex();

    CurveSettingTreeItem *childItem = parentItem->child(row);
    if (!childItem)
        return QModelIndex();

    return createIndex(row, column, childItem);
}

QModelIndex CurveSettingTreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();
    CurveSettingTreeItem *childItem = itemFromIndex(index);
    CurveSettingTreeItem *parentItem = childItem->parent();
    if (!parentItem || parentItem == m_rootItem)
        return QModelIndex();

    return createIndex(parentItem->pos(), 0, parentItem);
}

int CurveSettingTreeModel::rowCount(const QModelIndex &parent /*= QModelIndex()*/) const
{
    return itemFromIndex(parent)->childCount();
}

int CurveSettingTreeModel::columnCount(const QModelIndex &parent /*= QModelIndex()*/) const
{
    return m_headerList.size();
}

bool CurveSettingTreeModel::setData(const QModelIndex &index, const QVariant &value, int role /*= Qt::EditRole*/)
{
    CurveSettingTreeItem *item = itemFromIndex(index);
    if (!item)
        return false;

    bool b = item->setData(index.column(), value, role);
    emit dataChanged(index, index);
    return true;
}

bool CurveSettingTreeModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value,
                                          int role /*= Qt::EditRole*/)
{
    if (orientation != Qt::Horizontal || role != Qt::EditRole)
        return false;
    if (value == headerData(section, orientation, role))
        return false;

    m_headerList.replace(section, value.toString());
    emit headerDataChanged(orientation, section, section);
    return true;
}

// bool CurveSettingTreeModel::insertRows(int row, int count, const QModelIndex &parent /*= QModelIndex()*/)
//{
//	CurveSettingTreeItem* parentItem=itemFromIndex(parent);
//	if(!parentItem)return false;
//
//	beginInsertRows(parent,row,count);
//	parentItem->insert(row,count);
//	endInsertRows();
// }
//
// bool CurveSettingTreeModel::removeRows(int row, int count, const QModelIndex &parent /*= QModelIndex()*/)
//{
//	CurveSettingTreeItem* parentItem=itemFromIndex(parent);
//	if(!parentItem)return false;
//
//	beginInsertRows(parent,row,row+count-1);
//	parentItem->remove(row,count);
//	endInsertRows();
// }

CurveSettingTreeItem *CurveSettingTreeModel::root(void) const
{
    return m_rootItem;
}

CurveSettingTreeItem *CurveSettingTreeModel::itemFromIndex(const QModelIndex &index) const
{
    if (index.isValid()) {
        return static_cast<CurveSettingTreeItem *>(index.internalPointer());
    }

    return m_rootItem;
}

void CurveSettingTreeModel::refresh(void)
{
    beginResetModel();
    endResetModel();
}

// void CurveSettingTreeModel::setPlotVaribleInfoList(QList<PlotVaribleInfo> &varibleInfoList)
//{
//	beginResetModel();
//	foreach(PlotVaribleInfo varIter,varibleInfoList)
//	{
//		CurveSettingTreeItem* parentItem=new CurveSettingTreeItem(m_rootItem);
//		m_rootItem->append(parentItem);
//		parentItem->setVaribleInfo(varIter);
//		foreach(PlotCurveInfo curveIter,varIter.CurveInfoList)
//		{
//			CurveSettingTreeItem* childItem=new CurveSettingTreeItem(parentItem);
//			parentItem->append(childItem);
//			childItem->setItemData(curveIter);
//		}
//	}
//	endResetModel();
// }
//
// QList<PlotVaribleInfo> CurveSettingTreeModel::getPlotVaribleInfoList(void)
//{
//	QList<PlotVaribleInfo> tempList;
//
//	QList<CurveSettingTreeItem*> varibleList=m_rootItem->childs();
//	foreach(CurveSettingTreeItem* varIter,varibleList)
//	{
//		PlotVaribleInfo varibleInfo=varIter->getVaribleInfo();
//		varibleInfo.CurveInfoList.clear();
//
//		QList<CurveSettingTreeItem*> curveList=varIter->childs();
//		foreach(CurveSettingTreeItem* curveIter,curveList)
//		{
//			PlotCurveInfo curveInfo=curveIter->getItemData();
//			varibleInfo.CurveInfoList.push_back(curveInfo);
//		}
//		tempList.push_back(varibleInfo);
//	}
//
//	return tempList;
// }
//
// void CurveSettingTreeModel::addPlotVaribleInfo(PlotVaribleInfo& varibleInfo)
//{
//	CurveSettingTreeItem* parentItem=new CurveSettingTreeItem(m_rootItem);
//	m_rootItem->append(parentItem);
//	parentItem->setVaribleInfo(varibleInfo);
//	foreach(PlotCurveInfo curveIter,varibleInfo.CurveInfoList)
//	{
//		CurveSettingTreeItem* childItem=new CurveSettingTreeItem(parentItem);
//		parentItem->append(childItem);
//		childItem->setItemData(curveIter);
//	}
//	refresh();
// }
//
// void CurveSettingTreeModel::deletePlotVaribleInfo(QModelIndex &index)
//{
//	CurveSettingTreeItem* currentItem=this->itemFromIndex(index);
//	if(currentItem->level()==2)
//	{
//		currentItem=currentItem->parent();
//	}
//	m_rootItem->remove(currentItem);
//	refresh();
// }

void CurveSettingTreeModel::clearPlotVaribleInfoList(void)
{
    delete m_rootItem;
    m_rootItem = new CurveSettingTreeItem(m_type);
    refresh();
}

void CurveSettingTreeModel::setPlotVaribleInfoList(QList<PlotVaribleInfo> &varibleInfoList)
{
    beginResetModel();
    m_rootItem->childs().clear();
    for (PlotVaribleInfo &varIter : varibleInfoList) {
        CurveSettingTreeItem *parentItem = new CurveSettingTreeItem(m_type, m_rootItem);
        m_rootItem->append(parentItem);
        parentItem->setVaribleInfo(varIter);
        if (varIter.Type == Number) {
            if (varIter.CurveInfoList.size() > 0)
                parentItem->setItemData(varIter.CurveInfoList[0]);
        } else {
            for (PlotCurveInfo &curveIter : varIter.CurveInfoList) {
                CurveSettingTreeItem *childItem = new CurveSettingTreeItem(m_type, parentItem);
                parentItem->append(childItem);
                childItem->setItemData(curveIter);
                childItem->setVaribleInfo(varIter);
            }
        }
    }
    endResetModel();
}

QList<PlotVaribleInfo> CurveSettingTreeModel::getPlotVaribleInfoList(void)
{
    QList<PlotVaribleInfo> tempList;

    QList<CurveSettingTreeItem *> varibleList = m_rootItem->childs();
    for (CurveSettingTreeItem *varIter : varibleList) {
        PlotVaribleInfo varibleInfo = varIter->getVaribleInfo();
        varibleInfo.CurveInfoList.clear();

        if (varibleInfo.Type == Number) {
            PlotCurveInfo curveInfo = varIter->getItemData();
            varibleInfo.CurveInfoList.push_back(curveInfo);
        } else {
            QList<CurveSettingTreeItem *> curveList = varIter->childs();
            for (CurveSettingTreeItem *curveIter : curveList) {
                PlotCurveInfo curveInfo = curveIter->getItemData();
                varibleInfo.CurveInfoList.push_back(curveInfo);
            }
        }

        tempList.push_back(varibleInfo);
    }

    return tempList;
}

void CurveSettingTreeModel::addPlotVaribleInfo(PlotVaribleInfo &varibleInfo)
{
    CurveSettingTreeItem *parentItem = new CurveSettingTreeItem(m_type, m_rootItem);
    m_rootItem->append(parentItem);
    parentItem->setVaribleInfo(varibleInfo);
    if (varibleInfo.Type == Number) {
        if (varibleInfo.CurveInfoList.size() > 0)
            parentItem->setItemData(varibleInfo.CurveInfoList[0]);
    } else {
        for (PlotCurveInfo &curveIter : varibleInfo.CurveInfoList) {
            CurveSettingTreeItem *childItem = new CurveSettingTreeItem(m_type, parentItem);
            parentItem->append(childItem);
            childItem->setItemData(curveIter);
            childItem->setVaribleInfo(varibleInfo);
        }
    }

    refresh();
}

void CurveSettingTreeModel::deletePlotVaribleInfo(QModelIndex &index)
{
    CurveSettingTreeItem *currentItem = this->itemFromIndex(index);
    if (currentItem->level() == 2) {
        currentItem = currentItem->parent();
    }
    m_rootItem->remove(currentItem);
    refresh();
}

StyleComboBoxItemDelegate::StyleComboBoxItemDelegate(QObject *parent /*=nullptr*/, bool isIcon)
    : QStyledItemDelegate(parent)
{
    mIsIconCombo = isIcon;
}

StyleComboBoxItemDelegate::~StyleComboBoxItemDelegate() { }

QWidget *StyleComboBoxItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                                                 const QModelIndex &index) const
{
    QComboBox *comboBox = new QComboBox(parent);
    comboBox->setView(new QListView());
    comboBox->setEditable(false);
    comboBox->setIconSize(QSize(100, 16));

    if (mIsIconCombo) {
        comboBox->addItem(QIcon(":/straightLine"), "");
        comboBox->addItem(QIcon(":/dotline1"), "");
        comboBox->addItem(QIcon(":/dotline2"), "");
        comboBox->addItem(QIcon(":/dotline3"), "");
    } else {
        comboBox->addItem(tr("Line"));
        comboBox->addItem(tr("Stairs"));
        comboBox->addItem(tr("Stem"));
    }

    return comboBox;
}

void StyleComboBoxItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QComboBox *comboBox = static_cast<QComboBox *>(editor);

    int cur = index.model()->data(index, Qt::EditRole).toInt();
    if (cur < 0 || cur > 1)
        cur = 0;
    comboBox->setCurrentIndex(cur);
}

void StyleComboBoxItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox *comboBox = static_cast<QComboBox *>(editor);

    model->setData(index, comboBox->currentIndex(), Qt::EditRole);
}

void StyleComboBoxItemDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                                                     const QModelIndex &index) const
{
    editor->setGeometry(option.rect);
}

LineWidthItemDelegate::LineWidthItemDelegate(QObject *parent /*=nullptr*/)
    : QStyledItemDelegate(parent) // QItemDelegate(parent)
{
}

LineWidthItemDelegate::~LineWidthItemDelegate() { }

QWidget *LineWidthItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                                             const QModelIndex &index) const
{
    QSpinBox *editor = new QSpinBox(parent);
    editor->setMinimum(1);
    editor->setMaximum(10);
    return editor;
}

void LineWidthItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    int value = index.model()->data(index, Qt::EditRole).toInt();
    QSpinBox *spinBox = static_cast<QSpinBox *>(editor);
    spinBox->setValue(value);
}

void LineWidthItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QSpinBox *spinBox = static_cast<QSpinBox *>(editor);
    spinBox->interpretText();
    int value = spinBox->value();
    model->setData(index, value, Qt::EditRole);
}

void LineWidthItemDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                                                 const QModelIndex &index) const
{
    editor->setGeometry(option.rect);
}

StyleLineEditDelegate::StyleLineEditDelegate(QObject *parent /*= nullptr*/) : QStyledItemDelegate(parent) { }

StyleLineEditDelegate::~StyleLineEditDelegate() { }

QWidget *StyleLineEditDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                                             const QModelIndex &index) const
{
    QLineEdit *lineEdit = new QLineEdit(parent);
    lineEdit->setMaxLength(32);
    return lineEdit;
}

void StyleLineEditDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QLineEdit *lineEdit = static_cast<QLineEdit *>(editor);
    lineEdit->setText(index.model()->data(index, Qt::EditRole).toString());
}

void StyleLineEditDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QLineEdit *lineEdit = static_cast<QLineEdit *>(editor);

    model->setData(index, lineEdit->text(), Qt::EditRole);
}

void StyleLineEditDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                                                 const QModelIndex &index) const
{
    editor->setGeometry(option.rect);
}
