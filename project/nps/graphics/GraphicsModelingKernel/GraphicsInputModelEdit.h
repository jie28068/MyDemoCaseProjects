#ifndef GRAPHICSINPUTMODELEDIT_H
#define GRAPHICSINPUTMODELEDIT_H

#include "GraphicsKernelDefinition.h"
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QSharedPointer>
#include <QSortFilterProxyModel>
#include <QWidget>

class CanvasContext;
class CreateModelEdit;
class SimulationModelListView;

class CreateModelEditBox : public QWidget
{
    Q_OBJECT

public:
    CreateModelEditBox(QWidget *parent = nullptr);
    ~CreateModelEditBox();

    void setStatus(bool visible, bool actived);

    bool isActive();

    void setBlockList(const QString &canvasTypeString, QList<BlockStruct> &blocList);

    virtual bool eventFilter(QObject *obj, QEvent *evt) override;

    QSizeF getInitialSize();

    CreateModelEdit *searchLineEdit();

signals:
    void createBlock(QString protype, QString text);
public slots:
    void onSetContentWidgetSize(int itemCount);

protected:
    CreateModelEdit *m_searchLineEdit;

    bool isActived;

    QSizeF initialSize;
};

class CreateModelEdit : public QLineEdit
{
    Q_OBJECT
public:
    CreateModelEdit(QWidget *parent = 0);

    QSize sizeHint() const;

    int getPopWidth();

signals:
    // 下拉框中item数量
    void displayPopupWidget(int);

    void selectedBlockTypeName(QString);

    void createBlock(QString protype, QString text);

public slots:
    void onClearSearchText();
    void onTextChanged(const QString &);
    void onEditFinish();

    void onBlockSelected(QListWidgetItem *item);

public:
    int nCurrenPopupItemCount;
    QPushButton *m_pClearButton;
    SimulationModelListView *m_pFilterDisplayPopup; // 搜索框的下拉弹出框
    QList<BlockStruct> blockList;
};

#endif // GRAPHICSINPUTMODELEDIT_H
