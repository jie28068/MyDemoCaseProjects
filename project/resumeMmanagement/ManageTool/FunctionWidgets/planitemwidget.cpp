#include "planitemwidget.h"
#include <QBoxLayout>
#include <QDebug>

PlanItemWidget::PlanItemWidget(QVariantMap map, QWidget *parent) : QWidget(parent)
{
    setMinimumSize(100,60);
    datamap = map;
    nameLb = new QLabel(datamap.value("name").toString(),this);
    nameLb->setFont(QFont("微软雅黑",12,QFont::Bold));
    createTimeLb = new QLabel(datamap.value("create_time").toString(),this);
    createTimeLb->setFont(QFont("微软雅黑",8,QFont::Light));
    creatRoleLb = new QLabel("创建人:"+datamap.value("role_create").toString(),this);
    creatRoleLb->setFont(QFont("微软雅黑",8,QFont::Light));
    qDebug()<<nameLb->text()<<createTimeLb->text()<<creatRoleLb->text();
    QVBoxLayout* vertical = new QVBoxLayout(this);
    QHBoxLayout* horizontal = new QHBoxLayout();
    horizontal->addWidget(creatRoleLb);
    horizontal->addWidget(createTimeLb);

    vertical->addWidget(nameLb);
    vertical->addLayout(horizontal);
//    setStyleSheet(QString("#onameLb{font: 25 10pt \"微软雅黑\";}"
//                          "#ocreateTimeLb{font: 25 6pt \"微软雅黑 Light\";}"
//                          "#ocreatRoleLb{font: 25 6pt \"微软雅黑 Light\";}"));
}

PlanItemWidget::~PlanItemWidget()
{

}

const QVariantMap &PlanItemWidget::getDatamap() const
{
    return datamap;
}
