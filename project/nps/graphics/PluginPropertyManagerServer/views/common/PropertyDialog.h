#pragma once

#include "CommonWrapper.h"
#include "server/PropertyManagerServer/IPropertyManagerServer.h"
#include <QComboBox>
#include <QDialog>
#include <QGridLayout>
#include <QMap>
#include <QStackedWidget>
#include <QTabWidget>

namespace Ui {
class PropertyDialog;
}

using namespace Kcc::PropertyManager;
class SwitchWidget;
class PropertyDialog : public QWidget
{
    Q_OBJECT
public:
    PropertyDialog(QWidget *parent = nullptr);
    ~PropertyDialog(void);

    void setPropertyWrapper(CommonWrapper *pPropertyWrapper, const QString tabname = "");

    CommonWrapper *getPropertyWrapper();

    void clear();

protected:
    void paintEvent(QPaintEvent *event);

private:
    void refreshPropertyUI(CommonWrapper *pPropertyWrapper, const QString tabname = "");
    void clearWidget();

private slots:
    // void onCurrentItemChanged(QtBrowserItem *pItem);
    void onViewChanged();
    void onTabTreeCurrentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

private:
    Ui::PropertyDialog *ui;
    // 自定义配置页面
    QMap<QString, QWidget *> m_mapCustomTabBrowser;

    CommonWrapper *m_pWrapper;
    // std::vector<int> m_WidthArray;
};
// 自定义可通过combobox切换的画面
class SwitchWidget : public CWidget
{
    Q_OBJECT
public:
    SwitchWidget(QWidget *parent = nullptr);
    ~SwitchWidget();
    void addTabWidget(const QString &tabName, QWidget *pwidget);
    void setCurrentWidget(QWidget *pwidget);
public slots:
    void onCurrentTextChanged(const QString &textstr);

private:
    void InitUI();
    bool isWidgetValid();

private:
    QStringList m_tabOrderList;
    QMap<QString, QWidget *> m_widgetMap;
    QComboBox *m_tabComboBox;
    QStackedWidget *m_stackWidget;
};