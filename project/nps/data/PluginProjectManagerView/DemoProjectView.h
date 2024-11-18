#ifndef DEMOPROJECTVIEW_H
#define DEMOPROJECTVIEW_H

#include "ui_DemoProjectView.h"
#include <QStandardItemModel>
#include <QWidget>

static const int RelativePathRole = 300;
static const int NodeTypeRole = 301;
static const int FolderRole = 302;

static const QString NodeType_Project = "NodeType_Project";
static const QString RelativePath = "RelativePath";

class DemoInfo
{
public:
    QVariantMap projectConfig;   // 项目配置信息
    QVariantMap projectProperty; // 项目属性信息
};
Q_DECLARE_METATYPE(DemoInfo);
class DemoProjectView : public QWidget
{
    Q_OBJECT

public:
    DemoProjectView(QWidget *parent = 0);
    ~DemoProjectView();

    void setMessage(QString message);
    void setNameList(QStringList nameList, bool bworkspace = false);
    void setDemoMap(QMap<QString, QStringList> demoMap);
    void setSelectionMode(QAbstractItemView::SelectionMode mode);
    QStringList getSelectNames();
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

private:
    QIcon getFolderIcon(const QString &foldername);
    void resetGridView(const QString &folder);
    void setCurrentInfo(const QString &curprj, const QString &en_name);
    void deserializedDemoInfo(const QString &prjpath);

signals:
    void selectone(int index);

private:
    Ui::DemoProjectView ui;
    QStandardItemModel *m_treeModel;
    bool m_bworkspece, m_MultiSelect;
    QMap<QString, QStringList> m_folderList; // 分类及其对应的列表 最后一个是图标
    QMap<QString, QString> m_namepathMap;    // 项目名及其对应的路径
    QMap<QString, DemoInfo> m_infoMap;       // 项目名及其对应的属性、配置信息
    QStringList m_selectNames;               // 最终选择的结果

private slots:
    void onTreeItemClick(const QModelIndex &index);
    void onGridItemChecked(bool checked);
};
class GridItem : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(bool checked READ isChecked WRITE setChecked NOTIFY checkedChanged)
private:
    QLabel *m_svgLab;
    QLabel *m_textLab;
    bool m_checked;

public:
    GridItem(QWidget *parent = 0);
    ~GridItem();
    QString text() { return m_textLab->text(); };

    void setCurPrj(const QString &prjpath, const QString &englishname);
    void setItemImage();
    void setChecked(bool checked);
    bool isChecked() const;
public slots:
    void onSelectOne(int index);
signals:
    void checkedChanged(bool);

protected:
    void mousePressEvent(QMouseEvent *event);
};

#endif // DEMOPROJECTVIEW_H
