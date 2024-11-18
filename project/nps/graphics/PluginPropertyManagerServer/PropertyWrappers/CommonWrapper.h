#ifndef COMMONWRAPPER_H
#define COMMONWRAPPER_H

#pragma once
#include "TypeItemView.h"
#include <QDialog>
#include <QObject>
#include <QPixmap>
#include <QStringList>

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif
// tab详细信息
struct TabDetailItem {
    TabDetailItem(const QString &_treetabName) : treetabName(_treetabName)
    {
        tabWidgetNameList.clear();
        nameMapWidgetMap.clear();
    };
    QString treetabName;                       // tree所属tab名
    QStringList tabWidgetNameList;             // tabwidget的名称列表
    QMap<QString, QWidget *> nameMapWidgetMap; // 名称映射widget
};
class CommonWrapper : public QObject
{
    Q_OBJECT
public:
    enum LeftWidgetType { LeftWidgetType_BlockType, LeftWidgetType_BlockInstance, LeftWidgetType_Other };
    CommonWrapper(bool readOnly = false);
    virtual ~CommonWrapper();
    // CommonWrapper
    virtual void init();
    virtual QString getTitle();
    virtual QStringList getTreeTabNameList();                             // tree的tabitem列表
    virtual QStringList getTabWidgetNameList(const QString &treeTabName); // 获取tabwidget的列表
    virtual QWidget *getCustomPropertyWidget(const QString &treeTabName, const QString &tabWidgetName);
    virtual void setPropertyEditableStatus(bool bReadOnly);
    virtual void onDialogExecuteResult(QDialog::DialogCode code);
    virtual bool checkValue(QString &errorinfo = QString());
    virtual bool isPropertyChanged();
    virtual CommonWrapper::LeftWidgetType getLeftWidgetType();
    virtual QPixmap getBlockPixmap();
    virtual QString getHelpUrl();
signals:
    void viewChanged();
    void closeWrapper();

public:
    void addWidget(QWidget *widget, const QString &treeTabName, bool pushback = true,
                   const QString &tabWidgetName = "");

protected:
    QList<TabDetailItem> m_TabItemList;
    bool m_bPropertyIsChanged;
    bool m_readOnly;
};

#endif