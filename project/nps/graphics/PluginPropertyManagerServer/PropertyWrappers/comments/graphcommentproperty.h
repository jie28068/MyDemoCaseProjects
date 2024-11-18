#ifndef GRAPHCOMMENTROPERTY_H
#define GRAPHCOMMENTROPERTY_H

#include "CommonWrapper.h"
#include "customtablewidget.h"
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QTextEdit>

#define GRAPH_NAME QObject::tr("Graphic Properties")        // 图形属性
#define GRAPH_EDIT QObject::tr("Graphic Attribute Editing") // 图形属性编辑

// 属性keyword
static const QString KeyWord_GC_LineColor = QObject::tr("Colour");              // 颜色
static const QString KeyWord_GC_LineWidth = QObject::tr("Line Width (px)");     // 线宽(px)
static const QString KeyWord_GC_LineStyle = QObject::tr("Linetype");            // 线型
static const QString KeyWord_GC_LineDBArrow = QObject::tr("Show Front Arrows"); // 显示前端箭头
static const QString KeyWord_GC_LineDEArrow = QObject::tr("Show End Arrows");   // 显示末尾箭头

class SourceProxy;
class GraphCommentProperty : public CommonWrapper
{
    Q_OBJECT
public:
    enum ToolBlockType {
        ToolBlockType_Lines, // 线段、有箭头线段、折线等线段类
        ToolBlockType_Cycle, // 矩形、圆形、多边形等闭合图形
    };
    GraphCommentProperty(QSharedPointer<SourceProxy> sourceProxy, GraphCommentProperty::ToolBlockType blocktype);
    ~GraphCommentProperty();
    void init();

    // IPropertyWrapper
    virtual QString getTitle() override;
    virtual void onDialogExecuteResult(QDialog::DialogCode code) override;
    virtual bool checkValue(QString &errorinfo = QString()) override;

private:
    QList<CustomModelItem> getListData();

private:
    CustomTableWidget *m_pGraphEditWidget;
    ToolBlockType m_blockType;
    QSharedPointer<SourceProxy> m_sourceProxy;
};

#endif // GRAPHCOMMENTROPERTY_H