#ifndef PLANITEMWIDGET_H
#define PLANITEMWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QVariantMap>

class PlanItemWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PlanItemWidget(QVariantMap map,QWidget *parent = nullptr);
    ~PlanItemWidget();

    const QVariantMap &getDatamap() const;

signals:

private:
    QVariantMap datamap;
    QLabel* nameLb=NULL;
    QLabel* createTimeLb=NULL;
    QLabel* creatRoleLb=NULL;
};

#endif // PLANITEMWIDGET_H
