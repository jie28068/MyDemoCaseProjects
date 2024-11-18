#include "blockproperty.h"
#include "CoreLib/ServerManager.h"
#include "GlobalAssistant.h"
#include "PropertyServerMng.h"
#include <QDebug>

USE_LOGOUT_("BlockProperty")
namespace DRAWINFO {
// 画元件icon的一些颜色及大小
static const QColor TEXTCOLOR_NORMAL = QColor("#252d3f");    // 文字颜色
static const QColor BLOCKCOLOR_PROTOTYPE = TEXTCOLOR_NORMAL; // QColor("#e60012"); // 模块原型最外层的红色
static const QColor BLOCKCOLOR_INSTANCE = TEXTCOLOR_NORMAL;  // QColor("#e0a420");  // 模块实例最外层的绿色

static const int DRAWING_ITEM_WIDTH = 65;
static const int DRAWING_ITEM_HEIGHT = 45;
}

BlockProperty::BlockProperty(QSharedPointer<Kcc::BlockDefinition::Model> model, bool isReadOnly)
    : CommonWrapper(isReadOnly), m_pModel(model)
{
    // Q_ASSERT(model != nullptr);
}

BlockProperty::~BlockProperty() { }

void BlockProperty::init()
{
    CommonWrapper::init();
}

QString BlockProperty::getTitle()
{
    if (PropertyServerMng::getInstance().m_projectManagerServer == nullptr || m_pModel == nullptr) {
        return QString("");
    }
    QString strProjpath = PropertyServerMng::getInstance().m_projectManagerServer->GetCurrentProjectRelativeDir();
    if (m_pModel->getParentModel() != nullptr) {
        QString boardPath;
        PModel parentModel = m_pModel->getParentModel();
        while (parentModel != nullptr) {
            if (boardPath.isEmpty()) {
                boardPath = parentModel->getName();
            } else {
                boardPath = parentModel->getName() + "/" + boardPath;
            }
            parentModel = parentModel->getParentModel();
        }
        return QObject::tr("%1 - %2/%3/%4")
                .arg(m_pModel->getPrototypeName_CHS())
                .arg(strProjpath)
                .arg(boardPath)
                .arg(m_pModel->getName());
    } else {
        return QObject::tr("%1 - %2/%3").arg(m_pModel->getPrototypeName()).arg(strProjpath).arg(m_pModel->getName());
    }
}

void BlockProperty::onDialogExecuteResult(QDialog::DialogCode code)
{
    if (code == QDialog::Accepted) {
        m_bPropertyIsChanged = false;
    }
}

QStringList BlockProperty::getPropertyKeyChangedList()
{
    return propertyKeyChangedList;
}

CommonWrapper::LeftWidgetType BlockProperty::getLeftWidgetType()
{
    if (m_pModel == nullptr) {
        return CommonWrapper::LeftWidgetType_BlockType;
    }

    if (m_pModel->isInstance()) {
        return CommonWrapper::LeftWidgetType_BlockInstance;
    } else {
        return CommonWrapper::LeftWidgetType_BlockType;
    }
}

QPixmap BlockProperty::drawTextIcon(const QString &textstr, bool isInstance, bool bcombinedBlock)
{
    QPixmap pixmapText(DRAWINFO::DRAWING_ITEM_WIDTH, DRAWINFO::DRAWING_ITEM_HEIGHT);
    pixmapText.fill(QColor("#FFFFFF"));
    QPainter painter(&pixmapText);
    QPen pen(DRAWINFO::TEXTCOLOR_NORMAL);
    if (isInstance) {
        pen.setColor(DRAWINFO::BLOCKCOLOR_INSTANCE);
    } else {
        pen.setColor(DRAWINFO::BLOCKCOLOR_PROTOTYPE);
    }
    pen.setWidth(1);
    painter.setPen(pen);
    painter.setFont(QFont("Arial", 8));
    QRectF textRect(4, 4, DRAWINFO::DRAWING_ITEM_WIDTH - 8, DRAWINFO::DRAWING_ITEM_HEIGHT - 8);
    // QRectF rectnoarmal(1, 1, Global::DRAWING_ITEM_WIDTH - 3, Global::DRAWING_ITEM_HEIGHT - 3);
    QFontMetrics fontWidth(painter.font());
    QString elidnote = fontWidth.elidedText(textstr, Qt::ElideRight, 2 * textRect.width() - 1);
    int textW = painter.fontMetrics().width(textstr);
    int textH = painter.fontMetrics().height();
    painter.drawText(textRect, Qt::TextWrapAnywhere | Qt::AlignCenter, elidnote);
    // painter.drawRect(rectnoarmal);
    QRectF rectout(0, 0, DRAWINFO::DRAWING_ITEM_WIDTH - 1, DRAWINFO::DRAWING_ITEM_HEIGHT - 1);
    painter.drawRect(rectout);
    if (bcombinedBlock) {
        QRectF inRect(3, 3, DRAWINFO::DRAWING_ITEM_WIDTH - 7, DRAWINFO::DRAWING_ITEM_HEIGHT - 7);
        painter.drawRect(inRect);
    }
    return pixmapText;
}

QString BlockProperty::transToWFString(const double &value)
{
    // 元件模型所选择的数值离散方法，隐式梯形积分法，值为：1；后向欧拉法，值为0；权重数值积分法，输入框（输入框名为“权重因子”）
    QString wfstr = "";
    if (value == 0) {
        wfstr = QString("后向欧拉法(0)");
    } else if (value == 1) {
        wfstr = QString("隐式梯形积分法(1)");
    } else {
        wfstr = QString("权重数值积分法[权重因子:%1]").arg(QString::number(value));
    }
    return wfstr;
}

QString BlockProperty::transEnumMapValueToString(const QString &datarange, QVariant value)
{
    QStringList enummaplist = datarange.split(",");
    foreach (QString keyvaluestr, enummaplist) {
        QStringList keyvaluelist = keyvaluestr.split("=");
        if (keyvaluelist[1].toInt() == value.toInt()) {
            return keyvaluelist[0];
        }
    }
    return "";
}

QString BlockProperty::getAssociatedBoardsFromIDs(QStringList listIDs)
{
    QString strInfo;
    // QString strBoardName;
    // for (auto str : listIDs) {
    //     auto pDrawBorad = PropertyServerMng::getInstance().m_pDataManagerServer->GetBoradByUUID(str);
    //     if (!pDrawBorad) {
    //         continue;
    //     }

    //     strBoardName = pDrawBorad->getName();

    //     if (str == listIDs.last()) {
    //         strInfo += strBoardName;
    //     } else {
    //         strInfo += strBoardName + "、";
    //     }
    // }
    return strInfo;
}
