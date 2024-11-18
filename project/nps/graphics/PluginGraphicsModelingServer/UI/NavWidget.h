#ifndef NAVWIDGET_H
#define NAVWIDGET_H

#include "Global_GMS.h"
#include <QListWidget>

class CanvasWidget;

/// @brief
class NavWidget : public QListWidget
{
public:
    enum { kModelUUIDRole = Qt::UserRole + 1 };

    NavWidget(QWidget *parent = 0);
    ~NavWidget();

    void setTopModel(PModel pTopModel);
    void setCurModel(PModel pModel);

    void changeFirstItemName(const QString &modelName);
    void addItemList(const QList<PModel> &modelList);

private:
    void addModelItem(PModel pModel);

private slots:
    void onItemClicked(QListWidgetItem *item);

private:
    PModel m_topModel;
    PModel m_curModel;
    CanvasWidget *m_canvasWidget;
};

#endif