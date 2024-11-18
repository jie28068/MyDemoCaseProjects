#ifndef SEARCHBOX_H
#define SEARCHBOX_H

#include "Global_GMS.h"
#include "KLineEdit.h"
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QSharedPointer>
#include <QSortFilterProxyModel>
#include <QWidget>

struct searchListData {
    QString name;
    QString uuid;
    QString boardName; // 所属画板名称 如果是子系统的那么就是画板名/子系统画板名
    QStringList boardUuidPath;
    bool operator<(const searchListData &p) const { return name < p.name; }

    searchListData()
    {
        name = QString();
        uuid = QString();
        boardName = QString();
        boardUuidPath = QStringList();
    }
};

class SearchBox : public QWidget
{
    Q_OBJECT

public:
    SearchBox(QWidget *parent = nullptr);
    ~SearchBox();
    void setVisableStatus(bool visible);
    void setModel(PModel model);

signals:
    void selectedBlock(searchListData &data);
public slots:
    void onSetContentWidgetSize(int itemCount);

private:
    SearchLineEdit *m_searchLineEdit;
};

class SearchLineEdit : public KLineEdit
{
    Q_OBJECT
public:
    SearchLineEdit(QWidget *parent = 0);
    // 更新m_BlockDatalist
    void refreshBlockDataList();

protected:
    virtual void focusInEvent(QFocusEvent *e) override;

private:
    void insertDataListByBoard(PModel model, QList<searchListData> &dataList, QString boardPath,
                               QStringList pathUuidList);

    bool eventFilter(QObject *obj, QEvent *evt) override;

signals:
    // int 下拉框中item数量
    void displayPopupWidget(int);

    void selectedBlock(searchListData &data);

public slots:
    void onClearSearchText();

    void onTextChanged(const QString &);
    void onEditFinish();

    void onBlockClicked();

    void onBlockActivatedClicked(QListWidgetItem *item);

public:
    PModel m_model;

private:
    int nCurrenPopupItemCount;
    QPushButton *m_pClearButton;
    QList<searchListData> m_BlockDatalist; // 当前画板所有的元器件列表，包括子系统的
    QListWidget *m_pFilterDisplayPopup;    // 搜索框的下拉弹出框
};

#endif // SEARCHBOX_H
