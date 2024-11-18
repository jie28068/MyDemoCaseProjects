#include "PropertyTableModel.h"

#include <QColor>
#include <QComboBox>
#include <QLineEdit>
#include <QSize>

#include "CoreLib/ServerManager.h"
#include "GlobalAssistant.h"
#include "PropertyServerMng.h"
#include "commenteditwdiget.h"
// 使用代码服务

static const int NOTHREE = 3;
static const int NOFOUR = 4;
static const int BLOCK_NOTE_MAX_LENGH = 128;
static const QString KeyWord_GC_LineDBArrow = QObject::tr("show front arrows"); // 显示前端箭头
static const QString KeyWord_GC_LineDEArrow = QObject::tr("show end arrows");   // 显示末尾箭头

USE_LOGOUT_("CustomTableItemDelegate")

// tablemodel
PropertyTableModel::PropertyTableModel(QObject *parent) : m_bReadOnly(false), m_useColor(true) { }

PropertyTableModel::~PropertyTableModel() { }

int PropertyTableModel::rowCount(const QModelIndex &parent /*= QModelIndex( ) */) const
{
    return m_data.size();
}

int PropertyTableModel::columnCount(const QModelIndex &parent /*= QModelIndex( ) */) const
{
    return m_listHeader.size();
}

QVariant PropertyTableModel::data(const QModelIndex &index, int role /*= Qt::DisplayRole */) const
{
    if (!index.isValid()) {
        return QVariant();
    }
    int col = index.column();
    int row = index.row();
    int totalcol = columnCount();
    if (col >= totalcol || row >= rowCount()) {
        return QVariant();
    }
    if ((Qt::DisplayRole == role || Qt::EditRole == role || Qt::ToolTipRole == role
         || PropertyTableModel::CustomRole_ColorRole == role)
        && RoleDataDefinition::ControlTypeCheckbox != index.data(NPS::ModelDataTypeRole).toString()) {
        if (ColumnFirst == col) {
            if (totalcol < NOTHREE) {
                return m_data[row].name;
            } else if (totalcol == NOTHREE) {
                return m_data[row].keywords;
            } else if (NOFOUR == totalcol) {
                QStringList keylist = m_data[row].keywords.split("/", QString::SkipEmptyParts, Qt::CaseSensitive);
                keylist.pop_back();
                return keylist.join("/");
            }
        } else if (ColumnSecond == col && NOFOUR == totalcol) {
            QStringList keylist = m_data[row].keywords.split("/", QString::SkipEmptyParts, Qt::CaseSensitive);
            return keylist.back();
        } else if ((ColumnSecond == col && totalcol <= NOTHREE) || (ColumnThird == col && NOFOUR == totalcol)) {
            if (NPS::DataType_LineStyle == m_data[row].valueType) {
                if (Qt::ToolTipRole == role) {
                    return QVariant();
                }
                return m_data[row].value.toInt();
            } else if (NPS::DataType_LineWidth == m_data[row].valueType) {
                if (Qt::ToolTipRole == role) {
                    return QVariant();
                }
                return m_data[row].value.toInt();
            } else if (RoleDataDefinition::ControlTypeColor == m_data[row].valueType) {
                if (PropertyTableModel::CustomRole_ColorRole == role) {
                    return m_data[row].value;
                } else {
                    return QVariant();
                }
            } else {
                if (Qt::DisplayRole == role || Qt::ToolTipRole == role) {
                    if (Qt::ToolTipRole == role
                        && NPS::KEYWORDS_SIMUPARAM_DTM == index.data(NPS::ModelDataKeywordsRole).toString()) {
                        return NPS::WF_DTM_TIPS;
                    }
                    if (m_data[row].otherPropertyMap.contains(KEY_DISPLAYVALUEADD)) {
                        return m_data[row].otherPropertyMap[KEY_DISPLAYVALUEADD].toString()
                                + NPS::RealValueMaptoControlValue(index.data(NPS::ModelDataRealValueRole),
                                                                  index.data(NPS::ModelDataTypeRole).toString(),
                                                                  index.data(NPS::ModelDataRangeRole))
                                          .toString();
                    }
                    return NPS::RealValueMaptoControlValue(index.data(NPS::ModelDataRealValueRole),
                                                           index.data(NPS::ModelDataTypeRole).toString(),
                                                           index.data(NPS::ModelDataRangeRole));
                } else if (Qt::EditRole == role) {
                    return NPS::RealValueMaptoControlValue(index.data(NPS::ModelDataRealValueRole),
                                                           index.data(NPS::ModelDataTypeRole).toString(),
                                                           index.data(NPS::ModelDataRangeRole));
                }

                return QVariant();
            }
        } else if ((ColumnThird == col && NOTHREE == totalcol) || (ColumnFourth == col && NOFOUR == totalcol)) {
            return m_data[row].otherPropertyMap[KEY_DISCRIPTION];
        }
    } else if (NPS::ModelDataRealValueRole == role) {
        if ((ColumnThird == col && NOTHREE == totalcol) || (ColumnFourth == col && NOFOUR == totalcol)) {
            return m_data[row].otherPropertyMap[KEY_DISCRIPTION];
        }
        return m_data[row].value;
    } else if (Qt::CheckStateRole == role
               && RoleDataDefinition::ControlTypeCheckbox == index.data(NPS::ModelDataTypeRole).toString()) {
        return m_data[row].value.toBool() ? Qt::Checked : Qt::Unchecked;
    } else if (Qt::DecorationRole == role
               && RoleDataDefinition::ControlTypeColor == index.data(NPS::ModelDataTypeRole).toString()) {
        return QColor(m_data[row].value.toString());
    } else if (Qt::FontRole == role
               && &RoleDataDefinition::ControlTypeFont == index.data(NPS::ModelDataTypeRole).toString()) {
        return m_data[row].value;
    } else if (NPS::ModelDataTypeRole == role) {
        if ((ColumnFirst == col && totalcol <= NOTHREE)
            || ((ColumnFirst == col || ColumnSecond == col || ColumnFourth == col) && NOFOUR == totalcol)
            || (ColumnThird == col && NOTHREE == totalcol)) {
            return RoleDataDefinition::ControlTypeTextbox;
        }
        return m_data[row].valueType;
    } else if (NPS::ModelDataEditRole == role) {
        if (m_bReadOnly) {
            return false;
        }
        if ((ColumnFirst == col && totalcol <= NOTHREE)
            || ((ColumnFirst == col || ColumnSecond == col) && NOFOUR == totalcol)) {
            return false;
        }
        return m_data[row].bcanEdit;
    } else if (NPS::ModelDataRangeRole == role) {
        return m_data[row].controlLimit;
    } else if (Qt::TextColorRole == role) {
        if (!m_useColor) {
            return QVariant();
        }
        if (m_bReadOnly) {
            return QColor(NPS::Color_NotEditable);
        }
        if (!m_data[row].bcanEdit && m_data[row].valueType != RoleDataDefinition::ControlTypeColor) {
            return QColor(NPS::Color_NotEditable);
        }
    } else if (NPS::ModelDataKeywordsRole == role) {
        return m_data[row].keywords;
    } else if (role == NPS::ModelDataLineStyle && m_data[row].valueType == NPS::DataType_LineStyle) {
        return m_data[row].value;
    } else if (role == NPS::ModelDataLineWidth && m_data[row].valueType == NPS::DataType_LineWidth) {
        return m_data[row].value;
    }
    return QVariant();
}

QVariant PropertyTableModel::headerData(int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole */) const
{
    if (orientation == Qt::Horizontal) {
        if (Qt::DisplayRole == role) {
            Q_ASSERT(section < m_listHeader.count());
            return (m_listHeader[section]);
        }
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}

Qt::ItemFlags PropertyTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }
    if (RoleDataDefinition::ControlTypeCheckbox == index.data(NPS::ModelDataTypeRole).toString()) {
        if (index.data(NPS::ModelDataEditRole).toBool()) {
            return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;
        } else {
            return Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;
        }
    } else if (index.data(NPS::ModelDataEditRole).toBool()
               && RoleDataDefinition::ControlTypeColor != index.data(NPS::ModelDataTypeRole).toString()
               && RoleDataDefinition::ControlTypeFont != index.data(NPS::ModelDataTypeRole).toString()) {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    }
    return QAbstractTableModel::flags(index);
}

bool PropertyTableModel::setData(const QModelIndex &index, const QVariant &value, int role /*= Qt::EditRole */)
{
    if (!index.isValid() || (role != Qt::EditRole && role != Qt::CheckStateRole)) {
        return false;
    }
    QString datatype = index.data(NPS::ModelDataTypeRole).toString();
    QVariant realValue = value;
    if (Qt::CheckStateRole == role) {
        realValue = (Qt::CheckState(value.toInt()) == Qt::Checked);
    }

    if (NPS::isNumberDataType(datatype) && realValue.toString().trimmed().isEmpty()) {
        if (columnCount() == 4) {
            LOGOUT(NPS::LOG_VALUENULLSTRING(
                           index.sibling(index.row(), PropertyTableModel::ColumnFirst).data().toString(),
                           index.sibling(index.row(), PropertyTableModel::ColumnSecond).data().toString()),
                   LOG_WARNING);
        } else {
            LOGOUT(NPS::LOG_VALUENULLSTRING(
                           index.sibling(index.row(), PropertyTableModel::ColumnFirst).data().toString()),
                   LOG_WARNING);
        }
        return false;
    }
    QString keywords = index.data(NPS::ModelDataKeywordsRole).toString();
    if (Qt::CheckStateRole == role) {
        if (index.data(NPS::ModelDataEditRole).toBool()) {
            updateValue(keywords, realValue);
        }
        return true;
    } else if (Qt::EditRole == role) {
        if ((NOFOUR == columnCount() && ColumnFourth == index.column())
            || (NOTHREE == columnCount() && ColumnThird == index.column())) {
            if (realValue.toString().length() > BLOCK_NOTE_MAX_LENGH) {
                //"参数设置/备注最大长度不能超过128
                LOGOUT(QObject::tr("The maximum length  of parameter setting or remarks cannot exceed 128"), LOG_ERROR);
                return false;
            }
            updateValue(keywords, realValue, true);
        } else {
            if (RoleDataDefinition::ControlMathExpression == datatype
                && PropertyServerMng::getInstance().m_pCodeManagerSvr != nullptr) {
                QString errorinfo = "";
                int beginindex = -1;
                int endindex = -1;
                int checkstatus = PropertyServerMng::getInstance().m_pCodeManagerSvr->CheckMathExpr(
                        realValue.toString(), errorinfo, beginindex, endindex);
                if (MATH_EXPR_MATCH_PARTIAL == checkstatus || MATH_EXPR_MATCH_FAILED == checkstatus) {
                    // 部分正确匹配（比如"1+"）
                    LOGOUT(QObject::tr("expression input error, modification failed. please re-enter. reason for "
                                       "failure:%1")
                                   .arg(errorinfo),
                           LOG_WARNING);
                    // 表达式输入错误，修改失败，请重新输入，失败原因：%1
                    return false;
                }
            }
            updateValue(keywords, realValue);
        }
        return true;
    }
    return setData(index, value, role);
}

void PropertyTableModel::setModelData(const QList<CustomModelItem> &listdata, const QStringList &listheader)
{
    beginResetModel();
    m_listHeader.clear();
    m_data.clear();
    m_olddata.clear();
    m_listHeader = listheader;
    m_data = listdata;
    m_olddata = listdata;
    endResetModel();
}

bool PropertyTableModel::checkValueChange()
{
    if (m_data.size() != m_olddata.size())
        return false;
    for (int row = 0; row < m_data.size(); ++row) {
        if (m_data[row].value != m_olddata[row].value
            || m_data[row].otherPropertyMap != m_olddata[row].otherPropertyMap) {
            return true;
        }
    }
    return false;
}

bool PropertyTableModel::checkValueLegitimacy(QString &errorinfo)
{
    bool busename = columnCount() <= 2 ? true : false;
    errorinfo = "";
    foreach (CustomModelItem item, m_data) {
        if (!item.bcanEdit) {
            continue;
        }
        if (RoleDataDefinition::ControlTypeComboBoxDoubleEdit == item.valueType) {
            QStringList rangelist = item.controlLimit.toString().split("|");
            if (rangelist.size() == 2) {
                if (!NPS::checkValueAvailable(busename ? item.name : item.keywords,
                                              RoleDataDefinition::ControlTypeDouble, rangelist[1], item.value,
                                              errorinfo)) {
                    return false;
                }
            }
        } else {
            if (!NPS::checkValueAvailable(busename ? item.name : item.keywords, item.valueType, item.controlLimit,
                                          item.value, errorinfo)) {
                return false;
            }
        }
    }
    return true;
}

QList<CustomModelItem> PropertyTableModel::getNewListData()
{
    return m_data;
}

QList<CustomModelItem> PropertyTableModel::getOldListData()
{
    return m_olddata;
}

CustomModelItem PropertyTableModel::getModelItem(const QString &keywords, bool bnew)
{
    QList<CustomModelItem> listdata;
    if (bnew) {
        listdata = m_data;
    } else {
        listdata = m_olddata;
    }
    foreach (CustomModelItem item, listdata) {
        if (keywords == item.keywords) {
            return item;
        }
    }
    return CustomModelItem();
}

void PropertyTableModel::updateValue(const QString &keywords, const QVariant &value, bool bdescription,
                                     bool bsendSignal)
{
    int size = m_data.size();
    for (int row = 0; row < size; ++row) {
        if (m_data[row].keywords == keywords) {
            CustomModelItem olditem = m_data[row];
            CustomModelItem newitem = m_data[row];
            if (bdescription) {
                newitem.otherPropertyMap[KEY_DISCRIPTION] = value;
            } else {
                if (NPS::DataType_LineStyle == newitem.valueType) {
                    newitem.value = value.toString();
                } else if (NPS::DataType_LineWidth == newitem.valueType) {
                    newitem.value = value;
                } else if (RoleDataDefinition::ControlMathExpression == newitem.valueType) {
                    QVariant calvalue;
                    QString errorinfo = "";
                    if (PropertyServerMng::getInstance().m_pCodeManagerSvr != nullptr
                        && !PropertyServerMng::getInstance().m_pCodeManagerSvr->CalcMathExpr(value.toString(), calvalue,
                                                                                             errorinfo)) {
                        LOGOUT(QObject::tr("expression calculation failed, reason for failure:%1").arg(errorinfo),
                               LOG_WARNING); // 表达式计算失败，失败原因：%1
                        break;
                    }
                    newitem.otherPropertyMap[KEY_PARAMEXPVALUE] = NPS::doubleToString15(calvalue.toDouble());
                    newitem.value = value;
                } else {
                    newitem.value = value;
                }
            }
            m_data[row] = newitem;
            emit dataChanged(this->index(row, 0), this->index(row, columnCount() - 1));
            if (bsendSignal) {
                emit modelDataItemChanged(olditem, newitem);
            }
            break;
        }
    }
}

void PropertyTableModel::setModelDataReadOnly(bool bReadOnly)
{
    beginResetModel();
    m_bReadOnly = bReadOnly;
    endResetModel();
}

void PropertyTableModel::setModelDataColorUse(bool use)
{
    beginResetModel();
    m_useColor = use;
    endResetModel();
}

void PropertyTableModel::updateValue(const QString &keywords, const CustomModelItem &item, bool bsendSignal)
{
    int size = m_data.size();
    for (int row = 0; row < size; ++row) {
        if (m_data[row].keywords == keywords) {
            CustomModelItem olditem = m_data[row];
            m_data[row] = item;
            emit dataChanged(this->index(row, 0), this->index(row, columnCount() - 1));
            if (bsendSignal) {
                emit modelDataItemChanged(olditem, item);
            }
            break;
        }
    }
}

// 画板表格代理
CustomTableItemDelegate::CustomTableItemDelegate(QObject *parent) { }

CustomTableItemDelegate::~CustomTableItemDelegate() { }

QWidget *CustomTableItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                                               const QModelIndex &index) const
{
    Q_UNUSED(option);
    if (!index.isValid() || !index.data(NPS::ModelDataEditRole).toBool()) {
        return nullptr;
    }

    // 返回当前值所对应的类型字段值
    QString type = index.data(NPS::ModelDataTypeRole).toString();
    if (NPS::DataType_LineStyle == type) {
        // 新建线型选择box
        customLineStyleCombobox *pcLineStyleComboBox = new customLineStyleCombobox(parent);
        QListmodel *mListModel = new QListmodel();
        comboboxDelegate *m_delegate = new comboboxDelegate(pcLineStyleComboBox, 0);
        pcLineStyleComboBox->setModel(mListModel);        // 设置模型
        pcLineStyleComboBox->setItemDelegate(m_delegate); // 设置代理
        // m_pcLineStyleComboBox->setIndex(0);
        QList<UserInfo> InfoList;
        int num = 0;

        // 添加模型数据
        foreach (int style, pcLineStyleComboBox->getStyleList()) {
            UserInfo info;
            num++;
            info.number = QString::number(num);
            info.lineValue = style;
            InfoList.append(info);
        }
        mListModel->setModelData(InfoList);
        return pcLineStyleComboBox;
    } else if (NPS::DataType_LineWidth == type) {
        // 新建线宽选择box
        customLineWidthCombobox *m_pcLineWidthComboBox = new customLineWidthCombobox(parent);
        QListmodel *mListModel = new QListmodel();
        comboboxDelegate *m_delegate = new comboboxDelegate(m_pcLineWidthComboBox, 1);
        m_pcLineWidthComboBox->setModel(mListModel);        // 设置模型
        m_pcLineWidthComboBox->setItemDelegate(m_delegate); // 设置代理
        QList<UserInfo> InfoList;
        int num = 0;
        // 添加模型数据
        foreach (int width, m_pcLineWidthComboBox->getWidthList()) {
            UserInfo info;
            num++;
            info.number = QString::number(num);
            info.lineValue = width;
            InfoList.append(info);
        }
        mListModel->setModelData(InfoList);
        return m_pcLineWidthComboBox;
    } else {
        return NPS::createWidget(index.data(NPS::ModelDataTypeRole).toString(), index.data(NPS::ModelDataRangeRole),
                                 parent);
    }
}

void CustomTableItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QString type = index.data(NPS::ModelDataTypeRole).toString();
    if (NPS::DataType_LineStyle == type) {
        int num = index.data(Qt::EditRole).toInt();
        customLineStyleCombobox *combox = static_cast<customLineStyleCombobox *>(editor);
        if (combox != nullptr) {
            combox->setCurrentIndex(num - 1);
        }
    } else if (NPS::DataType_LineWidth == type) {
        int num = index.data(Qt::EditRole).toInt();
        customLineWidthCombobox *combox = static_cast<customLineWidthCombobox *>(editor);
        if (combox != nullptr) {
            combox->setCurrentIndex(num - 1);
        }
    } else {
        NPS::setWidgetControlData(editor, index.data(NPS::ModelDataRealValueRole), type,
                                  index.data(NPS::ModelDataRangeRole));
    }
}

void CustomTableItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if (model == nullptr || editor == nullptr || !index.isValid()) {
        return;
    }
    QString type = index.data(NPS::ModelDataTypeRole).toString();
    if (NPS::DataType_LineStyle == type) {
        customLineStyleCombobox *combox = static_cast<customLineStyleCombobox *>(editor);
        if (combox != nullptr) {
            model->setData(index, combox->currentIndex() + 1, Qt::EditRole);
        }
    } else if (NPS::DataType_LineWidth == type) {
        customLineWidthCombobox *combox = static_cast<customLineWidthCombobox *>(editor);
        if (combox != nullptr) {
            model->setData(index, combox->currentIndex() + 1, Qt::EditRole);
        }
    } else {
        model->setData(index,
                       NPS::getWidgetDataMaptoRealData(editor, index.data(NPS::ModelDataTypeRole).toString(),
                                                       index.data(NPS::ModelDataRangeRole)),
                       Qt::EditRole);
    }
}

void CustomTableItemDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                                                   const QModelIndex &index) const
{
    Q_UNUSED(index);
    editor->setGeometry(option.rect);
}

void CustomTableItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                                    const QModelIndex &index) const
{
    QStyledItemDelegate::paint(painter, option, index);
    if (!index.isValid()) {
        return;
    }
    // 返回当前值所对应的类型字段值
    QString typestr = index.data(NPS::ModelDataTypeRole).toString();
    if (NPS::DataType_LineStyle == typestr || NPS::DataType_LineWidth == typestr) {

        QString num;
        if (NPS::DataType_LineStyle == typestr)
            num = index.data(NPS::ModelDataLineStyle).toString();
        else if (NPS::DataType_LineWidth == typestr)
            num = index.data(NPS::ModelDataLineWidth).toString();

        painter->save();
        QRectF rect;
        rect.setX(option.rect.x());
        rect.setY(option.rect.y());
        rect.setWidth(option.rect.width());
        rect.setHeight(option.rect.height());

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

        painter->setPen(QPen(Qt::white));
        painter->setBrush(QColor(255, 255, 255));
        painter->drawPath(path);

        rect.setX(option.rect.x());
        rect.setY(option.rect.y());
        rect.setWidth(option.rect.width());
        rect.setHeight(26);

        int textRectWidth = 10, textRectHeigh = 15;
        // 画编号
        QRect textRect =
                QRect(rect.x() + 10, rect.y() + rect.height() / 2 - textRectHeigh / 2, textRectWidth, textRectHeigh);
        // painter->setPen(QPen(QColor(0, 0, 0)));
        // painter->setFont(QFont("微软雅黑", 9, QFont::Normal));
        // painter->drawText(textRect, Qt::AlignLeft, QString("%1").arg(num));

        // 画直线
        QPen pen;
        pen.setColor(Qt::black);

        if (NPS::DataType_LineStyle == typestr) {
            pen.setWidthF(3);
            pen.setStyle((Qt::PenStyle)num.toInt());
        }
        if (NPS::DataType_LineWidth == typestr) {
            pen.setWidthF(num.toInt());
            pen.setStyle(Qt::SolidLine);
        }
        painter->setPen(pen);
        painter->drawLine(QLine(textRect.x() + textRectWidth + 10, textRect.y() + textRectHeigh / 2,
                                textRect.x() + textRectWidth + 10 + rect.width() - 70,
                                textRect.y() + textRectHeigh / 2));
        painter->restore();
    }
    if (index.data(NPS::ModelDataEditRole).toBool() && NPS::TCOMBOBOX_CONTROLTYPES.contains(typestr)) {
        // 画个箭头
        painter->save();
        QRectF rect;
        QPointF p1(option.rect.x() + option.rect.width() - (NPS::ARROWLEFTDIS + NPS::ARROWWIDTH),
                   option.rect.y() + (option.rect.height() - NPS::ARROWHEIGHT) / 2);
        QPointF p2(option.rect.x() + option.rect.width() - (NPS::ARROWLEFTDIS + NPS::ARROWWIDTH / 2),
                   option.rect.y() + (option.rect.height() - NPS::ARROWHEIGHT) / 2 + NPS::ARROWHEIGHT);
        QPointF p3(option.rect.x() + option.rect.width() - NPS::ARROWLEFTDIS,
                   option.rect.y() + (option.rect.height() - NPS::ARROWHEIGHT) / 2);
        QPen pen;
        pen.setColor(QColor("#554d4b")); // #574f4d 		pen.setWidthF(1.5);
        pen.setStyle(Qt::SolidLine);
        painter->setPen(pen);
        painter->drawLine(p1, p2);
        painter->drawLine(p2, p3);
        painter->restore();
    }
}
