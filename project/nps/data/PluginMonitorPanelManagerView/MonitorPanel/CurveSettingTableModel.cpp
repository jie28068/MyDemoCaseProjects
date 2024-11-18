
#include "CurveSettingTableModel.h"

CurveSettingTableModel::CurveSettingTableModel(QObject *parent) : QAbstractTableModel(parent)
{
    // m_headerList<<"名称"<<"全路径"<<"别名"<<"参数类型"<<"线条颜色"<<"线宽";
    m_headerList << tr("order number")            // 序号
                 << tr("Variable row and column") // 对应变量行列
                 << tr("Curve Color")             // 曲线颜色
                 << tr("Curve Width");            // 曲线线宽
}

CurveSettingTableModel::~CurveSettingTableModel() { }

int CurveSettingTableModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_curveList.size();
}

int CurveSettingTableModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_headerList.size();
}

// QVariant CurveSettingTableModel::data(const QModelIndex &index, int role) const
//{
//
//	if(!index.isValid())return QVariant();
//
//	switch(index.column())
//	{
//	case 0:
//		{
//			if(role==Qt::DisplayRole /*|| role==Qt::EditRole*/)
//				return m_curveList[index.row()].Name;
//			else if(role==Qt::CheckStateRole)
//				return m_curveList[index.row()].Checked ? Qt::Checked : Qt::Unchecked;
//		}break;
//	case 1:
//		{
//			if(role==Qt::DisplayRole /*|| role==Qt::EditRole*/)
//				return m_curveList[index.row()].Path;
//		}break;
//	case 2:
//		{
//			if(role==Qt::DisplayRole /*|| role==Qt::EditRole*/)
//				return m_curveList[index.row()].Alias;
//		}break;
//	case 3:
//		{
//			if(role==Qt::DisplayRole /*|| role==Qt::EditRole*/)
//				return m_curveList[index.row()].Type;
//		}break;
//	case 4:
//		{
//			if(role==Qt::BackgroundColorRole)
//			{
//				return m_curveList[index.row()].Color;
//			}
//		}break;
//	case 5:
//		{
//			if(role==Qt::DisplayRole || role==Qt::EditRole)
//				return m_curveList[index.row()].Width;
//		}
//	default:
//		break;
//	}
//
//	return QVariant();
// }

QVariant CurveSettingTableModel::data(const QModelIndex &index, int role) const
{

    if (!index.isValid())
        return QVariant();

    switch (index.column()) {
    case 0: {
        if (role == Qt::DisplayRole /*|| role==Qt::EditRole*/)
            return m_curveList[index.row()].Index;
        else if (role == Qt::CheckStateRole)
            return m_curveList[index.row()].Checked ? Qt::Checked : Qt::Unchecked;
    } break;
    case 1: {
        if (role == Qt::DisplayRole /*|| role==Qt::EditRole*/)
            return QString("[%1,%2]").arg(m_curveList[index.row()].Row).arg(m_curveList[index.row()].Col);
    } break;
    case 2: {
        if (role == Qt::BackgroundColorRole) {
            return m_curveList[index.row()].Color;
        }
    } break;
    case 3: {
        if (role == Qt::DisplayRole || role == Qt::EditRole)
            return m_curveList[index.row()].Width;
    }
    default:
        break;
    }

    return QVariant();
}

bool CurveSettingTableModel::setData(const QModelIndex &index, const QVariant &value, int role /*= Qt::EditRole */)
{
    if (!index.isValid())
        return false;

    switch (index.column()) {
    case 0: {
        if (role == Qt::CheckStateRole)
            m_curveList[index.row()].Checked = value.toInt() == Qt::Checked ? true : false;
    } break;
    case 2: {
        if (role == Qt::EditRole)
            m_curveList[index.row()].Color = value.value<QColor>();
    } break;
    case 3: {
        if (role == Qt::EditRole)
            m_curveList[index.row()].Width = value.toInt();
    }
    default:
        break;
    }

    emit dataChanged(index, index);

    return true;
}

Qt::ItemFlags CurveSettingTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    Qt::ItemFlags itemFlag = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

    switch (index.column()) {
    case 0:
        itemFlag |= Qt::ItemIsUserCheckable;
        break;
    case 2:
    case 3:
        itemFlag |= Qt::ItemIsEditable;
        break;
    default:
        break;
    }
    return itemFlag;
}

QVariant CurveSettingTableModel::headerData(int section, Qt::Orientation orientation,
                                            int role /*= Qt::DisplayRole */) const
{
    if (section < m_headerList.size() && role == Qt::DisplayRole && orientation == Qt::Horizontal)
        return m_headerList.at(section);
    return QVariant();
}

void CurveSettingTableModel::setCurveInfo(QList<PlotCurveInfo> &info)
{
    beginResetModel();
    m_curveList = info;
    endResetModel();
}

QList<PlotCurveInfo> CurveSettingTableModel::getCurveInfo(void)
{
    return m_curveList;
}
//
// void CurveSettingTableModel::addCurveInfo(CurveInfo &info)
//{
//	beginInsertRows(QModelIndex(),m_curveList.size(),m_curveList.size());
//	m_curveList.append(info);
//	endInsertRows();
//}

// void CurveSettingTableModel::deleteCurveInfo(int nRow)
//{
//	beginRemoveRows(QModelIndex(),nRow,nRow);
//	m_curveList.removeAt(nRow);
//	endRemoveRows();
// }
//
// void CurveSettingTableModel::clearCurveInfo(void)
//{
//	beginResetModel();
//	m_curveList.clear();
//	endResetModel();
// }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// ColorComboBoxItemDelegate::ColorComboBoxItemDelegate(QObject *parent)
//	: QStyledItemDelegate(parent)
//{
//
//}
//
// ColorComboBoxItemDelegate::~ColorComboBoxItemDelegate()
//{
//
//}
//
// QWidget* ColorComboBoxItemDelegate::createEditor(QWidget * parent, const QStyleOptionViewItem &option, const
// QModelIndex &index) const
//{
//	QComboBox *comboBox=new QComboBox(parent);
//	comboBox->setEditable(false);
//	int k=0;
//	comboBox->setIconSize(QSize(80,24));
//	//foreach(QString iter,QColor::colorNames())
//	//{
//	//	QPixmap pix(80,24);
//	//	QColor clr(iter);
//	//	pix.fill(clr);
//	//	comboBox->addItem(QIcon(pix),QString());
//	//	comboBox->setItemData(k,clr);
//	//	k++;
//	//}
//
//	for(int i=0;i<CURVECOLORS_COUNT;i++)
//	{
//		QPixmap pix(80,24);
//		QColor clr(CURVECOLORS[i]);
//		pix.fill(clr);
//		comboBox->addItem(QIcon(pix),QString());
//		comboBox->setItemData(k,clr);
//		k++;
//	}
//
//	return comboBox;
//}
//
// void ColorComboBoxItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
//{
//	QComboBox *comboBox = static_cast<QComboBox*>(editor);
//	QColor clr=index.model()->data(index,Qt::BackgroundColorRole).value<QColor>();
//	comboBox->setCurrentIndex(comboBox->findData(clr));
//}
//
// void ColorComboBoxItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index)
// const
//{
//	QComboBox *comboBox = static_cast<QComboBox*>(editor);
//
//	model->setData(index,comboBox->itemData(comboBox->currentIndex()),Qt::EditRole);
//}
//
// void ColorComboBoxItemDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const
// QModelIndex &index) const
//{
//	editor->setGeometry(option.rect);
//}
