#ifndef BOOKMARK_H
#define BOOKMARK_H

#include "ActionManager.h"
#include "CanvasContext.h"

#include <QLineEdit>
#include <QMouseEvent>
#include <QPushButton>
#include <QSharedPointer>
#include <QWidget>
#include <QWidgetAction>
#include <qfiledialog.h>
#include <qheaderview.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlistwidget.h>
#include <qmenu.h>
#include <qtablewidget.h>
#include <qvector.h>

#define BOOKMARK_MAXNUMBER 64

class CanvasContext;
class SearchLineEdit;
class SourceProxy;
class ActionManager;
class BookMarkPrivate;
class BookMarkTableWidget;
class KCustomDialog;
class KLineEdit;

class GRAPHICSMODELINGKERNEL_EXPORT BookMark : public QWidget
{
    Q_OBJECT
    friend class BookMarkTableWidget;

private:
    BookMark(QWidget *parent = 0, Qt::WindowFlags f = 0);

public:
    /// @brief 书签需要单例
    static BookMark &getInstance();
    /// @brief 添加书签弹窗
    void AddBookMarkWidegt();
    /// @brief 添加书签弹窗功能
    void actionCreateAddBookmark();
    /// @brief 管理书签界面功能
    void ManageBookMarkWidegt();
    /// @brief 处理重命名
    QString findRename(QString str);
    /// @brief 更新显示
    void updateListWidget();
    /// @brief 根据枚举获取画板名
    QString readBroadName(QString i_type);
    /// @brief 设置画板信息
    void setCanvasContext(QSharedPointer<CanvasContext> a);
    /// @brief 获取书签菜单列表
    QMenu *getBookMarkMenu();
    /// @brief 通过UUID删除书签
    void deleteBookMarkAction(QString uid);
    /// @brief 书签保存的数据
    QVariantMap savedata(QString ls, QString rs);
    /// @brief menu清除重排
    void realignment();
    /// @brief 序列化
    void serialize();
    /// @brief 反序列化
    void deserialize(QVariantMap data);
    /// @brief 判断该行是否被选中过
    /// @param vule item所在行
    /// @return T/F
    bool alreadyExisted(int vule);
    /// @brief 获取本地时间
    QString getLocalTime();
    /// @brief 画板改变，同步改变
    /// @param  老画板名
    /// @param  新画板名
    void changeBoardName(const QString oldnew, const QString newname);
    /// @brief 获取书签数量
    int getBookMarkNumbee();
    /// @brief 书签最大限制
    /// @return
    int getBookMarkMaxLimit();
    /// @brief 获取有快捷键的书签
    QList<QAction *> *getBookMarks();
    /// @brief 设置要删除的行
    /// @param row
    void setDeleteRow(QVector<int> row);
    /// @brief 设置删除按钮
    /// @param falg
    void setButtonDelete(bool falg);

signals:
    /// @brief 触发画板位置信号
    void previewSceneChanged(QString uuid, QPointF pos);
    /// @brief 删除画板
    /// @param uuid 删除画板的uuid
    void previewUUIDChanged(QString uuid);
    void previewScaleChanged(QString uuid, double scale);
    void bookMarkSerialize(QVariantMap);

    void bookMarkDeserialize();
public slots:
    /// @brief 使用书签
    void onUseBookmark();
    /// @brief 书签删除
    void deleteItemBookmark();
    /// @brief 删除按钮切换
    void deleteBookmark(QTableWidgetItem *item);
    /// @brief 管理书签弹窗
    void actionCreateManageBookmark();
    /// @brief 获取修改前的文本
    void changeItemBookmark(QTableWidgetItem *item);
    /// @brief 修改选中item的文本
    void changeBookmark(QTableWidgetItem *item);
    /// @brief 关闭时删除按钮置灰
    void undeletePtn(int value);

private:
    static QVector<QAction *> shortcut_falg; // 9个快捷键的标志数组
    QPushButton *m_Cancel;                   // 添加书签弹窗取消按钮
    QPushButton *m_OK;                       // 添加书签弹窗确定按钮
    QPushButton *m_delete;                   // 管理书签弹窗删除按钮
    KCustomDialog *m_addDlg;                 // 添加书签弹窗
    KLineEdit *m_line;                       // 工具栏书签弹窗文本框
    QList<QAction *> *m_allaction;           // 保存所有新增有快捷键的action
    static QList<QString> *m_count;          // 计数重命名的标签
    KCustomDialog *m_manageDlg;              // 添加书签管理弹窗
    QSharedPointer<CanvasContext> data;      // 要保存的画板数据
    QString m_lastchangeText;                // 修改前的文本
    bool isfrist;                            // 是否第一次生成更多...
    QScopedPointer<BookMarkPrivate> dataPtr; // 保存数据
    QMenu *p_mroe;                           // 更多...二级菜单
    QTableWidget *m_mangertablewidget;       // 管理书签表
    int m_row;                               // 获取选中的第0列item
    bool isfristchangeditem;                 // 是否为第一次修改item
    bool isdoubleclicked;                    // 是否为第一次双击
    bool ismore;                             // 是否是更多里面的数据
    QString m_shortcutlastchangeText;        // 修改前的快捷键
    QVector<int> itemsRow;                   // 多个选中item的行
    QVector<int> itemsAllRow;                // ctrl+A选中item的行
};

class BookMarkPrivate
{
public:
    BookMarkPrivate();
    QMenu *getBookMarkMenu();
    QMenu *BookMarkList;
};

class BookMarkTableWidget : public QTableWidget
{
public:
    BookMarkTableWidget(BookMark *p, QWidget *parent = nullptr) : QTableWidget(parent), m_bookMark(p) { }

protected:
    void mousePressEvent(QMouseEvent *e);
    void keyPressEvent(QKeyEvent *event);
    void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected) override;

private:
    BookMark *m_bookMark;
};
#endif
