#ifndef MYTOOLBOX_H
#define MYTOOLBOX_H

#include "ComponentListModel.h"
#include "ContextMenu.h"
#include "GlobalDefinition.h"
#include "ui_MyToolBox.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

#define COMPOENTVIEW_QSS                                                                                               \
    (QFileInfo(QCoreApplication::applicationDirPath()).canonicalFilePath()                                             \
     + "/configs/Default/resource/qss/default/plugins/PluginComponentView.qss")

class MyListView;
class ToolPage;
class PageButton;

// 列表视图，多个分组ToolPage
class MyToolBox : public QWidget
{
    Q_OBJECT
public:
    explicit MyToolBox(QWidget *parent = nullptr);
    ~MyToolBox() { }

    ToolPage *addGroup(Global::Category category, const QString &strGroupName,const bool &isExpand);
    void removeAllGroup();
    void removeGroup(const QString &strGroupName);

    void setDragEnable(bool bEnable);

    void invalidatePage(const QString &strGroupName = QString());

    void expandAll();

    void collapseAll();

    void setSelectedPtr();

signals:
    void leftMouseButtonDoubleClicked(const QString &strTypeName);

private slots:
    void onListViewClicked(const QModelIndex &index);
    void onFilter(const QString &strFilter);
    void onScrollWidgetSizeChanged(int height);

protected:
    void paintEvent(QPaintEvent *e);

    virtual void resizeEvent(QResizeEvent *e);

private:
    Ui::MyToolBox ui;
    QVBoxLayout *m_pContentVBoxLayout;

    MyListView *m_pLastSelected;
    QAbstractItemModel *m_pModel;
    QMap<QString, ToolPage *> m_mapGroupPages;
};

// button + listview
class ToolPage : public QWidget
{
    Q_OBJECT
    friend class MyToolBox;

public:
    ToolPage(Global::Category category, const QString &m_strGroupName, MyToolBox *parent = nullptr,const bool &isExpand =true);

    void setModel(QAbstractItemModel *pModel);

    void setData(int nRole, const QVariant &value);

signals:
    void isExpandChanged(const QString &groupName,const bool  &isExpand);

public slots:
    void expand();
    void collapse();
    void expandChanged();

protected:
    void contextMenuEvent(QContextMenuEvent *e) override;
    void dragEnterEvent(QDragEnterEvent *e) override;
    void dragMoveEvent(QDragMoveEvent *e) override;
    void dropEvent(QDropEvent *e) override;

private slots:
    void onBottonFoldClicked();
    void onModelRowInserted(const QModelIndex &parent, int start, int end);

private:
    Global::Category m_category;
    QString m_strGroupName;
    QVBoxLayout *m_pLayout;
    PageButton *m_pButton;
    MyListView *m_pListView;

    MyToolBox *m_parent;
    ComponentProxyModel *m_pProxyModel;
    QMap<int, QVariant> m_mapGroupRoleTempalte; // 保存该分组的数据模板，对于添加到此分组的元素，将修改其数据
};

// 自定义button
class PageButton : public QWidget
{
    Q_OBJECT
public:
    PageButton(const QString &strText, QWidget *parent = nullptr,const bool &isExpand = true);
    void setExpand(bool bExpand);
    bool isExpand() const;

signals:
    void clicked();
    void expandChanged();

protected:
    virtual void mousePressEvent(QMouseEvent *) override;
    virtual void mouseReleaseEvent(QMouseEvent *) override;
    virtual void enterEvent(QEvent *) override;
    virtual void leaveEvent(QEvent *) override;
    virtual void paintEvent(QPaintEvent *) override;
    virtual QSize sizeHint() const override;

private:
    QString m_strText;
    bool m_bExpand;
    bool m_bPressed;
};

#endif // MYTOOLBOX_H
