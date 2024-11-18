#include "graphcommentproperty.h"
#include "GlobalAssistant.h"
#include "GraphicsModelingKernel/GraphicsKernelDefinition.h"
#include "GraphicsModelingKernel/SourceProxy.h"
#include "KLModelDefinitionCore/PublicDefine.h"
#include <QDebug>
#include <QLabel>
using namespace Kcc::BlockDefinition;

GraphCommentProperty::GraphCommentProperty(QSharedPointer<SourceProxy> sourceProxy,
                                           GraphCommentProperty::ToolBlockType blocktype)
    : CommonWrapper(), m_pGraphEditWidget(nullptr), m_blockType(blocktype), m_sourceProxy(sourceProxy)
{
}

GraphCommentProperty::~GraphCommentProperty() { }

void GraphCommentProperty::init()
{
    CommonWrapper::init();
    m_pGraphEditWidget = new CustomTableWidget();
    m_pGraphEditWidget->setListData(getListData());
    addWidget(m_pGraphEditWidget, GRAPH_EDIT, false);
}

QString GraphCommentProperty::getTitle()
{
    return QString(GRAPH_NAME);
}

void GraphCommentProperty::onDialogExecuteResult(QDialog::DialogCode code)
{
    if (code == QDialog::Accepted) {
        bool bvchanged = false;
        if (m_pGraphEditWidget != nullptr && m_sourceProxy != nullptr) {
            QList<CustomModelItem> listdata = m_pGraphEditWidget->getNewListData();
            foreach (CustomModelItem item, listdata) {
                if (KeyWord_GC_LineColor == item.keywords) {
                    QString oldcolorstr = m_sourceProxy->getAnnotationMap()[Annotation::segmentColor].toString();
                    if (item.value.toString() != oldcolorstr) {
                        bvchanged = true;
                        m_sourceProxy->setAnnotationProperty(Annotation::segmentColor, QColor(item.value.toString()));
                    }
                }
                if (KeyWord_GC_LineWidth == item.keywords) {
                    double oldwidth = m_sourceProxy->getAnnotationMap()[Annotation::segmentWidth].toDouble();
                    if (item.value.toDouble() != oldwidth) {
                        bvchanged = true;
                        m_sourceProxy->setAnnotationProperty(Annotation::segmentWidth, item.value.toDouble());
                    }
                }
                if (KeyWord_GC_LineStyle == item.keywords) {
                    int oldStyle = m_sourceProxy->getAnnotationMap()[Annotation::segmentStyle].toInt();
                    if (item.value.toInt() != oldStyle) {
                        bvchanged = true;
                        m_sourceProxy->setAnnotationProperty(Annotation::segmentStyle, item.value.toInt());
                    }
                }
                if (KeyWord_GC_LineDBArrow == item.keywords) {
                    bool oldvalue = m_sourceProxy->getAnnotationMap()[Annotation::isShowHeadArrow].toBool();
                    if (item.value.toBool() != oldvalue) {
                        bvchanged = true;
                        m_sourceProxy->setAnnotationProperty(Annotation::isShowHeadArrow, item.value.toBool());
                    }
                }
                if (KeyWord_GC_LineDEArrow == item.keywords) {
                    bool oldvalue = m_sourceProxy->getAnnotationMap()[Annotation::isShowArrow].toBool();
                    if (item.value.toBool() != oldvalue) {
                        bvchanged = true;
                        m_sourceProxy->setAnnotationProperty(Annotation::isShowArrow, item.value.toBool());
                    }
                }
            }
        }

        if (bvchanged) {
            m_bPropertyIsChanged = true;
        }
    }
}

bool GraphCommentProperty::checkValue(QString &errorinfo /*= QString( ) */)
{
    if (m_pGraphEditWidget != nullptr && !m_pGraphEditWidget->checkLegitimacy(errorinfo)) {
        return false;
    }
    errorinfo = "";
    return true;
}

QList<CustomModelItem> GraphCommentProperty::getListData()
{
    QList<CustomModelItem> listdata;
    switch (m_blockType) {
    case GraphCommentProperty::ToolBlockType_Lines: {
        QString lineColor = m_sourceProxy->getAnnotationMap()[Annotation::segmentColor].toString();
        listdata.append(CustomModelItem(KeyWord_GC_LineColor, KeyWord_GC_LineColor, lineColor,
                                        RoleDataDefinition::ControlTypeColor, true));
        double linewidth = m_sourceProxy->getAnnotationMap()[Annotation::segmentWidth].toDouble();
        listdata.append(CustomModelItem(KeyWord_GC_LineWidth, KeyWord_GC_LineWidth, linewidth, NPS::DataType_LineWidth,
                                        true, "1,2,3,4,5,6,7,8,9"));
        int lineStyle = m_sourceProxy->getAnnotationMap()[Annotation::segmentStyle].toInt();
        listdata.append(CustomModelItem(KeyWord_GC_LineStyle, KeyWord_GC_LineStyle, lineStyle, NPS::DataType_LineStyle,
                                        true, "1,2,3,4,5"));
        //(Qt::SolidLine)(实线);(Qt::DashLine)(虚线);(Qt::DotLine)(点线);
        //(Qt::DashDotLine)(点划线);(Qt::DashDotDotLine)(双点划线);
        bool displayba = m_sourceProxy->getAnnotationMap()[Annotation::isShowHeadArrow].toBool();
        listdata.append(CustomModelItem(KeyWord_GC_LineDBArrow, KeyWord_GC_LineDBArrow, displayba,
                                        RoleDataDefinition::ControlTypeCheckbox, true));
        bool displayea = m_sourceProxy->getAnnotationMap()[Annotation::isShowArrow].toBool();
        listdata.append(CustomModelItem(KeyWord_GC_LineDEArrow, KeyWord_GC_LineDEArrow, displayea,
                                        RoleDataDefinition::ControlTypeCheckbox, true));
    } break;
    case GraphCommentProperty::ToolBlockType_Cycle: {
        QString cycleColor = m_sourceProxy->getAnnotationMap()[Annotation::segmentColor].toString();
        listdata.append(CustomModelItem(KeyWord_GC_LineColor, KeyWord_GC_LineColor, cycleColor,
                                        RoleDataDefinition::ControlTypeColor, true));
        double cyclewidth = m_sourceProxy->getAnnotationMap()[Annotation::segmentWidth].toDouble();
        listdata.append(CustomModelItem(KeyWord_GC_LineWidth, KeyWord_GC_LineWidth, cyclewidth, NPS::DataType_LineWidth,
                                        true, "1,2,3,4,5,6,7,8,9"));
        int cycleStyle = m_sourceProxy->getAnnotationMap()[Annotation::segmentStyle].toInt();
        listdata.append(CustomModelItem(KeyWord_GC_LineStyle, KeyWord_GC_LineStyle, cycleStyle, NPS::DataType_LineStyle,
                                        true, "1,2,3,4,5"));
    } break;
    default:
        break;
    }
    return listdata;
}
